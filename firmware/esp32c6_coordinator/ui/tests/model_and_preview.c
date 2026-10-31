#include "ui_model.h"
#include "ui_view.h"
#include "ui_geometry.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t good[]={1,3,15,12,0,9,0,2,0};

static void tests(void)
{
    ui_model_t m;
    ui_model_init(&m,100);
    assert(m.splash && m.timeout_index==2 && m.rotation==0);
    assert(!ui_model_fresh(&m,0));
    for (size_t n=0;n<sizeof good;++n) assert(!ui_model_apply(&m,good,n,100));
    assert(ui_model_apply(&m,good,sizeof good,100));
    assert(m.joined==12 && m.available==9 && m.stale==2);
    assert(ui_model_fresh(&m,15099)); assert(!ui_model_fresh(&m,15100));
    ui_model_t old=m;
    uint8_t bad[9]; memcpy(bad,good,9); bad[0]=2;
    assert(!ui_model_apply(&m,bad,9,500)); assert(!memcmp(&m,&old,sizeof m));
    memcpy(bad,good,9); bad[1]=128; assert(!ui_model_apply(&m,bad,9,500));
    memcpy(bad,good,9); bad[2]=27; assert(!ui_model_apply(&m,bad,9,500));
    memcpy(bad,good,9); bad[5]=13; assert(!ui_model_apply(&m,bad,9,500));
    memcpy(bad,good,9); bad[7]=4; assert(!ui_model_apply(&m,bad,9,500));
    assert(ui_model_apply(&m,good,9,UINT32_MAX-100));
    assert(ui_model_fresh(&m,100)); assert(!ui_model_fresh(&m,16000));
    assert(!ui_model_tick(&m,1899)); assert(ui_model_tick(&m,1900) && !m.splash);
    assert(ui_model_touch(&m,170,20,2000)==UI_ACTION_NONE);
    assert(ui_model_touch(&m,20,20,2000)==UI_ACTION_REDRAW && m.menu_open);
    assert(ui_model_touch(&m,70,110,2001)==UI_ACTION_REDRAW && m.page==UI_PAGE_SETTINGS);
    assert(!m.menu_open);
    for (int i=0;i<4;++i) {
        assert(ui_model_touch(&m,80,70,2100+i)==UI_ACTION_SETTINGS);
        assert(m.rotation==((i+1)&3));
        assert(ui_model_width(&m)==((i&1)?170:320));
        assert(ui_model_height(&m)==((i&1)?320:170));
    }
    assert(ui_model_touch(&m,80,120,2200)==UI_ACTION_SETTINGS && m.timeout_index==3);
    assert(!ui_model_tick(&m,62199)); assert(ui_model_tick(&m,62200) && m.sleeping);
    /* A host heartbeat must neither wake the screen nor reset the idle timer. */
    assert(ui_model_apply(&m,good,9,62201) && m.sleeping && m.last_touch_ms==2200);
    assert(ui_model_touch(&m,80,120,62202)==UI_ACTION_WAKE && !m.sleeping && m.timeout_index==3);
    assert(ui_model_touch(&m,80,120,62203)==UI_ACTION_SETTINGS && m.timeout_index==0);
    assert(!ui_model_tick(&m,999999) && !m.sleeping);
    assert(ui_model_touch(&m,80,120,UINT32_MAX-100)==UI_ACTION_SETTINGS && m.timeout_index==1);
    assert(!ui_model_tick(&m,100)); assert(ui_model_tick(&m,15000) && m.sleeping);
    assert(ui_model_touch(&m,20,20,15001)==UI_ACTION_WAKE && !m.menu_open);
    assert(ui_model_touch(&m,20,20,15002)==UI_ACTION_REDRAW && m.menu_open);
    assert(ui_model_touch(&m,70,150,15003)==UI_ACTION_REDRAW && m.page==UI_PAGE_DEBUG);
    assert(ui_model_touch(&m,80,120,15004)==UI_ACTION_NONE);
    assert(ui_model_touch(&m,20,20,15005)==UI_ACTION_REDRAW);
    assert(ui_model_touch(&m,70,60,15006)==UI_ACTION_REDRAW && m.page==UI_PAGE_NETWORK);

    /* Check every pixel, final partial tiles and inverse touch in all rotations. */
    uint16_t input[UI_MAX_WIDTH*UI_TILE_ROWS+2], output[UI_MAX_WIDTH*UI_TILE_ROWS+2];
    ui_diagnostics_t d={.panel_ready=true,.touch_ready=true,.settings_saved=true};
    for (int r=0;r<4;++r) {
        m.rotation=r;
        const int w=ui_model_width(&m), h=ui_model_height(&m);
        unsigned coverage=0;
        for (int y=0;y<h;y+=UI_TILE_ROWS) {
            const int rows=h-y<UI_TILE_ROWS?h-y:UI_TILE_ROWS, count=w*rows;
            input[0]=output[0]=0x1234; input[count+1]=output[count+1]=0x4321;
            for (int page=0;page<3;++page) for (int state=0;state<4;++state) {
                m.page=page; m.menu_open=state==1; m.splash=state==2; m.sleeping=state==3;
                ui_view_render(input+1,y,rows,&m,&d,100);
                assert(input[0]==0x1234 && input[count+1]==0x4321);
                if (m.sleeping) for (int i=1;i<=count;++i) assert(input[i]==0);
            }
            for(int i=0;i<count;++i) input[i+1]=(uint16_t)i;
            int x0,y0,x1,y1;
            assert(ui_geometry_pack(r,y,rows,input+1,output+1,&x0,&y0,&x1,&y1));
            assert(x0>=0 && x1<=170 && y0>=0 && y1<=320 && (x1-x0)*(y1-y0)==count);
            for (int yy=0;yy<rows;++yy) for (int x=0;x<w;++x) {
                int px=x, py=y+yy;
                if(r==1) {px=169-y-yy;py=x;}
                if(r==2) {px=169-x;py=319-y-yy;}
                if(r==3) {px=y+yy;py=319-x;}
                uint16_t tx=px,ty=py;
                assert(ui_geometry_touch(r,&tx,&ty) && tx==x && ty==y+yy);
                const uint16_t v=input[yy*w+x+1], packed=output[(py-y0)*(x1-x0)+px-x0+1];
                assert(packed==(uint16_t)((v<<8)|(v>>8)));
                ++coverage;
            }
            assert(output[0]==0x1234 && output[count+1]==0x4321);
        }
        assert(coverage==170*320);
    }
    uint16_t x=170,y=0; int a,b,c,e;
    assert(!ui_geometry_touch(0,&x,&y));
    assert(!ui_geometry_pack(4,0,16,input,output,&a,&b,&c,&e));
    assert(!ui_geometry_pack(1,160,16,input,output,&a,&b,&c,&e));
    assert(!ui_geometry_pack(0,0,17,input,output,&a,&b,&c,&e));
    fprintf(stderr,"PASS: host validation, expiry/wraparound, navigation, sleep/wake, every pixel/touch inverse and tile bounds in four rotations\n");
}

