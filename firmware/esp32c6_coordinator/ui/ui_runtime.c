#include "coordinator_ui.h"
#include "ui_view.h"
#include "ui_geometry.h"
#include "board_display.h"
#include "board_identity.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include <string.h>
#include <stdio.h>

static ui_model_t model;
static ui_diagnostics_t diagnostics;
static portMUX_TYPE lock = portMUX_INITIALIZER_UNLOCKED;
static uint16_t *wire_pixels;
static bool render_failed, ui_changed=true, settings_pending;
static bool input_pressed;
static uint16_t input_x, input_y;
static uint8_t frame_rotation;
static struct { uint8_t stage, rotation, library_rotation; uint32_t error; uint16_t x,y,width,height; } last_fault;
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

static void control_changed(ui_control_t control, uint8_t value)
{
    portENTER_CRITICAL(&lock);
    const ui_action_t action=ui_model_control(&model,control,value);
    settings_pending |= action==UI_ACTION_SETTINGS;
    ui_changed |= action!=UI_ACTION_NONE;
    portEXIT_CRITICAL(&lock);
}

static void read_pointer(lv_indev_t *input, lv_indev_data_t *data)
{
    (void)input;
    data->point.x=input_x; data->point.y=input_y;
    data->state=input_pressed?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;
}

static void flush_pixels(lv_display_t *display, const lv_area_t *area, uint8_t *pixels)
{
    int x0,y0,x1,y1;
    uint8_t stage=4;
    esp_err_t error=ESP_ERR_INVALID_ARG;
    const uint8_t adapter_rotation=frame_rotation;
    const uint8_t library_rotation=(uint8_t)lv_display_get_rotation(display);
    bool ok=!render_failed;
    if (ok) ok=library_rotation==LV_DISPLAY_ROTATION_0;
    if (ok) { stage=1; ok=ui_geometry_pack_rect(adapter_rotation,
        area->x1,area->y1,lv_area_get_width(area),lv_area_get_height(area),
        (const uint16_t *)pixels,wire_pixels,&x0,&y0,&x1,&y1); }
    if (ok) { stage=2; error=board_display_tile(x0,y0,x1,y1,wire_pixels); ok=error==ESP_OK; }
    portENTER_CRITICAL(&lock);
    if (!ok && !render_failed) {
        ++diagnostics.draw_errors; diagnostics.panel_ready=false;
        last_fault.stage=stage; last_fault.error=(uint32_t)error;
        last_fault.x=area->x1; last_fault.y=area->y1;
        last_fault.width=lv_area_get_width(area); last_fault.height=lv_area_get_height(area);
        last_fault.rotation=adapter_rotation; last_fault.library_rotation=library_rotation;
    }
    if (ok && lv_display_flush_is_last(display)) ++diagnostics.frames;
    portEXIT_CRITICAL(&lock);
    render_failed |= !ok;
    /* The LVGL draw buffer is separate from DMA storage. On failure, no
     * later flush may touch wire_pixels, even if the SPI callback arrives. */
    lv_display_flush_ready(display);
}

