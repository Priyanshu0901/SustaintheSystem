#include "ui_view.h"
#include "ui_theme.h"
#include "ui_splash_asset.h"
#include <stdio.h>
#include <string.h>

/* Original compact 5x7 uppercase glyphs, one row per byte. */
static const char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-:.?/ +";
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
 {14,17,1,2,4,0,4},{1,2,2,4,8,8,16},{0,0,0,0,0,0,0}, {0,4,4,31,4,4,0}
};
typedef struct { uint16_t *p; int y, rows, width; } canvas_t;
static void rect(canvas_t *c, int x, int y, int w, int h, uint16_t color)
{
    for (int yy = y; yy < y+h; ++yy) {
        if (yy < c->y || yy >= c->y+c->rows) continue;
        for (int xx = x; xx < x+w; ++xx)
            if (xx >= 0 && xx < c->width) c->p[(yy-c->y)*c->width+xx] = color;
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
static void icon(canvas_t *c, int x, int y, int kind, uint16_t color)
{
    if (kind == 0) { /* connected nodes */
        rect(c,x+2,y+3,18,2,color); rect(c,x+10,y+3,2,17,color);
        rect(c,x,y,6,7,color); rect(c,x+16,y,6,7,color); rect(c,x+8,y+16,6,7,color);
    } else if (kind == 1) { /* screen controls */
        for (int i=0;i<3;++i) {
            rect(c,x,y+3+i*7,22,2,color);
            rect(c,x+(i==1?5:14),y+i*7,4,7,color);
        }
    } else { /* diagnostic bars */
        rect(c,x,y+12,5,10,color); rect(c,x+8,y+5,5,17,color); rect(c,x+16,y,5,22,color);
    }
}

static void network(canvas_t *c, const ui_model_t *m, bool fresh, bool wide)
{
    const bool online = fresh && (m->flags & UI_NETWORK_UP);
    rect(c,12,48,c->width-24,44,UI_CARD);
    rect(c,20,59,5,5,online?UI_OK:UI_WARN);
    text(c,32,58,fresh?(online?"NETWORK READY":"NETWORK DOWN"):
         (m->received?"HOST DATA EXPIRED":"WAITING FOR HOST"),1,online?UI_OK:UI_WARN);
    text(c,20,77,fresh?((m->flags&UI_MQTT_UP)?"USB / MQTT ONLINE":"MQTT DISCONNECTED"):
         "CONNECT PC HOST",1,UI_MUTED);
    const int top=wide?105:110;
    text(c,12,top,"REGISTERED",1,UI_MUTED);
    value(c,12,top+17,m->joined,fresh,wide?3:5,UI_TEXT);
    text(c,wide?93:116,top,"CHANNEL",1,UI_MUTED);
    value(c,wide?93:116,top+17,m->channel,fresh&&m->channel,2,UI_ACCENT);
    const int ax=wide?171:20, sx=wide?243:98, ay=wide?top:196;
    if (!wide) rect(c,12,185,c->width-24,72,UI_CARD);
    text(c,ax,ay,"AVAILABLE",1,UI_MUTED); text(c,sx,ay,"STALE",1,UI_MUTED);
    value(c,ax,ay+18,m->available,fresh,3,UI_OK);
    value(c,sx,ay+18,m->stale,fresh,3,UI_WARN);
    char buf[32];
    if (fresh) snprintf(buf,sizeof buf,"UNKNOWN %u  JOIN %s",m->joined-m->available-m->stale,
                        (m->flags&UI_JOIN_OPEN)?"OPEN":"SHUT");
    else strcpy(buf,"HOST VALUES UNAVAILABLE");
    text(c,12,wide?153:274,buf,1,UI_MUTED);
    if (!wide) text(c,12,300,"ZIGBEE / USB",1,UI_MUTED);
}

static void debug(canvas_t *c, const ui_diagnostics_t *d, bool wide)
{
    const char *labels[]={"LCD","TOUCH","UPTIME S","TOUCHES","FRAMES","DRAW ERR"};
    for (int i=0;i<6;++i) {
        const int x=(wide&&i>=3)?170:12, y=54+(wide?i%3:i)*(wide?32:36);
        text(c,x,y,labels[i],1,UI_MUTED);
        if (i<2) text(c,x+72,y,i==0?(d->panel_ready?"READY":"ERROR"):
                       (d->touch_ready?"READY":"MISSING"),1,UI_TEXT);
        else {
            const uint32_t values[]={d->uptime_s,d->touches,d->frames,d->draw_errors};
            value(c,x+72,y,values[i-2],true,1,i==5&&d->draw_errors?UI_WARN:UI_TEXT);
        }
    }
    text(c,12,wide?153:294,"WAVESHARE C6 / 8MB",1,UI_MUTED);
}

static void settings(canvas_t *c, const ui_model_t *m, const ui_diagnostics_t *d, bool wide)
{
    char rotation[24];
    snprintf(rotation,sizeof rotation,"%u DEG",m->rotation*90U);
    static const char *timeouts[]={"OFF","15 SEC","30 SEC","60 SEC"};
    const char *labels[]={"ROTATION","POWER SAVE"};
    const char *values[]={rotation,timeouts[m->timeout_index&3]};
    for (int i=0;i<2;++i) {
        const int y=48+i*52;
        rect(c,12,y,c->width-24,44,UI_CARD);
        text(c,20,y+7,labels[i],1,UI_MUTED);
        text(c,20,y+24,values[i],1,UI_TEXT);
        text(c,c->width-32,y+18,"+",1,UI_ACCENT);
    }
    if (wide) text(c,12,153,d->settings_saved?"TAP TO CHANGE / SAVED":"SETTINGS NOT SAVED",1,
                   d->settings_saved?UI_MUTED:UI_WARN);
    else {
        text(c,12,165,"TAP A ROW TO CHANGE",1,UI_MUTED);
        text(c,12,200,"BLACK SCREEN WHEN IDLE",1,UI_MUTED);
        text(c,12,220,"TOUCH TO WAKE",1,UI_TEXT);
        text(c,12,240,"RADIO STAYS ACTIVE",1,UI_MUTED);
        text(c,12,294,d->settings_saved?"SETTINGS SAVED":"SETTINGS NOT SAVED",1,
             d->settings_saved?UI_MUTED:UI_WARN);
    }
}

void ui_view_render(uint16_t *p, int y, int rows, const ui_model_t *m,
                    const ui_diagnostics_t *d, uint32_t now)
{
    const int w=ui_model_width(m), h=ui_model_height(m);
    canvas_t c = {p,y,rows,w};
    rect(&c,0,y,w,rows,m->sleeping||m->splash?0:UI_BG);
    if (m->sleeping) return;
    if (m->splash) {
        const int left=(w-UI_SPLASH_WIDTH)/2, top=(h-UI_SPLASH_HEIGHT)/2;
        for (int yy=0;yy<UI_SPLASH_HEIGHT;++yy) for (int x=0;x<UI_SPLASH_WIDTH;++x)
            rect(&c,left+x,top+yy,1,1,ui_splash_pixels[yy*UI_SPLASH_WIDTH+x]);
        return;
    }
    if (m->menu_open) {
        text(&c,52,17,"PAGES",2,UI_TEXT);
        static const char *names[]={"NETWORK","SETTINGS","DEBUG"};
        static const uint8_t pages[]={UI_PAGE_NETWORK,UI_PAGE_SETTINGS,UI_PAGE_DEBUG};
        for (int i=0;i<3;++i) {
            const int yy=48+i*40;
            rect(&c,12,yy,w-24,36,UI_CARD);
            const uint16_t color=m->page==pages[i]?UI_ACCENT:UI_TEXT;
            icon(&c,22,yy+7,i,color); text(&c,58,yy+14,names[i],1,color);
        }
    } else {
        static const char *titles[]={"NETWORK","DEBUG","SETTINGS"};
        text(&c,52,17,titles[m->page<=UI_PAGE_SETTINGS?m->page:0],2,UI_TEXT);
        const bool wide=w>h;
        if (m->page==UI_PAGE_SETTINGS) settings(&c,m,d,wide);
        else if (m->page==UI_PAGE_DEBUG) debug(&c,d,wide);
        else network(&c,m,ui_model_fresh(m,now),wide);
    }
    for (int i=0;i<3;++i) rect(&c,16,13+i*7,22,2,UI_TEXT);
    rect(&c,12,42,w-24,1,UI_LINE);
}
