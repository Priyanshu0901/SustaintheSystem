#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
esp_err_t board_display_init(void);
#define BOARD_LCD_TILE_PIXELS (320 * 32)
/** Queue a native portrait rectangle; returns without waiting for DMA.
 * UI task only. Call wait before submitting another rectangle. The RGB565
 * wire buffer stays alive/unchanged until wait succeeds; retain it on failure. */
esp_err_t board_display_submit(int x0, int y0, int x1, int y1, const uint16_t *rgb565);
/** Yield to FreeRTOS until the queued DMA completes (bounded timeout).
 * A transfer failure is terminal for this display instance. */
esp_err_t board_display_wait(void);
/** Turn panel/backlight off for display saving. Touch/radio remain active. */
esp_err_t board_display_set_awake(bool awake);
/** Set saved/live brightness (10..100%); retain it while the display sleeps. */
esp_err_t board_display_set_brightness(uint8_t percent);
bool board_touch_present(void);
/** Returns current pressed state; coordinates are portrait 170x320. */
bool board_touch_read(uint16_t *x, uint16_t *y);
