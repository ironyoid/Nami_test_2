#include "udp.h"
#include "csi.h"
static const char *TAG = "NAMI_TEST_UDP";
#define DEBUG
void udp_task(void *arg)
{
    portBASE_TYPE xStatus;
    int addr_family = 0;
    int ip_protocol = 0;
    static const char *payload = "Message from ESP32 ";

    while (1)
    {

        wifi_csi_info_t data;
        xStatus = xQueueReceive(csi_queue, &data, portMAX_DELAY);
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
#endif

            struct sockaddr_in dest_addr;
            dest_addr.sin_addr.s_addr = inet_addr(HOST_IP);
            dest_addr.sin_family = AF_INET;
            dest_addr.sin_port = htons(HOST_PORT);
            addr_family = AF_INET;
            ip_protocol = IPPROTO_IP;

            int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
            if (sock < 0)
            {
#ifdef DEBUG
                ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
#endif
            }
            else
            {
#ifdef DEBUG
                ESP_LOGI(TAG, "Socket created, sending to %s:%d", HOST_IP, HOST_PORT);
#endif
                int err = sendto(sock, &data.mac, sizeof(data.mac) + sizeof(data.rx_ctrl), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                //int err = sendto(sock, payload, strlen(payload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                if (err < 0)
                {
#ifdef DEBUG
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
#endif
                }
                else
                {
#ifdef DEBUG
                    ESP_LOGI(TAG, "Message sent");
#endif
                    if (sock != -1)
                    {
#ifdef DEBUG
                        ESP_LOGE(TAG, "Shutting down socket and restarting...");
#endif
                        shutdown(sock, 0);
                        close(sock);
                    }
                }
            }
        }
        else
        {
#ifdef DEBUG
            ESP_LOGE(TAG, "Could not recive data from the queue...");
#endif
        }
#ifdef DEBUG
        printf("\n");
#endif
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}