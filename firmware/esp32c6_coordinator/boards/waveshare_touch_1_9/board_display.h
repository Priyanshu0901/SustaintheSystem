#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
esp_err_t board_display_init(void);
/** Native portrait rectangle, RGB565 bytes already in panel wire order.
 * DMA buffer must be kept alive and unchanged until this call succeeds. */
esp_err_t board_display_tile(int x0, int y0, int x1, int y1, const uint16_t *rgb565);
/** Turn panel/backlight off for display saving. Touch/radio remain active. */
esp_err_t board_display_set_awake(bool awake);
/** Set saved/live brightness (10..100%); retain it while the display sleeps. */
esp_err_t board_display_set_brightness(uint8_t percent);
bool board_touch_present(void);
/** Returns current pressed state; coordinates are portrait 170x320. */
bool board_touch_read(uint16_t *x, uint16_t *y);
