#include "defines.h"
#include "csi.h"
#include "wifi.h"
#include "udp.h"
/* FreeRTOS event group to signal when we are connected*/
static const char *TAG = "NAMI_TEST_MAIN";


void nvs_init() {
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void app_main() {
    csi_queue = xQueueCreate(CSI_QUEUE_LEN, sizeof(wifi_csi_info_t));
    csi_semaphore = xSemaphoreCreateMutex();
    nvs_init();
    sta_ap_init();
    csi_init();
    xTaskCreate(&udp_task, "udp_task", 2048, NULL, 32, &xTask_udp_handle);
    xTaskCreate(&udp_task_2, "udp_task_2", 2048, NULL, 10, &xTask_udp_2_handle);
}
