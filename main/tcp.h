#ifndef __TCP_H
#define __TCP_H
#include "defines.h"
#define TCP_PORT 6350
typedef enum {
    OKEY        = 0, 
    UNKNOW      = 1,
    ERR         = 3,
    ECRC        = 4,
    TIMEOUT     = 5, 
    NOACCESS    = 6
    } err_response_t;

    struct instruction_t
{
    uint16_t length;
    uint16_t CRC;
};

TaskHandle_t tcp_server_handle;
void tcp_server_task(void *ptr);
int8_t check_mac_list(uint8_t *buf);
#endif