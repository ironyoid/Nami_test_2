#ifndef WIFI_H
#define WIFI_H
#include "defines.h"
void sta_ap_init();
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num;
#endif