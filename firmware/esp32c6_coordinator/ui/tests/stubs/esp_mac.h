#pragma once
#include <stdint.h>
#include "esp_err.h"
enum { ESP_MAC_BASE=7 };
int esp_read_mac(uint8_t *mac, int type);
