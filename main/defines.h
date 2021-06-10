#ifndef __DEFINES_H
#define __DEFINES_H

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
#include "lwip/api.h"
#include "lwip/netdb.h"
#include "lwip/netbuf.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <lwip/netdb.h>

#define LEN_MAC_ADDR                20
#define ESP_WIFI_SSID               "Keenetic-5919"     /*Add your AP SSID here */
#define ESP_WIFI_PASS               "2kcdbL5b"          /*Add your AP password here */
#define EXAMPLE_MAX_STA_CONN        16
#define CSI_QUEUE_LEN               10
#define MAC_LIST_LEN                16                  /*Maximal number of transmitters */
#endif