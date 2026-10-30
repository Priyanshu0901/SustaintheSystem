#include "coordinator_ui.h"
#include "ui_view.h"
#include "board_display.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static ui_model_t model;
static ui_diagnostics_t diagnostics;
static portMUX_TYPE lock = portMUX_INITIALIZER_UNLOCKED;
static uint32_t now_ms(void) { return (uint32_t)(esp_timer_get_time() / 1000); }
static void put16(uint8_t *p, uint16_t v) { p[0] = v; p[1] = v >> 8; }
static void put32(uint8_t *p, uint32_t v) { put16(p, v); put16(p+2, v >> 16); }

size_t coordinator_ui_command(uint16_t cmd, const uint8_t *data, size_t n, uint8_t *out, size_t cap)
{
    if (cmd != UI_STATUS_COMMAND && cmd != UI_DIAGNOSTICS_COMMAND) return 0;
    if (cap < 21) return 0;
    const uint32_t now = now_ms();
    const uint16_t heap_kb = esp_get_free_heap_size()/1024;
    portENTER_CRITICAL(&lock);
    bool valid = cmd == UI_STATUS_COMMAND ? ui_model_apply(&model,data,n,now) : n == 0;
    out[0] = 0; out[1] = valid ? 0 : 10;
    out[2] = UI_PROTOCOL_VERSION;
    out[3] = (diagnostics.panel_ready ? 1 : 0) | (diagnostics.touch_ready ? 2 : 0) |
             (ui_model_fresh(&model,now) ? 4 : 0) | ((model.flags & UI_NETWORK_UP) ? 8 : 0);
    out[4] = model.page;
    put16(out+5, diagnostics.touches); put32(out+7,diagnostics.frames);
    put16(out+11,diagnostics.draw_errors); put32(out+13,now/1000);
    const uint32_t age = model.received ? (uint32_t)(now-model.last_update_ms)/1000 : 65535;
    put16(out+17,age > 65535 ? 65535 : age);
    put16(out+19,heap_kb);
    portEXIT_CRITICAL(&lock);
    return 21;
}

static void ui_task(void *arg)
{
    uint16_t *tile = heap_caps_malloc(UI_WIDTH*UI_TILE_ROWS*sizeof(uint16_t), MALLOC_CAP_DMA);
    if (!tile) { vTaskDelete(NULL); return; }
    const bool ready = board_display_init() == ESP_OK;
    portENTER_CRITICAL(&lock);
    diagnostics.panel_ready = ready;
    diagnostics.touch_ready = board_touch_present();
    portEXIT_CRITICAL(&lock);
    if (!ready) { heap_caps_free(tile); vTaskDelete(NULL); return; }
    bool was_pressed = false, redraw = true;
    uint32_t rendered_at = 0;
    for (;;) {
        uint16_t x=0, y=0;
        const bool pressed = board_touch_read(&x,&y);
        const uint32_t now = now_ms();
        ui_model_t snapshot;
        ui_diagnostics_t health;
        portENTER_CRITICAL(&lock);
        if (pressed && !was_pressed) {
            ++diagnostics.touches;
            redraw |= ui_model_touch(&model,x,y);
        }
        diagnostics.uptime_s = now/1000;
        snapshot = model; health = diagnostics;
        portEXIT_CRITICAL(&lock);
        was_pressed = pressed;
        if (redraw || now-rendered_at >= 1000) {
            bool ok = true;
            for (int yy=0; yy<UI_HEIGHT; yy+=UI_TILE_ROWS) {
                ui_view_render(tile,yy,UI_TILE_ROWS,&snapshot,&health,now);
                if (board_display_tile(yy,UI_TILE_ROWS,tile) != ESP_OK) { ok=false; break; }
                taskYIELD();
            }
            portENTER_CRITICAL(&lock);
            if (ok) ++diagnostics.frames; else ++diagnostics.draw_errors;
            portEXIT_CRITICAL(&lock);
            if (!ok) {
                /* A timed-out DMA transfer may still own tile. Never reuse/free it. */
                vTaskDelete(NULL); return;
            }
            rendered_at = now; redraw=false;
        }
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

void coordinator_ui_start(void)
{
    /* Failure is isolated to the screen; NCP startup must still proceed. */
    xTaskCreate(ui_task,"coordinator_ui",4096,NULL,1,NULL);
}
