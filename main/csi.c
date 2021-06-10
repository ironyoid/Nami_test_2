#include "csi.h"
//#define DEBUG
static const char *TAG = "NAMI_TEST_CSI";

void _wifi_csi_cb(void *ctx, wifi_csi_info_t *data)
{
    wifi_csi_info_t d = data[0];

    portBASE_TYPE xStatus = xQueueSend(csi_queue, data, 0);
    if (xStatus == pdPASS)
    {
#ifdef DEBUG
        ESP_LOGI(TAG, "Success...");
#endif
    }
    else
    {
#ifdef DEBUG
        ESP_LOGI(TAG, "Could not add data to the queue...");
#endif
    }
   // vTaskDelay(0);
}

void csi_init() {

    ESP_ERROR_CHECK(esp_wifi_set_csi(1));

    wifi_csi_config_t configuration_csi;
    configuration_csi.lltf_en = 1;
    configuration_csi.htltf_en = 1;
    configuration_csi.stbc_htltf2_en = 1;
    configuration_csi.ltf_merge_en = 1;
    configuration_csi.channel_filter_en = 0;
    configuration_csi.manu_scale = 0;

    ESP_ERROR_CHECK(esp_wifi_set_csi_config(&configuration_csi));
    ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(&_wifi_csi_cb, NULL));
}