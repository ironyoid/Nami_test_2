#ifndef __TCP_H
#define __TCP_H
#include "defines.h"
#define TCP_PORT 6350
typedef enum
{
    OKEY = 0,
    UNKNOW = 1,
    ERR = 3,
    ECRC = 4,
    ELEMENT_EXIST = 5,
    ELEMENT_DOSNT_EXIST = 6,
    OUT_OF_RANGE = 7,

} err_response_t;

struct instruction_t
{
    uint16_t length;
    uint16_t CRC;
};
SemaphoreHandle_t x_mac_List_semaphore;
TaskHandle_t tcp_server_handle;
void tcp_task(void *ptr);
int8_t check_mac_list(uint8_t *buf);
#endif