int main(int argc, char **argv)
{
    if(argc<2) { tests(); return 0; }
    ui_model_t m; ui_model_init(&m,0); m.splash=false;
    ui_diagnostics_t d={.panel_ready=true,.touch_ready=true,.settings_saved=true,
                       .uptime_s=1234,.touches=7,.frames=1234};
    if(strcmp(argv[1],"waiting")) ui_model_apply(&m,good,9,0);
    if(!strcmp(argv[1],"debug")) m.page=UI_PAGE_DEBUG;
    if(!strcmp(argv[1],"settings")) m.page=UI_PAGE_SETTINGS;
    if(!strcmp(argv[1],"menu")) m.menu_open=true;
    if(!strcmp(argv[1],"splash")) m.splash=true;
    if(argc>2) m.rotation=(uint8_t)(atoi(argv[2])&3);
    const int w=ui_model_width(&m), h=ui_model_height(&m);
    uint16_t tile[UI_MAX_WIDTH*UI_TILE_ROWS];
    printf("P6\n%d %d\n255\n",w,h);
    for(int y=0;y<h;y+=UI_TILE_ROWS) {
        const int rows=h-y<UI_TILE_ROWS?h-y:UI_TILE_ROWS;
        ui_view_render(tile,y,rows,&m,&d,1000);
        for(int i=0;i<w*rows;++i) {
            const uint16_t color=tile[i];
            const uint8_t rgb[]={((color>>11)&31)*255/31,((color>>5)&63)*255/63,(color&31)*255/31};
            fwrite(rgb,1,3,stdout);
        }
    }
    return 0;
}
