#include "ui_model.h"

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

bool ui_model_touch(ui_model_t *m, uint16_t x, uint16_t y)
{
    if (x >= UI_WIDTH || y < 282 || y >= UI_HEIGHT) return false;
    const uint8_t page = x < UI_WIDTH / 2 ? 0 : 1;
    const bool changed = m->page != page;
    m->page = page;
    return changed;
}
