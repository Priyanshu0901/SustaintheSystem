#include "ui_view.h"
#include "ui_theme.h"
#include <stdio.h>
#include <string.h>

/* Original compact 5x7 uppercase glyphs, one row per byte. */
static const char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-:.?/ ";
static const uint8_t glyphs[][7] = {
 {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},
 {30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
 {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},
 {14,17,17,15,1,1,14},{14,17,17,31,17,17,17},{30,17,17,30,17,17,30},
 {14,17,16,16,16,17,14},{30,17,17,17,17,17,30},{31,16,16,30,16,16,31},
 {31,16,16,30,16,16,16},{14,17,16,23,17,17,15},{17,17,17,31,17,17,17},
 {14,4,4,4,4,4,14},{7,2,2,2,18,18,12},{17,18,20,24,20,18,17},
 {16,16,16,16,16,16,31},{17,27,21,21,17,17,17},{17,25,21,19,17,17,17},
 {14,17,17,17,17,17,14},{30,17,17,30,16,16,16},{14,17,17,17,21,18,13},
 {30,17,17,30,20,18,17},{15,16,16,14,1,1,30},{31,4,4,4,4,4,4},
 {17,17,17,17,17,17,14},{17,17,17,17,17,10,4},{17,17,17,21,21,27,17},
 {17,17,10,4,10,17,17},{17,17,10,4,4,4,4},{31,1,2,4,8,16,31},
 {0,0,0,31,0,0,0},{0,4,4,0,4,4,0},{0,0,0,0,0,4,4},
 {14,17,1,2,4,0,4},{1,2,2,4,8,8,16},{0,0,0,0,0,0,0}
};
typedef struct { uint16_t *p; int y, rows; } canvas_t;
static void rect(canvas_t *c, int x, int y, int w, int h, uint16_t color)
{
    for (int yy = y; yy < y+h; ++yy) {
        if (yy < c->y || yy >= c->y+c->rows) continue;
        for (int xx = x; xx < x+w; ++xx)
            if (xx >= 0 && xx < UI_WIDTH) c->p[(yy-c->y)*UI_WIDTH+xx] = color;
    }
}
static void text(canvas_t *c, int x, int y, const char *s, int scale, uint16_t color)
{
    for (; *s; ++s, x += 6*scale) {
        const char *g = strchr(alphabet, *s);
        const unsigned index = g ? (unsigned)(g-alphabet) : 39;
        for (int row=0; row<7; ++row)
            for (int col=0; col<5; ++col)
                if (glyphs[index][row] & (1 << (4-col)))
                    rect(c,x+col*scale,y+row*scale,scale,scale,color);
    }
}
static void value(canvas_t *c, int x, int y, uint32_t n, bool valid, int scale, uint16_t color)
{
    char buf[16];
    if (valid) snprintf(buf,sizeof buf,"%lu",(unsigned long)n); else strcpy(buf,"--");
    text(c,x,y,buf,scale,color);
}
void ui_view_render(uint16_t *p, int y, int rows, const ui_model_t *m,
                    const ui_diagnostics_t *d, uint32_t now)
{
    canvas_t c = {p,y,rows};
    const bool fresh = ui_model_fresh(m,now);
    const bool online = fresh && (m->flags & UI_NETWORK_UP);
    rect(&c,0,y,UI_WIDTH,rows,UI_BG);
    text(&c,12,12,"SUSTAIN",2,UI_TEXT);
    text(&c,12,34,"ZIGBEE COORDINATOR",1,UI_MUTED);
    rect(&c,12,54,146,49,UI_CARD);
    rect(&c,20,65,5,5,online?UI_OK:UI_WARN);
    text(&c,32,64,online?"NETWORK READY":"HOST WAITING",1,online?UI_OK:UI_WARN);
    text(&c,20,83,fresh?((m->flags&UI_MQTT_UP)?"USB / MQTT ONLINE":"MQTT DISCONNECTED"):
         (m->received?"HOST DATA EXPIRED":"CONNECT PC HOST"),1,UI_MUTED);
    if (m->page == 0) {
        text(&c,12,119,"REGISTERED NODES",1,UI_MUTED);
        value(&c,12,137,m->joined,fresh,5,UI_TEXT);
        text(&c,117,139,"CH",1,UI_MUTED);
        value(&c,117,154,m->channel,fresh&&m->channel,2,UI_ACCENT);
        rect(&c,12,190,146,67,UI_CARD);
        text(&c,21,201,"AVAILABLE",1,UI_MUTED);
        text(&c,106,201,"STALE",1,UI_MUTED);
        value(&c,21,216,m->available,fresh,3,UI_OK);
        value(&c,106,216,m->stale,fresh,3,UI_WARN);
        const unsigned unknown = m->joined - m->available - m->stale;
        char buf[28];
        if (!fresh) strcpy(buf,"WAITING FOR HOST DATA");
        else snprintf(buf,sizeof buf,"UNKNOWN %u  JOIN %s",unknown,(m->flags&UI_JOIN_OPEN)?"OPEN":"SHUT");
        text(&c,12,267,buf,1,UI_MUTED);
    } else {
        text(&c,12,119,"BOARD / HEALTH",1,UI_ACCENT);
        text(&c,12,143,"LCD",1,UI_MUTED); text(&c,82,143,d->panel_ready?"READY":"ERROR",1,UI_TEXT);
        text(&c,12,164,"TOUCH",1,UI_MUTED); text(&c,82,164,d->touch_ready?"READY":"MISSING",1,UI_TEXT);
        text(&c,12,185,"UPTIME S",1,UI_MUTED); value(&c,82,185,d->uptime_s,true,1,UI_TEXT);
        text(&c,12,206,"TOUCHES",1,UI_MUTED); value(&c,82,206,d->touches,true,1,UI_TEXT);
        text(&c,12,227,"FRAMES",1,UI_MUTED); value(&c,82,227,d->frames,true,1,UI_TEXT);
        text(&c,12,248,"DRAW ERR",1,UI_MUTED); value(&c,82,248,d->draw_errors,true,1,UI_TEXT);
        text(&c,12,267,"WAVESHARE C6 / 8MB",1,UI_MUTED);
    }
    rect(&c,0,282,170,1,UI_LINE);
    rect(&c,m->page?85:0,282,85,3,UI_ACCENT);
    text(&c,20,299,"NETWORK",1,m->page?UI_MUTED:UI_ACCENT);
    text(&c,110,299,"SYSTEM",1,m->page?UI_ACCENT:UI_MUTED);
}
