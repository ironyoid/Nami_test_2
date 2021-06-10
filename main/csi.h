#ifndef CSI_H
#define CSI_H
#include "defines.h"
void csi_init();
QueueHandle_t csi_queue;
SemaphoreHandle_t csi_semaphore;
#endif