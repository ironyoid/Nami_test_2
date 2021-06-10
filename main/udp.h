#ifndef __UDP_H
#define __UDP_H
#include "defines.h"
TaskHandle_t xTask_udp_handle;
TaskHandle_t xTask_udp_2_handle;
char udp_ip[64];
uint16_t udp_port;
struct list_of_mac_t
{
    uint8_t mac[6][MAC_LIST_LEN];
    uint8_t length;
}list_of_mac;
void udp_task(void *arg);
#endif