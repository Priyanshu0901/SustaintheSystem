#pragma once
#include "ui_model.h"
#include "lvgl.h"
typedef struct {
    uint32_t uptime_s, frames;
    uint16_t touches, draw_errors;
    bool panel_ready, touch_ready;
    bool settings_saved;
    uint16_t heap_kib;
    char device_id[18];
} ui_diagnostics_t;
typedef void (*ui_control_callback_t)(ui_control_t control, uint8_t value);
/** Construct the EEZ-generated widget tree. All calls stay on the UI task. */
void ui_view_create(ui_control_callback_t callback);
/** Bind truthful snapshots to widgets and adapt portrait/landscape geometry. */
void ui_view_update(const ui_model_t *model, const ui_diagnostics_t *diagnostics, uint32_t now_ms);
