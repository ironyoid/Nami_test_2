#include "tcp.h"
#include "udp.h"
#define DEBUG
static const char *TAG = "NAMI_TEST_TCP";

uint16_t CRC16(uint8_t *DATA, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    uint8_t i;
    while (length--)
    {
        crc ^= *DATA++ << 8;
        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}

static void tcp_write(int s, const void *dataptr, int size, int flags)
{
    int to_write = size;
    while (to_write > 0)
    {
        int written = send(s, dataptr + (size - to_write), to_write, flags);
        if (written < 0)
        {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        }
        to_write -= written;
    }
}

void err_response(err_response_t response, int s, int flags)
{
    switch (response)
    {
    case OKEY:
        tcp_write(s, "\x02\x54\x41\x00\x00\x00\x01\x4C", 8, flags);
        break;
    case ERR:
        tcp_write(s, "\x02\x54\x41\xFF\x00\x00\xCE\x2F", 8, flags);
        break;
    case ECRC:
        tcp_write(s, "\x02\x54\x41\xFE\x00\x00\xF9\x1F", 8, flags);
        break;
    case TIMEOUT:
        tcp_write(s, "\x02\x54\x41\xFD\x00\x00\xA0\x4F", 8, flags);
        break;
    case NOACCESS:
        tcp_write(s, "\x02\x54\x41\xFC\x00\x00\x97\x7F", 8, flags);
        break;
    case UNKNOW:
        tcp_write(s, "\x02\x54\x41\xFF\x00\x00\xCE\x2F", 8, flags);
        break;
    }
}

void controller_read(int sock, int flags, uint8_t *buf, uint16_t count, uint16_t size)
{
    uint8_t EE_error_flag = 1;
    if (size >= 6)
    {
        sprintf((char *)udp_ip, "%d.%d.%d.%d", buf[count], buf[count + 1], buf[count + 2], buf[count + 3]);
        count += 4;
        ESP_LOGW(TAG, "UDP_IP %s\n", udp_ip);
        udp_port = ((uint16_t)buf[count] << 8) & 0xFF00;
        udp_port |= ((uint16_t)buf[count + 1]) & 0x00FF;
        count += 2;
        ESP_LOGW(TAG, "UDP_PORT %d\n", udp_port);
        
    }
    else
    {
        EE_error_flag = 0;
    }
    if (EE_error_flag == 1)
    {
        err_response(OKEY, sock, flags);
    }
    else
    {
        err_response(ERR, sock, flags);
    }
}

void tcp_server_task(void *pvParameters)
{
    char addr_str[128];
    uint16_t CRC;
    int addr_family = AF_INET;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;
    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = AF_INET;
    dest_addr_ip4->sin_port = htons(TCP_PORT);
    ip_protocol = IPPROTO_IP;

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0)
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        close(listen_sock);
        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "Socket bound, port %d", TCP_PORT);

    err = listen(listen_sock, 1);
    if (err != 0)
    {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        close(listen_sock);
        vTaskDelete(NULL);
    }

    while (1)
    {

        int len;
        uint8_t rx_buffer[128];
        struct instruction_t instruction;
        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        uint addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Convert ip address to string
        if (source_addr.ss_family == PF_INET)
        {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }
        else if (source_addr.ss_family == PF_INET6)
        {
            inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
        }
        ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

        do
        {
            uint16_t count = 0;
            len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0)
            {
                ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
            }
            else if (len == 0)
            {
                ESP_LOGW(TAG, "Connection closed");
            }
            else
            {
                rx_buffer[len] = 0; // Null-terminate whatever is received and treat it like a string
                ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);

                if ((rx_buffer[count] == 0x02) && (rx_buffer[count + 1] == 'T') && (rx_buffer[count + 2] == 'Q'))
                {
                    count += 3;
                    instruction.length = ((uint16_t)rx_buffer[count] << 8) & 0xFF00;
                    instruction.length |= ((uint16_t)rx_buffer[count + 1]) & 0x00FF;
#ifdef DEBUG
                    ESP_LOGI(TAG, "instruction.length %d", instruction.length);
#endif
                    count += 2;
                    instruction.CRC = (((uint16_t)rx_buffer[count + instruction.length] << 8) & 0xFF00);
                    instruction.CRC |= ((uint16_t)rx_buffer[count + instruction.length + 1]) & 0x00FF;
                    CRC = CRC16(rx_buffer, len - 2);
                    ESP_LOGI(TAG, "CRC16 %d", CRC);
                    if (instruction.CRC == CRC)
                    {
                        controller_read(sock, 0, rx_buffer, count, instruction.length);
                    }
                    else
                    {
                        err_response(ECRC, sock, 0);
#ifdef DEBUG
                        ESP_LOGE(TAG, "false crc");
#endif
                    }
                }
                else
                {
                    ESP_LOGE(TAG, "Command not recognized 02TQ");
                    err_response(UNKNOW, sock, 0);
                }
            }
        } while (len > 0);

        shutdown(sock, 0);
        close(sock);
    }
}