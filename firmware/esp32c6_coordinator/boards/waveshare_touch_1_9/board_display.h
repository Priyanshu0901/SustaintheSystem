#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
esp_err_t board_display_init(void);
esp_err_t board_display_tile(int y, int rows, uint16_t *rgb565);
bool board_touch_present(void);
/** Returns current pressed state; coordinates are portrait 170x320. */
bool board_touch_read(uint16_t *x, uint16_t *y);
