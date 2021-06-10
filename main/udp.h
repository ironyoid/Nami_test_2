#ifndef __UDP_H
#define __UDP_H
#include "defines.h"
TaskHandle_t xTask_udp_handle;
TaskHandle_t xTask_udp_2_handle;
char udp_ip[64];
uint16_t udp_port;
void udp_task(void *arg);
#endif