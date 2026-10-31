#include "ui_model.h"
#include "ui_view.h"
#include "ui_feedback.h"
#include "firmware_version.h"
#include "ui_geometry.h"
#include "ui_navigation.h"
#include "ui_settings.h"
#include "ui_config.h"
#include "generated/screens.h"
#include "board_identity.h"
#include "esp_mac.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t good[]={1,3,15,12,0,9,0,2,0};
static bool mac_fails;
int esp_read_mac(uint8_t *mac, int type)
{
    assert(type==ESP_MAC_BASE);
    if (mac_fails) return -1;
    /* Exercise the SDK's full eight-byte maximum under ASan. The reader
     * formats the six-byte base request and must not overwrite its stack. */
    const uint8_t data[]={0xAC,0xEB,0xE6,0x39,0xFE,0xEC,0xFE,0xEC};
    memcpy(mac,data,sizeof data);
    return ESP_OK;
}
static ui_model_t model;
static ui_action_t last_action;
static unsigned restart_requests;
static uint32_t clock_ms;
static uint16_t framebuffer[UI_WIDTH*UI_HEIGHT];
static uint16_t draw_buffer[UI_MAX_WIDTH*UI_TILE_ROWS];
static lv_display_t *display;
static uint32_t flushed_pixels;
static bool pressed;
static lv_point_t point;
static ui_diagnostics_t health={.panel_ready=true,.touch_ready=true,.settings_saved=true,
    .uptime_s=1234,.touches=7,.frames=1234,.heap_kib=180,.device_id="AC:EB:E6:39:FE:EC"};