size_t coordinator_ui_command(uint16_t cmd, const uint8_t *data, size_t n, uint8_t *out, size_t cap)
{
    if (cmd==UI_FAULT_COMMAND) {
        if (cap<18) return 0;
        portENTER_CRITICAL(&lock);
        out[0]=0; out[1]=n==0?0:10; out[2]=UI_PROTOCOL_VERSION;
        out[3]=last_fault.stage; put32(out+4,last_fault.error);
        put16(out+8,last_fault.x); put16(out+10,last_fault.y);
        put16(out+12,last_fault.width); put16(out+14,last_fault.height);
        out[16]=last_fault.rotation;
        out[17]=last_fault.library_rotation;
        portEXIT_CRITICAL(&lock);
        return 18;
    }
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
    const size_t buffer_bytes=UI_MAX_WIDTH*UI_TILE_ROWS*sizeof(uint16_t);
    uint16_t *draw = heap_caps_malloc(buffer_bytes, MALLOC_CAP_8BIT);
    wire_pixels = heap_caps_malloc(buffer_bytes, MALLOC_CAP_DMA);
    if (!draw || !wire_pixels) { heap_caps_free(draw); heap_caps_free(wire_pixels); vTaskDelete(NULL); return; }
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
    char device_id[18]={0};
    board_identity_read(device_id);
    portENTER_CRITICAL(&lock);
    diagnostics.panel_ready = ready;
    diagnostics.touch_ready = board_touch_present();
    diagnostics.settings_saved = saved;
    memcpy(diagnostics.device_id,device_id,sizeof device_id);
    model.rotation=rotation; model.timeout_index=timeout;
    model.last_touch_ms=model.splash_started_ms=now_ms();
    portEXIT_CRITICAL(&lock);
    if (!ready) { heap_caps_free(draw); heap_caps_free(wire_pixels); vTaskDelete(NULL); return; }
    lv_init();
    lv_tick_set_cb(now_ms);
    lv_display_t *display=lv_display_create(UI_WIDTH,UI_HEIGHT);
    if (!display) { heap_caps_free(draw); heap_caps_free(wire_pixels); vTaskDelete(NULL); return; }
    lv_display_set_color_format(display,LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display,draw,NULL,buffer_bytes,LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display,flush_pixels);
    lv_indev_t *pointer=lv_indev_create();
    if (!pointer) { vTaskDelete(NULL); return; }
    lv_indev_set_type(pointer,LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(pointer,read_pointer);
    ui_view_create(control_changed);
    bool was_pressed = false, suppress_until_release=false;
    bool awake=false;
    uint32_t updated_at = 0;
    uint8_t applied_rotation=255;
    for (;;) {
        uint16_t x=0, y=0;
        const bool pressed = board_touch_read(&x,&y);
        const uint32_t now = now_ms();
        ui_model_t snapshot;
        ui_diagnostics_t health;
        const uint16_t heap_kib=esp_get_free_heap_size()/1024;
        portENTER_CRITICAL(&lock);
        if (pressed) model.last_touch_ms=now; /* Holding touch prevents sleep. */
        if (!diagnostics.touch_ready) model.last_touch_ms=now; /* Keep a screen without wake input visible. */
        const bool touch_valid=pressed && ui_geometry_touch(model.rotation,&x,&y);
        if (touch_valid && !was_pressed) {
            ++diagnostics.touches;
            const ui_action_t action=ui_model_touch(&model,x,y,now);
            ui_changed |= action!=UI_ACTION_NONE;
            suppress_until_release |= action==UI_ACTION_WAKE || action==UI_ACTION_REDRAW;
        }
        ui_changed |= ui_model_tick(&model,now);
        diagnostics.uptime_s = now/1000;
        diagnostics.heap_kib=heap_kib;
        snapshot = model; health = diagnostics;
        portEXIT_CRITICAL(&lock);
        was_pressed = pressed;
        if (!pressed) suppress_until_release=false;
        if (touch_valid) { input_x=x; input_y=y; }
        input_pressed=touch_valid && !suppress_until_release;
        if (settings_pending) {
            settings_pending=false;
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
        if (applied_rotation!=snapshot.rotation) {
            /* The board adapter owns both transforms. Keep LVGL unrotated
             * and change its logical resolution, avoiding a second input rotation. */
            frame_rotation=snapshot.rotation;
            lv_display_set_resolution(display,ui_model_width(&snapshot),ui_model_height(&snapshot));
            applied_rotation=snapshot.rotation;
            ui_changed=true;
        }
        if (ui_changed || now-updated_at>=1000) {
            ui_view_update(&snapshot,&health,now);
            ui_changed=false; updated_at=now;
        }
        if (!awake) lv_obj_invalidate(lv_screen_active());
        lv_timer_handler();
        if (render_failed) { heap_caps_free(draw); vTaskDelete(NULL); return; }
        if (!awake) {
            if (board_display_set_awake(true)==ESP_OK) awake=true;
            else {
                portENTER_CRITICAL(&lock); ++diagnostics.draw_errors; portEXIT_CRITICAL(&lock);
                vTaskDelete(NULL); return;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void coordinator_ui_start(void)
{
    ui_model_init(&model,now_ms());
    /* Failure is isolated to the screen; NCP startup must still proceed. */
    xTaskCreate(ui_task,"coordinator_ui",6144,NULL,1,NULL);
}
