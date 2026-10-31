#include "ui_model.h"
#include <string.h>

static uint16_t read16(const uint8_t *p) { return (uint16_t)p[0] | ((uint16_t)p[1] << 8); }

bool ui_model_apply(ui_model_t *m, const uint8_t *d, size_t n, uint32_t now)
{
    if (!m || !d || n != 9 || d[0] != UI_PROTOCOL_VERSION || (d[1] & ~7U)) return false;
    const uint16_t joined = read16(d + 3), available = read16(d + 5), stale = read16(d + 7);
    if ((d[2] != 0 && (d[2] < 11 || d[2] > 26)) || joined > 200 ||
        available > joined || stale > joined - available) return false;
    if ((d[1] & UI_NETWORK_UP) && d[2] == 0) return false;
    m->received = true;
    m->flags = d[1]; m->channel = d[2];
    m->joined = joined; m->available = available; m->stale = stale;
    m->last_update_ms = now;
    return true;
}

bool ui_model_fresh(const ui_model_t *m, uint32_t now)
{
    return m->received && (uint32_t)(now - m->last_update_ms) < UI_HOST_TIMEOUT_MS;
}

void ui_model_init(ui_model_t *m, uint32_t now)
{
    memset(m, 0, sizeof *m);
    m->timeout_index = 2;
    m->brightness=m->brightness_committed=100;
    m->splash = true;
    m->last_touch_ms = m->splash_started_ms = now;
}

int ui_model_width(const ui_model_t *m) { return (m->rotation & 1) ? UI_HEIGHT : UI_WIDTH; }
int ui_model_height(const ui_model_t *m) { return (m->rotation & 1) ? UI_WIDTH : UI_HEIGHT; }
uint32_t ui_model_timeout_ms(const ui_model_t *m)
{
    static const uint32_t timeouts[] = {0,15000,30000,60000};
    return timeouts[m->timeout_index & 3];
}

bool ui_model_tick(ui_model_t *m, uint32_t now)
{
    bool changed = false;
    if (m->splash && (uint32_t)(now-m->splash_started_ms) >= UI_SPLASH_MS) {
        m->splash = false;
        changed = true;
    }
    const uint32_t timeout = ui_model_timeout_ms(m);
    if (!m->sleeping && !m->splash && timeout && (uint32_t)(now-m->last_touch_ms) >= timeout) {
        m->sleeping = true;
        changed = true;
    }
    return changed;
}

ui_action_t ui_model_touch(ui_model_t *m, uint16_t x, uint16_t y, uint32_t now)
{
    if (x >= ui_model_width(m) || y >= ui_model_height(m)) return UI_ACTION_NONE;
    m->last_touch_ms = now;
    if (m->sleeping) { m->sleeping = false; return UI_ACTION_WAKE; }
    if (m->splash) { m->splash = false; return UI_ACTION_REDRAW; }
    return UI_ACTION_NONE;
}

ui_action_t ui_model_control(ui_model_t *m, ui_control_t control, uint8_t value)
{
    if ((control==UI_CONTROL_BRIGHTNESS_PREVIEW || control==UI_CONTROL_BRIGHTNESS) && value>=10 && value<=100) {
        const bool changed=m->brightness!=value;
        m->brightness=value;
        if (control==UI_CONTROL_BRIGHTNESS && m->brightness_committed!=value) {
            m->brightness_committed=value;
            return UI_ACTION_SETTINGS;
        }
        return changed?UI_ACTION_REDRAW:UI_ACTION_NONE;
    }
    if (control==UI_CONTROL_PAGE && value<UI_PAGE_COUNT && m->page!=value) {
        m->page=value;
        return UI_ACTION_REDRAW;
    }
    if (control==UI_CONTROL_ROTATION && value<4 && m->rotation!=value) {
        m->rotation=value;
        return UI_ACTION_SETTINGS;
    }
    if (control==UI_CONTROL_TIMEOUT && value<4 && m->timeout_index!=value) {
        m->timeout_index=value;
        return UI_ACTION_SETTINGS;
    }
    return UI_ACTION_NONE;
}
