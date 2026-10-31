#pragma once
#include "ui_model.h"

/** Map native portrait touch to the current logical orientation (clockwise). */
bool ui_geometry_touch(uint8_t rotation, uint16_t *x, uint16_t *y);
/** Pack one logical tile into native portrait panel order and wire RGB565 bytes.
 * Output needs UI_MAX_WIDTH*UI_TILE_ROWS pixels, distinct from input.
 * Bounds are native half-open panel coordinates. Invalid input returns false. */
bool ui_geometry_pack(uint8_t rotation, int y, int rows, const uint16_t *input,
                      uint16_t *output, int *x0, int *y0, int *x1, int *y1);
