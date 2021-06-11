#include "defines.h"
#include "csi.h"
#include "wifi.h"
#include "udp.h"
#include "tcp.h"
static const char *TAG = "NAMI_TEST_MAIN";

/* It's necessary for Wi-Fi */
static void nvs_init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void app_main() {
    csi_queue = xQueueCreate(CSI_QUEUE_LEN, sizeof(wifi_csi_info_t));
    x_mac_List_semaphore = xSemaphoreCreateMutex(); 
    nvs_init();
    sta_ap_init();
    csi_init();
    xTaskCreate(&udp_task, "udp_task", 2048, NULL, 0, &xTask_udp_handle);      /* Priority is lower then Wi-Fi driver task (yep, recomendation from manual) */
    xTaskCreate(&tcp_task, "tcp_server", 4096, NULL, 10, &tcp_server_handle);  
}
