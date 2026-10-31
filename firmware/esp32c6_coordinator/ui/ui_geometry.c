#include "ui_geometry.h"

bool ui_geometry_touch(uint8_t rotation, uint16_t *x, uint16_t *y)
{
    if (rotation > 3 || *x >= UI_WIDTH || *y >= UI_HEIGHT) return false;
    const uint16_t px = *x, py = *y;
    switch (rotation) {
    case 1: *x = py; *y = UI_WIDTH-1-px; break;
    case 2: *x = UI_WIDTH-1-px; *y = UI_HEIGHT-1-py; break;
    case 3: *x = UI_HEIGHT-1-py; *y = px; break;
    default: break;
    }
    return true;
}

bool ui_geometry_pack(uint8_t r, int y, int rows, const uint16_t *in,
                      uint16_t *out, int *x0, int *y0, int *x1, int *y1)
{
    const int w = (r & 1) ? UI_HEIGHT : UI_WIDTH;
    const int h = (r & 1) ? UI_WIDTH : UI_HEIGHT;
    if (r > 3 || y < 0 || rows < 1 || rows > UI_TILE_ROWS || y+rows > h) return false;
    switch (r) {
    case 1: *x0=UI_WIDTH-y-rows; *x1=UI_WIDTH-y; *y0=0; *y1=UI_HEIGHT; break;
    case 2: *x0=0; *x1=UI_WIDTH; *y0=UI_HEIGHT-y-rows; *y1=UI_HEIGHT-y; break;
    case 3: *x0=y; *x1=y+rows; *y0=0; *y1=UI_HEIGHT; break;
    default: *x0=0; *x1=UI_WIDTH; *y0=y; *y1=y+rows; break;
    }
    for (int yy=0; yy<rows; ++yy) for (int x=0; x<w; ++x) {
        int px=x, py=y+yy;
        switch (r) {
        case 1: px=UI_WIDTH-1-y-yy; py=x; break;
        case 2: px=UI_WIDTH-1-x; py=UI_HEIGHT-1-y-yy; break;
        case 3: px=y+yy; py=UI_HEIGHT-1-x; break;
        default: break;
        }
        uint16_t c=in[yy*w+x];
        out[(py-*y0)*(*x1-*x0)+px-*x0]=(uint16_t)((c<<8)|(c>>8));
    }
    return true;
}
