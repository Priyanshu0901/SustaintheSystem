#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define UI_WIDTH 170
#define UI_HEIGHT 320
#define UI_TILE_ROWS 16
#define UI_MAX_WIDTH 320
#define UI_HOST_TIMEOUT_MS 15000U
#define UI_SPLASH_MS 1800U
#define UI_PROTOCOL_VERSION 1
#define UI_STATUS_COMMAND 0x7F10
#define UI_DIAGNOSTICS_COMMAND 0x7F11
#define UI_FAULT_COMMAND 0x7F12

enum { UI_NETWORK_UP = 1, UI_MQTT_UP = 2, UI_JOIN_OPEN = 4 };
typedef enum { UI_PAGE_NETWORK, UI_PAGE_DEBUG, UI_PAGE_SETTINGS, UI_PAGE_ABOUT, UI_PAGE_COUNT } ui_page_t;
typedef enum { UI_CONTROL_PAGE, UI_CONTROL_ROTATION, UI_CONTROL_TIMEOUT } ui_control_t;
typedef enum { UI_ACTION_NONE, UI_ACTION_REDRAW, UI_ACTION_SETTINGS, UI_ACTION_WAKE } ui_action_t;

/** Host-owned counts describe its device registry, excluding the coordinator.
 * Available/stale come from actual published Zigbee2MQTT availability, never
 * from a neighbor table. The remainder is explicitly unknown. */
typedef struct {
    bool received;
    uint8_t flags, channel, page;
    uint16_t joined, available, stale;
    uint32_t last_update_ms;
    bool sleeping, splash;
    uint8_t rotation, timeout_index;
    uint32_t last_touch_ms, splash_started_ms;
} ui_model_t;

/** Decode exactly nine little-endian bytes. Invalid input leaves model intact. */
bool ui_model_apply(ui_model_t *model, const uint8_t *data, size_t length, uint32_t now_ms);
bool ui_model_fresh(const ui_model_t *model, uint32_t now_ms);
/** Initialize local display settings; host values remain unknown. */
void ui_model_init(ui_model_t *model, uint32_t now_ms);
/** Screen-only controls. First touch while sleeping only wakes the display. */
ui_action_t ui_model_touch(ui_model_t *model, uint16_t x, uint16_t y, uint32_t now_ms);
/** Apply a validated LVGL control value. Re-selecting a value does not write NVS. */
ui_action_t ui_model_control(ui_model_t *model, ui_control_t control, uint8_t value);
/** Returns true on splash expiry or entry into display sleep. */
bool ui_model_tick(ui_model_t *model, uint32_t now_ms);
uint32_t ui_model_timeout_ms(const ui_model_t *model);
int ui_model_width(const ui_model_t *model);
int ui_model_height(const ui_model_t *model);
