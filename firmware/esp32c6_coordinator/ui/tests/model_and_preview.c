#include "ui_model.h"
#include "ui_view.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void tests(void)
{
    ui_model_t m = {0};
    const uint8_t good[] = {1,3,15,12,0,9,0,2,0};
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
    assert(ui_model_apply(&m,good,9,UINT32_MAX-100));
    assert(ui_model_fresh(&m,100)); assert(!ui_model_fresh(&m,16000));
    assert(!ui_model_touch(&m,169,281));
    assert(ui_model_touch(&m,100,300) && m.page==1);
    assert(!ui_model_touch(&m,170,300));
    assert(ui_model_touch(&m,20,300) && m.page==0);
    ui_diagnostics_t d={.panel_ready=true,.touch_ready=true};
    uint16_t pixels[UI_WIDTH*UI_TILE_ROWS+2];
    for (int page=0;page<2;++page) for(int y=0;y<UI_HEIGHT;y+=UI_TILE_ROWS) {
        pixels[0]=0x1234; pixels[UI_WIDTH*UI_TILE_ROWS+1]=0x4321; m.page=page;
        ui_view_render(pixels+1,y,UI_TILE_ROWS,&m,&d,100);
        assert(pixels[0]==0x1234 && pixels[UI_WIDTH*UI_TILE_ROWS+1]==0x4321);
    }
    fprintf(stderr,"PASS: model validation, expiry/wraparound, touch bounds and tile bounds\n");
}
int main(int argc, char **argv)
{
    tests();
    if(argc<2) return 0;
    ui_model_t m={0}; const uint8_t good[]={1,3,15,12,0,9,0,2,0};
    ui_diagnostics_t d={.panel_ready=true,.touch_ready=true,.uptime_s=1234,.touches=7,.frames=1234};
    if(strcmp(argv[1],"waiting")) ui_model_apply(&m,good,9,0);
    if(!strcmp(argv[1],"system")) m.page=1;
    uint16_t tile[UI_WIDTH*UI_TILE_ROWS];
    printf("P6\n170 320\n255\n");
    for(int y=0;y<UI_HEIGHT;y+=UI_TILE_ROWS) {
        ui_view_render(tile,y,UI_TILE_ROWS,&m,&d,1000);
        for(unsigned i=0;i<sizeof tile/sizeof *tile;++i) {
            const uint16_t c=tile[i];
            const uint8_t rgb[]={((c>>11)&31)*255/31,((c>>5)&63)*255/63,(c&31)*255/31};
            fwrite(rgb,1,3,stdout);
        }
    }
    return 0;
}
