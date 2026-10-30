#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define UI_WIDTH 170
#define UI_HEIGHT 320
#define UI_TILE_ROWS 16
#define UI_HOST_TIMEOUT_MS 15000U
#define UI_PROTOCOL_VERSION 1
#define UI_STATUS_COMMAND 0x7F10
#define UI_DIAGNOSTICS_COMMAND 0x7F11

enum { UI_NETWORK_UP = 1, UI_MQTT_UP = 2, UI_JOIN_OPEN = 4 };

/** Host-owned counts describe its device registry, excluding the coordinator.
 * Available/stale come from actual published Zigbee2MQTT availability, never
 * from a neighbor table. The remainder is explicitly unknown. */
typedef struct {
    bool received;
    uint8_t flags, channel, page;
    uint16_t joined, available, stale;
    uint32_t last_update_ms;
} ui_model_t;

/** Decode exactly nine little-endian bytes. Invalid input leaves model intact. */
bool ui_model_apply(ui_model_t *model, const uint8_t *data, size_t length, uint32_t now_ms);
bool ui_model_fresh(const ui_model_t *model, uint32_t now_ms);
/** Touch only changes the selected tab. It cannot open joining or erase data. */
bool ui_model_touch(ui_model_t *model, uint16_t x, uint16_t y);
