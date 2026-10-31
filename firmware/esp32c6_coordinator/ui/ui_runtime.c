#include "coordinator_ui.h"
#include "ui_view.h"
#include "ui_geometry.h"
#include "board_display.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include <string.h>

static ui_model_t model;
static ui_diagnostics_t diagnostics;
static portMUX_TYPE lock = portMUX_INITIALIZER_UNLOCKED;
static uint32_t now_ms(void) { return (uint32_t)(esp_timer_get_time() / 1000); }
static void put16(uint8_t *p, uint16_t v) { p[0] = v; p[1] = v >> 8; }
static void put32(uint8_t *p, uint32_t v) { put16(p, v); put16(p+2, v >> 16); }

static bool load_settings(uint8_t *rotation, uint8_t *timeout)
{
    nvs_handle_t handle;
    esp_err_t err=nvs_open("display",NVS_READONLY,&handle);
    if (err==ESP_ERR_NVS_NOT_FOUND) return true; /* Defaults need no write. */
    if (err!=ESP_OK) return false;
    uint8_t value;
    err=nvs_get_u8(handle,"rotation",&value);
    bool ok=err==ESP_ERR_NVS_NOT_FOUND || (err==ESP_OK && value<4);
    if (err==ESP_OK && value<4) *rotation=value;
    err=nvs_get_u8(handle,"timeout",&value);
    ok &= err==ESP_ERR_NVS_NOT_FOUND || (err==ESP_OK && value<4);
    if (err==ESP_OK && value<4) *timeout=value;
    nvs_close(handle);
    return ok;
}

static bool save_settings(const ui_model_t *m)
{
    nvs_handle_t handle;
    if (nvs_open("display",NVS_READWRITE,&handle)!=ESP_OK) return false;
    esp_err_t err=nvs_set_u8(handle,"rotation",m->rotation);
    if (err==ESP_OK) err=nvs_set_u8(handle,"timeout",m->timeout_index);
    if (err==ESP_OK) err=nvs_commit(handle);
    nvs_close(handle);
    return err==ESP_OK;
}

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
             (ui_model_fresh(&model,now) ? 4 : 0) | ((model.flags & UI_NETWORK_UP) ? 8 : 0) |
             (model.sleeping ? 16 : 0) | (diagnostics.settings_saved ? 32 : 0) |
             ((model.rotation & 3) << 6);
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
    uint16_t *tile = heap_caps_malloc(UI_MAX_WIDTH*UI_TILE_ROWS*sizeof(uint16_t), MALLOC_CAP_8BIT);
    uint16_t *wire = heap_caps_malloc(UI_MAX_WIDTH*UI_TILE_ROWS*sizeof(uint16_t), MALLOC_CAP_DMA);
    if (!tile || !wire) { heap_caps_free(tile); heap_caps_free(wire); vTaskDelete(NULL); return; }
    const bool ready = board_display_init() == ESP_OK;
    /* UI startup is asynchronous; app::init owns NVS initialization. Retry
     * reads briefly without initializing, erasing or blocking the NCP. */
    uint8_t rotation=0, timeout=2;
    bool saved=false;
    for (int i=0;ready && i<20;++i) {
        saved=load_settings(&rotation,&timeout);
        if (saved) break;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    portENTER_CRITICAL(&lock);
    diagnostics.panel_ready = ready;
    diagnostics.touch_ready = board_touch_present();
    diagnostics.settings_saved = saved;
    model.rotation=rotation; model.timeout_index=timeout;
    model.last_touch_ms=model.splash_started_ms=now_ms();
    portEXIT_CRITICAL(&lock);
    if (!ready) { heap_caps_free(tile); heap_caps_free(wire); vTaskDelete(NULL); return; }
    bool was_pressed = false, redraw = true;
    bool awake=false;
    uint32_t rendered_at = 0;
    for (;;) {
        uint16_t x=0, y=0;
        const bool pressed = board_touch_read(&x,&y);
        const uint32_t now = now_ms();
        ui_model_t snapshot;
        ui_diagnostics_t health;
        bool settings_changed=false;
        portENTER_CRITICAL(&lock);
        if (pressed) model.last_touch_ms=now; /* Holding touch prevents sleep. */
        if (!diagnostics.touch_ready) model.last_touch_ms=now; /* Keep a screen without wake input visible. */
        if (pressed && !was_pressed && ui_geometry_touch(model.rotation,&x,&y)) {
            ++diagnostics.touches;
            const ui_action_t action=ui_model_touch(&model,x,y,now);
            redraw |= action!=UI_ACTION_NONE;
            settings_changed=action==UI_ACTION_SETTINGS;
        }
        redraw |= ui_model_tick(&model,now);
        diagnostics.uptime_s = now/1000;
        snapshot = model; health = diagnostics;
        portEXIT_CRITICAL(&lock);
        was_pressed = pressed;
        if (settings_changed) {
            const bool stored=save_settings(&snapshot);
            portENTER_CRITICAL(&lock);
            diagnostics.settings_saved=stored;
            portEXIT_CRITICAL(&lock);
            health.settings_saved=stored;
        }
        if (snapshot.sleeping) {
            if (awake) {
                const bool ok=board_display_set_awake(false)==ESP_OK;
                portENTER_CRITICAL(&lock);
                if (!ok) ++diagnostics.draw_errors;
                portEXIT_CRITICAL(&lock);
                awake=false;
            }
            vTaskDelay(pdMS_TO_TICKS(40));
            continue;
        }
        if (redraw || now-rendered_at >= 1000) {
            bool ok = true;
            const int height=ui_model_height(&snapshot);
            for (int yy=0; yy<height; yy+=UI_TILE_ROWS) {
                const int rows=height-yy<UI_TILE_ROWS?height-yy:UI_TILE_ROWS;
                int x0,y0,x1,y1;
                ui_view_render(tile,yy,rows,&snapshot,&health,now);
                if (!ui_geometry_pack(snapshot.rotation,yy,rows,tile,wire,&x0,&y0,&x1,&y1) ||
                    board_display_tile(x0,y0,x1,y1,wire) != ESP_OK) { ok=false; break; }
                taskYIELD();
            }
            if (ok && !awake) {
                ok=board_display_set_awake(true)==ESP_OK;
                awake=ok;
            }
            portENTER_CRITICAL(&lock);
            if (ok) ++diagnostics.frames; else ++diagnostics.draw_errors;
            portEXIT_CRITICAL(&lock);
            if (!ok) {
                /* A timed-out DMA transfer may still own wire. Never reuse/free it. */
                heap_caps_free(tile);
                vTaskDelete(NULL); return;
            }
            rendered_at = now; redraw=false;
        }
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

void coordinator_ui_start(void)
{
    ui_model_init(&model,now_ms());
    /* Failure is isolated to the screen; NCP startup must still proceed. */
    xTaskCreate(ui_task,"coordinator_ui",4096,NULL,1,NULL);
}
