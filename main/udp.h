#ifndef UDP_H
#define UDP_H
#include "defines.h"
TaskHandle_t xTask_udp_handle;
TaskHandle_t xTask_udp_2_handle;
void udp_task(void *arg);
void udp_task_2(void *pvParameters);
#endif