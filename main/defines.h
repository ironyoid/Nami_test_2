#ifndef DEFINES_H
#define DEFINES_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"


#define LEN_MAC_ADDR 20
#define ESP_WIFI_SSID      "NAMI_test_2"
#define ESP_WIFI_PASS      "1234567890"
#define EXAMPLE_MAX_STA_CONN       16

#endif