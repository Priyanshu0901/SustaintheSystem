#pragma once
#include "FreeRTOS.h"
void vTaskDelay(uint32_t ticks);
void vTaskDelete(void *task);
int xTaskCreate(void (*entry)(void *),const char *name,unsigned stack,void *arg,unsigned priority,void *handle);
