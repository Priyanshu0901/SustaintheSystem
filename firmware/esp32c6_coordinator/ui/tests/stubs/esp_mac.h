#pragma once
#include <stdint.h>
enum { ESP_OK=0, ESP_MAC_BASE=7 };
int esp_read_mac(uint8_t *mac, int type);
