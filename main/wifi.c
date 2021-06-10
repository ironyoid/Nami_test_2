#include "wifi.h"
static const char *TAG = "NAMI_TEST_WIFI";
static esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa((const ip4_addr_t *) &event->event_info.got_ip.ip_info.ip));
            s_retry_num = 0;
            xEventGroupSetBits(s_wifi_event_group, BIT0);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED: {
            if (s_retry_num < 10) {
                esp_wifi_connect();
                xEventGroupClearBits(s_wifi_event_group, BIT0);
                s_retry_num++;
                ESP_LOGI(TAG, "retry to connect to the AP");
            }
            ESP_LOGI(TAG, "connect to the AP fail\n");
            break;
        }
        default:
            break;
    }
    return ESP_OK;
}

void sta_ap_init()
{
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));


    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_config_t sta_config = {
            .sta = {
                    .ssid = ESP_WIFI_SSID,
                    .password = ESP_WIFI_PASS,
                    .channel = 8,
            },
    };
    wifi_config_t ap_config = {
        .ap = {
                    .ssid = "NAMI_test_2",
                    .password = "1234567890",
                    .max_connection = EXAMPLE_MAX_STA_CONN,
                    .authmode = WIFI_AUTH_WPA_WPA2_PSK,
                    .channel = 8,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_wifi_set_ps(WIFI_PS_NONE);

    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
}