#pragma once
#include "ui_model.h"
typedef struct {
    uint32_t uptime_s, frames;
    uint16_t touches, draw_errors;
    bool panel_ready, touch_ready;
} ui_diagnostics_t;
/** Render a portrait tile to host-endian RGB565. Caller supplies width*rows pixels.
 * Pure C: no ESP-IDF, heap allocation, radio or panel calls. */
void ui_view_render(uint16_t *pixels, int y, int rows, const ui_model_t *model,
                    const ui_diagnostics_t *diagnostics, uint32_t now_ms);