static uint32_t tick(void) { return clock_ms; }
extern unsigned test_nvs_commits;
void hardware_settings_tests(void);
static void control(ui_control_t type, uint8_t value)
{
    last_action=ui_model_control(&model,type,value);
    if(last_action==UI_ACTION_SETTINGS) assert(ui_settings_save(&model));
    if(last_action==UI_ACTION_RESTART) ++restart_requests;
}
static void pointer_read(lv_indev_t *input, lv_indev_data_t *data)
{
    (void)input; data->point=point; data->state=pressed?LV_INDEV_STATE_PRESSED:LV_INDEV_STATE_RELEASED;
}
static void flush(lv_display_t *d, const lv_area_t *area, uint8_t *data)
{
    const int w=lv_display_get_horizontal_resolution(d), h=lv_display_get_vertical_resolution(d);
    const int rw=lv_area_get_width(area), rh=lv_area_get_height(area);
    assert(area->x1>=0 && area->y1>=0 && area->x2<w && area->y2<h);
    assert(rw*rh<=UI_MAX_WIDTH*UI_TILE_ROWS);
    flushed_pixels+=(uint32_t)(rw*rh);
    for (int y=0;y<rh;++y)
        memcpy(framebuffer+(area->y1+y)*w+area->x1,(uint16_t *)data+y*rw,rw*sizeof(uint16_t));
    lv_display_flush_ready(d);
}
static void pump(unsigned ms)
{
    for (unsigned t=0;t<ms;t+=20) { clock_ms+=20; lv_timer_handler(); }
}
static void refresh(void)
{
    lv_display_set_resolution(display,ui_model_width(&model),ui_model_height(&model));
    assert(lv_display_get_rotation(display)==LV_DISPLAY_ROTATION_0);
    ui_view_update(&model,&health,clock_ms);
    lv_obj_invalidate(lv_screen_active());
    pump(100);
}
static void click(int x,int y)
{
    point.x=x;point.y=y;pressed=true;pump(100);pressed=false;pump(100);
}
static void popup_tests(void)
{
    const unsigned nav_count=ui_navigation_count();
    lv_obj_t *items[6];uint8_t pages[6];
    for(unsigned i=0;i<nav_count;++i) {items[i]=ui_navigation_entry(i)->menu;pages[i]=ui_navigation_entry(i)->page;}
    items[nav_count]=objects.rotation_select;items[nav_count+1]=objects.timeout_select;
    pages[nav_count]=pages[nav_count+1]=UI_PAGE_SETTINGS;
    assert(nav_count==(UI_SHOW_DEBUG_PAGE?4U:3U));
    assert(ui_navigation_entry(0)->page==UI_PAGE_NETWORK && ui_navigation_entry(1)->page==UI_PAGE_SETTINGS && ui_navigation_entry(2)->page==UI_PAGE_ABOUT);
    if(UI_SHOW_DEBUG_PAGE) assert(ui_navigation_entry(3)->page==UI_PAGE_DEBUG);
    else assert(!strstr(ui_navigation_options(),"Debug"));
    for(int r=0;r<4;++r) for(unsigned i=0;i<nav_count+2;++i) for(unsigned option=0;option<(i<nav_count?nav_count:4);++option) {
        model.rotation=r;model.page=pages[i];refresh();
        lv_dropdown_open(items[i]);pump(80);
        lv_obj_t *list=lv_dropdown_get_list(items[i]);
        lv_area_t a;lv_obj_get_coords(list,&a);
        assert(a.x1>=10 && a.y1>=10 && a.x2<ui_model_width(&model)-10 && a.y2<ui_model_height(&model)-10);
        lv_obj_t *text=lv_obj_get_child(list,0);
        const lv_font_t *font=lv_obj_get_style_text_font(text,0);
        const int line=lv_font_get_line_height(font)+lv_obj_get_style_text_line_space(text,0);
        /* Exercise options below a short landscape popup's scroll viewport. */
        lv_obj_scroll_to_y(list,(int)option*line-lv_obj_get_content_height(list)/2,LV_ANIM_OFF);
        pump(80);
        lv_area_t label_area;lv_obj_get_coords(text,&label_area);
        const int y=label_area.y1+(int)option*line+lv_font_get_line_height(font)/2;
        assert(y>a.y1 && y<a.y2);
        click(a.x1+20,y);
        assert(!lv_dropdown_is_open(items[i]));
        assert(lv_dropdown_get_selected(items[i])==option);
        if(i<nav_count) assert(model.page==ui_navigation_entry(option)->page);
        if(i==nav_count) assert(model.rotation==option);
        if(i==nav_count+1) assert(model.timeout_index==option);
    }
    for(int r=0;r<4;++r) {
        model.rotation=r;model.page=UI_PAGE_SETTINGS;refresh();
        assert(lv_dropdown_get_symbol(objects.settings_navigation)==NULL);
        assert(!strcmp(lv_dropdown_get_text(objects.settings_navigation),(r&1)?LV_SYMBOL_SETTINGS "  Settings":LV_SYMBOL_SETTINGS));
        lv_area_t a;lv_obj_get_coords(objects.settings_logo,&a);
        assert(a.x2==ui_model_width(&model)-11 && a.y1==10);
        lv_obj_get_coords(objects.sleep_info,&a);click((a.x1+a.x2)/2,(a.y1+a.y2)/2);
        lv_obj_t *toast=lv_obj_get_child(lv_layer_top(),0);
        assert(!lv_obj_has_flag(toast,LV_OBJ_FLAG_HIDDEN));
        assert(!strcmp(lv_label_get_text(toast),"Touch to wake.\nRadio stays active."));
        ui_feedback_save_result(false);pump(100);
        assert(strstr(lv_label_get_text(toast),"Could not save"));
        ui_feedback_save_result(true);pump(100);
        assert(lv_obj_has_flag(toast,LV_OBJ_FLAG_HIDDEN));
        ui_feedback_save_result(true);pump(100);
        assert(lv_obj_has_flag(toast,LV_OBJ_FLAG_HIDDEN));
        ui_feedback_show("Touch to wake.");
        ui_feedback_save_result(true);pump(100);
        assert(!strcmp(lv_label_get_text(toast),"Touch to wake."));
        model.rotation^=1;refresh();lv_obj_get_coords(toast,&a);
        assert(a.x1>=10 && a.x2<ui_model_width(&model)-10 && a.y2<ui_model_height(&model)-10);
        pump(3100);assert(lv_obj_has_flag(toast,LV_OBJ_FLAG_HIDDEN));
    }
    assert(!strcmp(lv_label_get_text(objects.device_name),"ESPC6"));
    assert(!strcmp(lv_label_get_text(objects.firmware_version),"Firmware " UI_FIRMWARE_VERSION));
}
static void brightness_and_spacing_tests(void)
{
    for(int r=0;r<4;++r) {
        model.rotation=r;model.page=UI_PAGE_SETTINGS;
        model.brightness=model.brightness_committed=100;refresh();
        lv_area_t a;lv_obj_get_coords(objects.brightness_slider,&a);
        const unsigned before=test_nvs_commits;
        point.x=a.x2;point.y=(a.y1+a.y2)/2;pressed=true;pump(100);
        point.x=a.x1+(a.x2-a.x1)/3;pump(100);
        assert(model.brightness>=10 && model.brightness<100 && model.brightness_committed==100);
        assert(test_nvs_commits==before);
        const int value=lv_slider_get_value(objects.brightness_slider);
        ui_view_update(&model,&health,clock_ms);
        assert(lv_slider_get_value(objects.brightness_slider)==value);
        pressed=false;pump(100);
        assert(test_nvs_commits==before+1 && model.brightness_committed==model.brightness);
        refresh();char percent[8];snprintf(percent,sizeof percent,"%u%%",model.brightness);
        assert(!strcmp(lv_label_get_text(objects.brightness_value),percent));
        assert(lv_obj_get_width(objects.sleep_info)==18 && lv_obj_get_height(objects.sleep_info)==18);
        if(!(r&1)) {
            model.page=UI_PAGE_ABOUT;refresh();
            lv_obj_t *rows[]={objects.maker_name,objects.device_name,objects.firmware_version,objects.library_version,objects.firmware_role};
            for(unsigned i=0;i<5;++i) {
                assert(lv_obj_get_style_text_align(rows[i],0)==LV_TEXT_ALIGN_CENTER);
                if(i) assert(lv_obj_get_y(rows[i])-lv_obj_get_y(rows[i-1])==32);
            }
        }
    }
}
static void scroll_and_actions_tests(void)
{
    for(int r=0;r<4;++r) for(unsigned page=0;page<ui_navigation_count();++page) {
        const ui_navigation_entry_t *entry=ui_navigation_entry(page);
        model.rotation=r;model.page=entry->page;refresh();
        lv_obj_t *body=entry->content;
        assert(lv_obj_has_flag(body,LV_OBJ_FLAG_SCROLLABLE));
        assert(lv_obj_get_scroll_dir(body)==LV_DIR_VER && lv_obj_get_scrollbar_mode(body)==LV_SCROLLBAR_MODE_ACTIVE);
        assert(lv_color_eq(lv_obj_get_style_bg_color(body,0),lv_color_black()));
        assert(lv_color_eq(lv_obj_get_style_bg_color(entry->screen,0),lv_color_black()));
        assert(framebuffer[0]==0);
        lv_area_t hor,ver,header_before,header_after;
        lv_obj_get_scrollbar_area(body,&hor,&ver);assert(lv_area_get_width(&ver)<=0);
        lv_obj_get_coords(entry->menu,&header_before);
        /* Exercise overflow scrolling even for a page whose current text fits. */
        lv_obj_t *spacer=lv_obj_create(body);lv_obj_set_size(spacer,1,1);
        lv_obj_set_pos(spacer,0,lv_obj_get_height(body)+100);
        lv_obj_remove_flag(spacer,LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_opa(spacer,LV_OPA_TRANSP,0);
        lv_obj_scroll_to_y(body,0,LV_ANIM_OFF);pump(100);
        point.x=ui_model_width(&model)-6;point.y=ui_model_height(&model)-25;pressed=true;pump(100);
        for(int i=0;i<8;++i) {point.y-=8;pump(40);}
        assert(lv_obj_get_scroll_y(body)>0);
        lv_obj_get_scrollbar_area(body,&hor,&ver);assert(lv_area_get_width(&ver)>0);
        lv_obj_get_coords(entry->menu,&header_after);
        assert(!memcmp(&header_before,&header_after,sizeof header_before));
        pressed=false;pump(1200);
        lv_obj_get_scrollbar_area(body,&hor,&ver);assert(lv_area_get_width(&ver)<=0);
        lv_obj_delete(spacer);lv_obj_scroll_to_y(body,0,LV_ANIM_OFF);pump(100);
    }
    for(int r=0;r<4;++r) {
        model.rotation=r;model.page=UI_PAGE_SETTINGS;refresh();
        lv_obj_t *buttons[]={objects.reset_button,objects.restart_button};
        for(unsigned i=0;i<2;++i) {
            lv_obj_scroll_to_view(buttons[i],LV_ANIM_OFF);pump(100);
            lv_area_t button;lv_obj_get_coords(buttons[i],&button);
            const unsigned saves_before=test_nvs_commits,restarts_before=restart_requests;
            point.x=(button.x1+button.x2)/2;point.y=(button.y1+button.y2)/2;pressed=true;pump(80);
            for(unsigned step=0;step<3;++step){point.y+=6;pump(40);}
            pressed=false;pump(500);
            assert(test_nvs_commits==saves_before && restart_requests==restarts_before);
        }
        lv_obj_scroll_to_view(objects.restart_button,LV_ANIM_OFF);pump(100);
        lv_area_t a;lv_obj_get_coords(objects.restart_button,&a);
        assert(a.y1>=48 && a.y2<ui_model_height(&model));
        const unsigned commits=test_nvs_commits,restarts=restart_requests;
        const ui_model_t before=model;
        click((a.x1+a.x2)/2,(a.y1+a.y2)/2);
        assert(restart_requests==restarts+1 && test_nvs_commits==commits);
        assert(!memcmp(&before,&model,sizeof model));
        model.timeout_index=0;model.brightness=model.brightness_committed=25;
        assert(ui_model_apply(&model,good,9,clock_ms));
        const ui_model_t host=model;
        lv_obj_scroll_to_view(objects.reset_button,LV_ANIM_OFF);pump(100);
        lv_obj_get_coords(objects.reset_button,&a);click((a.x1+a.x2)/2,(a.y1+a.y2)/2);
        assert(last_action==UI_ACTION_SETTINGS && test_nvs_commits==commits+1);
        assert(model.rotation==0 && model.timeout_index==2 && model.brightness==100 && model.brightness_committed==100);
        assert(model.page==UI_PAGE_SETTINGS && model.flags==host.flags && model.channel==host.channel);
        assert(model.joined==host.joined && model.available==host.available && model.stale==host.stale && model.last_update_ms==host.last_update_ms);
        refresh();assert(lv_obj_get_scroll_y(objects.settings_content)==0);
    }
}
static void model_tests(void)
{
    struct { char before; char value[18]; char after; } identity={.before='A',.after='Z'};
    assert(board_identity_read(identity.value));
    assert(!strcmp(identity.value,"AC:EB:E6:39:FE:EC") && identity.before=='A' && identity.after=='Z');
    mac_fails=true;assert(!board_identity_read(identity.value) && identity.value[0]==0);mac_fails=false;
    ui_model_t m;
    ui_model_init(&m,100);
    assert(m.splash && m.timeout_index==2 && m.rotation==0 && m.brightness==100);
    assert(ui_model_control(&m,UI_CONTROL_BRIGHTNESS_PREVIEW,9)==UI_ACTION_NONE);
    assert(ui_model_control(&m,UI_CONTROL_BRIGHTNESS,101)==UI_ACTION_NONE);
    assert(ui_model_control(&m,UI_CONTROL_BRIGHTNESS_PREVIEW,50)==UI_ACTION_REDRAW);
    assert(m.brightness_committed==100);
    assert(ui_model_control(&m,UI_CONTROL_BRIGHTNESS,50)==UI_ACTION_SETTINGS);
    assert(ui_model_control(&m,UI_CONTROL_BRIGHTNESS,50)==UI_ACTION_NONE);
    for(size_t n=0;n<sizeof good;++n) assert(!ui_model_apply(&m,good,n,100));
    assert(ui_model_apply(&m,good,9,100));
    assert(ui_model_fresh(&m,15099) && !ui_model_fresh(&m,15100));
    ui_model_t before=m; uint8_t bad[9]; memcpy(bad,good,9);bad[5]=13;
    assert(!ui_model_apply(&m,bad,9,500) && !memcmp(&m,&before,sizeof m));
    bad[5]=9;bad[7]=4;assert(!ui_model_apply(&m,bad,9,500));
    assert(ui_model_apply(&m,good,9,UINT32_MAX-100));
    assert(ui_model_fresh(&m,100) && !ui_model_fresh(&m,16000));
    assert(!ui_model_tick(&m,1899) && ui_model_tick(&m,1900));
    assert(ui_model_control(&m,UI_CONTROL_PAGE,UI_PAGE_ABOUT)==UI_ACTION_REDRAW);
    assert(ui_model_control(&m,UI_CONTROL_PAGE,UI_PAGE_COUNT)==UI_ACTION_NONE);
    assert(ui_model_control(&m,UI_CONTROL_ROTATION,4)==UI_ACTION_NONE);
    assert(ui_model_control(&m,UI_CONTROL_TIMEOUT,3)==UI_ACTION_SETTINGS);
    assert(ui_model_control(&m,UI_CONTROL_TIMEOUT,3)==UI_ACTION_NONE);
    assert(!ui_model_tick(&m,60099) && ui_model_tick(&m,60100) && m.sleeping);
    assert(ui_model_apply(&m,good,9,60101) && m.sleeping && m.last_touch_ms==100);
    assert(ui_model_touch(&m,20,20,60102)==UI_ACTION_WAKE && m.page==UI_PAGE_ABOUT);
    assert(ui_model_control(&m,UI_CONTROL_TIMEOUT,0)==UI_ACTION_SETTINGS);
    assert(!ui_model_tick(&m,999999) && !m.sleeping);
    assert(ui_model_touch(&m,170,0,999999)==UI_ACTION_NONE);
    m.last_touch_ms=UINT32_MAX-100;m.timeout_index=1;
    assert(!ui_model_tick(&m,100) && ui_model_tick(&m,15000));
}
static void geometry_tests(void)
{
    uint16_t in[UI_MAX_WIDTH*UI_TILE_ROWS], out[UI_MAX_WIDTH*UI_TILE_ROWS+2];
    for(unsigned i=0;i<UI_MAX_WIDTH*UI_TILE_ROWS;++i) in[i]=(uint16_t)i;
    for(int r=0;r<4;++r) {
        const int w=(r&1)?320:170,h=(r&1)?170:320;
        for(int row=0;row<h;row+=16) {
            const int rows=h-row<16?h-row:16;
            int x0,y0,x1,y1;
            out[0]=0x1234;out[w*rows+1]=0x4321;
            assert(ui_geometry_pack(r,row,rows,in,out+1,&x0,&y0,&x1,&y1));
            for(int yy=0;yy<rows;++yy) for(int xx=0;xx<w;++xx) {
                int px=xx,py=row+yy;
                if(r==1){px=169-row-yy;py=xx;}
                if(r==2){px=169-xx;py=319-row-yy;}
                if(r==3){px=row+yy;py=319-xx;}
                uint16_t tx=px,ty=py,v=in[yy*w+xx];
                assert(ui_geometry_touch(r,&tx,&ty) && tx==xx && ty==row+yy);
                assert(out[1+(py-y0)*(x1-x0)+px-x0]==(uint16_t)((v<<8)|(v>>8)));
            }
            assert(out[0]==0x1234 && out[w*rows+1]==0x4321);
        }
        /* Dirty regions are not restricted to full-width strips. */
        for(int x=0;x<w;x+=37) for(int y=0;y<h;y+=41) {
            const int rw=w-x<29?w-x:29,rh=h-y<31?h-y:31;
            int x0,y0,x1,y1;
            assert(ui_geometry_pack_rect(r,x,y,rw,rh,in,out,&x0,&y0,&x1,&y1));
            for(int yy=y0;yy<y1;++yy) for(int xx=x0;xx<x1;++xx) {
                uint16_t tx=xx,ty=yy;assert(ui_geometry_touch(r,&tx,&ty));
                assert(tx>=x && tx<x+rw && ty>=y && ty<y+rh);
                const uint16_t v=in[(ty-y)*rw+tx-x];
                assert(out[(yy-y0)*(x1-x0)+xx-x0]==(uint16_t)((v<<8)|(v>>8)));
            }
        }
    }
    int a,b,c,d;
    assert(!ui_geometry_pack_rect(0,0,0,170,320,in,out,&a,&b,&c,&d));
    assert(!ui_geometry_pack_rect(4,0,0,1,1,in,out,&a,&b,&c,&d));
    assert(!ui_geometry_pack_rect(0,169,319,2,2,in,out,&a,&b,&c,&d));
}
static void gui_init(void)
{
    assert(LVGL_VERSION_MAJOR==9 && LVGL_VERSION_MINOR==5 && LVGL_VERSION_PATCH==0);
    lv_init();lv_tick_set_cb(tick);
    display=lv_display_create(170,320);
    assert(display);lv_display_set_color_format(display,LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display,draw_buffer,NULL,sizeof draw_buffer,LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display,flush);
    lv_indev_t *input=lv_indev_create();assert(input);
    lv_indev_set_type(input,LV_INDEV_TYPE_POINTER);lv_indev_set_read_cb(input,pointer_read);
    ui_model_init(&model,0);model.splash=false;
    assert(ui_model_apply(&model,good,9,0));
    ui_view_create(control);refresh();
}
static void gui_tests(void)
{
    assert(!strcmp(lv_label_get_text(objects.registered_value),"12"));
    assert(lv_dropdown_get_option_count(objects.network_navigation)==ui_navigation_count());
    assert(!strcmp(lv_dropdown_get_text(objects.network_navigation),LV_SYMBOL_WIFI));
    assert(!strcmp(lv_label_get_text(objects.maker_name),"RayKestrel Systems"));
    assert(!strcmp(lv_label_get_text(objects.library_version),"LVGL 9.5.0"));
    /* Exercise actual LVGL pointer hit testing to open the active-icon menu. */
    point.x=25;point.y=22;pressed=true;pump(100);pressed=false;pump(100);
    assert(lv_dropdown_is_open(objects.network_navigation));
    lv_dropdown_set_selected(objects.network_navigation,2);
    /* A real list release closes the popup before delivering selection. */
    lv_dropdown_close(objects.network_navigation);
    lv_obj_send_event(objects.network_navigation,LV_EVENT_VALUE_CHANGED,NULL);
    assert(model.page==UI_PAGE_ABOUT);refresh();assert(lv_screen_active()==objects.about);
    lv_dropdown_set_selected(objects.about_navigation,1);
    lv_obj_send_event(objects.about_navigation,LV_EVENT_VALUE_CHANGED,NULL);
    assert(model.page==UI_PAGE_SETTINGS);refresh();
    lv_dropdown_set_selected(objects.rotation_select,1);
    lv_obj_send_event(objects.rotation_select,LV_EVENT_VALUE_CHANGED,NULL);
    assert(model.rotation==1 && last_action==UI_ACTION_SETTINGS);refresh();
    assert(lv_display_get_horizontal_resolution(display)==320);
    lv_dropdown_open(objects.rotation_select);
    lv_dropdown_set_selected(objects.rotation_select,2);
    ui_view_update(&model,&health,clock_ms);
    assert(lv_dropdown_get_selected(objects.rotation_select)==2 && model.rotation==1);
    lv_dropdown_close(objects.rotation_select);
    lv_dropdown_set_selected(objects.timeout_select,0);
    lv_obj_send_event(objects.timeout_select,LV_EVENT_VALUE_CHANGED,NULL);
    assert(model.timeout_index==0 && last_action==UI_ACTION_SETTINGS);
    lv_obj_send_event(objects.timeout_select,LV_EVENT_VALUE_CHANGED,NULL);
    assert(last_action==UI_ACTION_NONE);
    clock_ms=16000;ui_view_update(&model,&health,clock_ms);
    assert(!strcmp(lv_label_get_text(objects.registered_value),"--"));
    for(int r=0;r<4;++r) for(int p=0;p<UI_PAGE_COUNT;++p) {
        model.rotation=r;model.page=p;refresh();
        assert(lv_obj_get_width(lv_screen_active())==ui_model_width(&model));
        assert(lv_obj_get_height(lv_screen_active())==ui_model_height(&model));
    }
    model.rotation=0;model.page=UI_PAGE_NETWORK;refresh();
    flushed_pixels=0;model.rotation=2;
    ui_view_update(&model,&health,clock_ms);pump(100);
    assert(flushed_pixels>=UI_WIDTH*UI_HEIGHT);
    for(int r=0;r<4;++r) {
        model.rotation=r;model.page=UI_PAGE_NETWORK;refresh();
        uint16_t px=25,py=22;
        if(r==1){px=147;py=25;}
        if(r==2){px=144;py=297;}
        if(r==3){px=22;py=294;}
        assert(ui_geometry_touch(r,&px,&py) && px==25 && py==22);
        point.x=px;point.y=py;pressed=true;pump(100);pressed=false;pump(100);
        assert(lv_dropdown_is_open(objects.network_navigation));
        lv_dropdown_close(objects.network_navigation);
    }
    popup_tests();
    brightness_and_spacing_tests();
    scroll_and_actions_tests();
    fprintf(stderr,"PASS: silent saves, page swipe/active-only scrollbar/fixed header/black, reset/restart actions, Debug=%d and existing brightness/PWM/NVS/popup/model regressions\n",UI_SHOW_DEBUG_PAGE);
}
int main(int argc,char **argv)
{
    if(argc<2){hardware_settings_tests();model_tests();geometry_tests();gui_init();gui_tests();lv_deinit();return 0;}
    gui_init();
    if(!strcmp(argv[1],"waiting")) model.received=false;
    if(!strcmp(argv[1],"actions") || !strcmp(argv[1],"scrolling") || !strcmp(argv[1],"settings") || !strcmp(argv[1],"rotation") || !strcmp(argv[1],"timeout") || !strcmp(argv[1],"help") || !strcmp(argv[1],"saved")) model.page=UI_PAGE_SETTINGS;
    if(!strcmp(argv[1],"debug")) model.page=UI_PAGE_DEBUG;
    if(!strcmp(argv[1],"about")) model.page=UI_PAGE_ABOUT;
    if(!strcmp(argv[1],"splash")) model.splash=true;
    if(argc>2)model.rotation=(uint8_t)(atoi(argv[2])&3);
    refresh();
    if(!strcmp(argv[1],"menu")){lv_dropdown_open(objects.network_navigation);pump(100);}
    if(!strcmp(argv[1],"rotation")){lv_dropdown_open(objects.rotation_select);pump(100);}
    if(!strcmp(argv[1],"timeout")){lv_dropdown_open(objects.timeout_select);pump(100);}
    if(!strcmp(argv[1],"help")){lv_obj_send_event(objects.sleep_info,LV_EVENT_CLICKED,NULL);pump(100);}
    if(!strcmp(argv[1],"saved")){ui_feedback_save_result(true);pump(100);}
    if(!strcmp(argv[1],"actions")){lv_obj_scroll_to_view(objects.restart_button,LV_ANIM_OFF);pump(100);}
    if(!strcmp(argv[1],"scrolling")){point.x=ui_model_width(&model)-6;point.y=ui_model_height(&model)-25;pressed=true;pump(100);for(int i=0;i<8;++i){point.y-=8;pump(40);}}
    printf("P6\n%d %d\n255\n",ui_model_width(&model),ui_model_height(&model));
    for(unsigned i=0;i<UI_WIDTH*UI_HEIGHT;++i){
        const uint16_t c=framebuffer[i];
        const uint8_t rgb[]={((c>>11)&31)*255/31,((c>>5)&63)*255/63,(c&31)*255/31};
        fwrite(rgb,1,3,stdout);
    }
    lv_deinit();return 0;
}
