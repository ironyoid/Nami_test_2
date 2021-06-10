#include "csi.h"
static const char *TAG = "NAMI_TEST_CSI";

void _wifi_csi_cb(void *ctx, wifi_csi_info_t *data)
{
    wifi_csi_info_t d = data[0];
    char mac[20] = {0};
#ifdef DEBUG
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", d.mac[0], d.mac[1], d.mac[2], d.mac[3], d.mac[4], d.mac[5]);
    ESP_LOGI(TAG, "CSI_DATA");
    ESP_LOGI(TAG, "%s,", mac);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.rssi);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.rate);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.sig_mode);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.mcs);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.cwb);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.smoothing);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.not_sounding);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.aggregation);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.stbc);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.fec_coding);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.sgi);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.noise_floor);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.ampdu_cnt);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.channel);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.secondary_channel);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.timestamp);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.ant);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.sig_len);
    ESP_LOGI(TAG, "%d,", d.rx_ctrl.rx_state);
    printf("\n");
#endif
    portBASE_TYPE xStatus = xQueueSend(csi_queue, data, portMAX_DELAY);
    if (xStatus == pdPASS)
    {
        ESP_LOGI(TAG, "Success...");
    }
    else
    {
        ESP_LOGI(TAG, "Could not add data to the queue...");
    }
    //vTaskDelay(0);
}

void csi_init() {

    ESP_ERROR_CHECK(esp_wifi_set_csi(1));

    // @See: https://github.com/espressif/esp-idf/blob/master/components/esp_wifi/include/esp_wifi_types.h#L401
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