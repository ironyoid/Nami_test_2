#include "wifi.h"
#define DEBUG
static const char *TAG = "NAMI_TEST_WIFI";
/**
 * @brief   Wi-Fi event handler (just from example)
 * @return  void
 * @note    
 */
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 20) {
            esp_wifi_connect();
            s_retry_num++;
#ifdef DEBUG
            ESP_LOGI(TAG, "retry to connect to the AP");
#endif
        } else {
            xEventGroupSetBits(s_wifi_event_group, BIT1);
        }
#ifdef DEBUG
        ESP_LOGI(TAG,"connect to the AP fail");
#endif
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
 #ifdef DEBUG
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
#endif
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, BIT0);
    }
}
/**
 * @brief   Station init (just from example)
 * @return  void
 * @note    
 */
void sta_ap_init()
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
#ifdef DEBUG
    ESP_LOGI(TAG, "wifi_init_sta finished.");
#endif
    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT)
    {
#ifdef DEBUG
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
#endif                 
    }
    else if (bits & WIFI_FAIL_BIT)
    {
#ifdef DEBUG
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
#endif  
    }
    else
    {
#ifdef DEBUG
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
#endif 
    }
    const wifi_promiscuous_filter_t filt = {
            .filter_mask = WIFI_PROMIS_FILTER_MASK_DATA
    };

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
    esp_wifi_set_promiscuous(true);             /* For scan all Wi-Fi */
    esp_wifi_set_promiscuous_filter(&filt);
    esp_wifi_set_ps(WIFI_PS_NONE);              /* Turn off power save mode */
#ifdef DEBUG
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s", ESP_WIFI_SSID, ESP_WIFI_PASS);
#endif 
}