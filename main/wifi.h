#ifndef __WIFI_H
#define __WIFI_H
#include "defines.h"
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define EXAMPLE_ESP_MAXIMUM_RETRY 10
void sta_ap_init();
EventGroupHandle_t s_wifi_event_group;
int s_retry_num;
#endif