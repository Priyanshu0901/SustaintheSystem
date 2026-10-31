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
    if (rows<1 || rows>UI_TILE_ROWS) return false;
    return ui_geometry_pack_rect(r,0,y,w,rows,in,out,x0,y0,x1,y1);
}

bool ui_geometry_pack_rect(uint8_t r, int x, int y, int width, int rows, const uint16_t *in,
                           uint16_t *out, int *x0, int *y0, int *x1, int *y1)
{
    const int w = (r & 1) ? UI_HEIGHT : UI_WIDTH;
    const int h = (r & 1) ? UI_WIDTH : UI_HEIGHT;
    if (r>3 || x<0 || y<0 || width<1 || rows<1 || width>w || rows>h ||
        x>w-width || y>h-rows || width*rows>UI_MAX_WIDTH*UI_TILE_ROWS) return false;
    switch (r) {
    case 1: *x0=UI_WIDTH-y-rows; *x1=UI_WIDTH-y; *y0=x; *y1=x+width; break;
    case 2: *x0=UI_WIDTH-x-width; *x1=UI_WIDTH-x; *y0=UI_HEIGHT-y-rows; *y1=UI_HEIGHT-y; break;
    case 3: *x0=y; *x1=y+rows; *y0=UI_HEIGHT-x-width; *y1=UI_HEIGHT-x; break;
    default: *x0=x; *x1=x+width; *y0=y; *y1=y+rows; break;
    }
    for (int yy=0; yy<rows; ++yy) for (int xx=0; xx<width; ++xx) {
        int px=x+xx, py=y+yy;
        switch (r) {
        case 1: px=UI_WIDTH-1-y-yy; py=x+xx; break;
        case 2: px=UI_WIDTH-1-x-xx; py=UI_HEIGHT-1-y-yy; break;
        case 3: px=y+yy; py=UI_HEIGHT-1-x-xx; break;
        default: break;
        }
        uint16_t c=in[yy*width+xx];
        out[(py-*y0)*(*x1-*x0)+px-*x0]=(uint16_t)((c<<8)|(c>>8));
    }
    return true;
}
