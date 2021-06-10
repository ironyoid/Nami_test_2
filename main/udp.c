#include "udp.h"
#include "csi.h"
static const char *TAG = "NAMI_TEST_UDP";

void udp_task(void *arg)
{
    portBASE_TYPE xStatus;
    while (1)
    {
        wifi_csi_info_t data;
        xStatus = xQueueReceive(csi_queue, &data, 100 / portTICK_RATE_MS);
        if (xStatus == pdPASS)
        {
            char mac[20] = {0};
#ifdef DEBUG
            ESP_LOGE(TAG, "Success...");
            sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", data.mac[0], data.mac[1], data.mac[2], data.mac[3], data.mac[4], data.mac[5]);
            ESP_LOGE(TAG, "CSI_DATA");
            ESP_LOGE(TAG, "%s,", mac);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.rssi);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.rate);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.sig_mode);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.mcs);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.cwb);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.smoothing);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.not_sounding);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.aggregation);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.stbc);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.fec_coding);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.sgi);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.noise_floor);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.ampdu_cnt);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.channel);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.secondary_channel);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.timestamp);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.ant);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.sig_len);
            ESP_LOGE(TAG, "%d,", data.rx_ctrl.rx_state);
            printf("\n");
#endif
        }
        else
        {
            ESP_LOGE(TAG, "Could not recive data from the queue...");
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void udp_task_2(void *pvParameters)
{
    char rx_buffer[128];
    char host_ip[] = HOST_IP;
    int addr_family = 0;
    int ip_protocol = 0;
    static const char *payload = "Message from ESP32 ";
    while (1)
    {

        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(HOST_IP);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(HOST_PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, sending to %s:%d", HOST_IP, HOST_PORT);

        int err = sendto(sock, payload, strlen(payload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0)
        {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Message sent");

        if (sock != -1)
        {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
            vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}