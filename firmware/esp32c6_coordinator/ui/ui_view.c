#include "ui_view.h"
#include "generated/ui.h"
#include "ui_splash_asset.h"
#include <stdio.h>
#include <string.h>

static ui_control_callback_t on_control;
static int current_width;
static uint8_t current_rotation;
static const uint8_t menu_pages[]={UI_PAGE_NETWORK,UI_PAGE_SETTINGS,UI_PAGE_DEBUG,UI_PAGE_ABOUT};
static const char *page_icons[]={LV_SYMBOL_WIFI,LV_SYMBOL_EYE_OPEN,LV_SYMBOL_SETTINGS,LV_SYMBOL_FILE};

static void navigate(lv_event_t *event)
{
    const uint32_t selected=lv_dropdown_get_selected(lv_event_get_target_obj(event));
    if (selected<4 && on_control) on_control(UI_CONTROL_PAGE,menu_pages[selected]);
}
static void choose_rotation(lv_event_t *event)
{
    if (on_control) on_control(UI_CONTROL_ROTATION,(uint8_t)lv_dropdown_get_selected(lv_event_get_target_obj(event)));
}
static void choose_timeout(lv_event_t *event)
{
    if (on_control) on_control(UI_CONTROL_TIMEOUT,(uint8_t)lv_dropdown_get_selected(lv_event_get_target_obj(event)));
}
static void label(lv_obj_t *obj, const char *value)
{
    if (strcmp(lv_label_get_text(obj),value)) lv_label_set_text(obj,value);
}
static void number(lv_obj_t *obj, uint32_t value, bool valid)
{
    char text[16];
    if (valid) snprintf(text,sizeof text,"%lu",(unsigned long)value);
    else strcpy(text,"--");
    label(obj,text);
}
static void place(lv_obj_t *obj, int x, int y, int w, int h)
{
    lv_obj_set_pos(obj,x,y); lv_obj_set_size(obj,w,h);
}
static void dropdown_style(lv_obj_t *dropdown)
{
    lv_obj_t *list=lv_dropdown_get_list(dropdown);
    lv_obj_set_style_text_font(list,&lv_font_montserrat_14,0);
    lv_obj_set_style_max_height(list,170,0);
    lv_obj_set_style_min_width(list,140,0);
    lv_obj_set_style_bg_color(list,lv_color_hex(0x202624),0);
    lv_obj_set_style_border_color(list,lv_color_hex(0x4C615A),0);
    lv_obj_set_style_text_color(list,lv_color_hex(0xf2f5f4),0);
    lv_obj_set_style_bg_color(list,lv_color_hex(0x335D51),LV_PART_SELECTED|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(list,lv_color_hex(0xf2f5f4),LV_PART_SELECTED|LV_STATE_CHECKED);
}

static void resize(int width, int height)
{
    const bool wide=width>height;
    lv_obj_t *screens[]={objects.network,objects.settings,objects.debug,objects.about,objects.boot};
    lv_obj_t *logos[]={objects.network_logo,objects.settings_logo,objects.debug_logo,objects.about_logo};
    for (unsigned i=0;i<5;++i) {
        lv_obj_set_size(screens[i],width,height);
        lv_obj_remove_flag(screens[i],LV_OBJ_FLAG_SCROLLABLE);
    }
    for (unsigned i=0;i<4;++i) lv_obj_set_pos(logos[i],width-36,10);
    place(objects.network_card,10,50,width-20,wide?46:56);
    place(objects.network_state,18,wide?55:60,width-36,18);
    place(objects.host_state,18,wide?76:83,width-36,16);
    const int top=wide?103:125;
    place(objects.registered_caption,10,top,90,18);
    place(objects.registered_value,10,top+20,90,42);
    place(objects.channel_caption,wide?100:105,top,wide?60:55,18);
    place(objects.channel_value,wide?100:105,top+24,55,28);
    place(objects.available_caption,wide?172:10,wide?top:207,80,18);
    place(objects.available_value,wide?172:10,wide?top+24:230,70,28);
    place(objects.stale_caption,wide?260:98,wide?top:207,60,18);
    place(objects.stale_value,wide?260:98,wide?top+24:230,60,28);
    place(objects.unknown_value,10,wide?150:278,wide?155:150,18);
    place(objects.joining_value,wide?178:10,wide?150:299,140,18);
    lv_obj_set_style_text_font(objects.registered_value,wide?&lv_font_montserrat_20:&lv_font_montserrat_32,0);

    place(objects.rotation_caption,10,57,150,18);
    place(objects.rotation_select,10,81,wide?142:150,38);
    place(objects.timeout_caption,wide?168:10,wide?57:139,150,18);
    place(objects.timeout_select,wide?168:10,wide?81:163,wide?142:150,38);
    place(objects.sleep_hint,10,wide?128:223,wide?195:150,42);
    place(objects.save_state,wide?210:10,wide?143:294,wide?105:150,18);
    label(objects.sleep_hint,wide?"Touch to wake. Radio stays active.":"Touch to wake.\nRadio stays active.");

    lv_obj_t *captions[]={objects.touches_caption,objects.uptime_caption,objects.frames_caption,objects.errors_caption,objects.heap_caption};
    lv_obj_t *values[]={objects.touches_value,objects.uptime_value,objects.frames_value,objects.errors_value,objects.heap_value};
    for (int i=0;i<5;++i) {
        const int x=wide&&i>=3?170:10, y=57+(wide?i%3:i)*(wide?34:46);
        place(captions[i],x,y,145,18); place(values[i],x,wide?y+16:y+20,145,wide?18:24);
        lv_obj_set_style_text_font(values[i],wide?&lv_font_montserrat_12:&lv_font_montserrat_16,0);
    }
    place(objects.about_symbol,wide?20:53,wide?64:54,64,64);
    const int ax=wide?105:10;
    place(objects.maker_caption,ax,wide?50:130,wide?205:150,16);
    place(objects.maker_name,ax,wide?68:151,wide?205:150,22);
    place(objects.device_name,ax,wide?96:188,wide?205:150,37);
    place(objects.device_id,10,wide?145:238,wide?200:150,18);
    place(objects.library_version,wide?215:10,wide?145:270,105,18);
    place(objects.firmware_role,10,294,150,18);
    if (wide) lv_obj_add_flag(objects.firmware_role,LV_OBJ_FLAG_HIDDEN);
    else lv_obj_remove_flag(objects.firmware_role,LV_OBJ_FLAG_HIDDEN);
    lv_obj_center(objects.boot_symbol);
    current_width=width;
}

void ui_view_create(ui_control_callback_t callback)
{
    on_control=callback;
    ui_init();
    lv_obj_t *menus[]={objects.network_navigation,objects.settings_navigation,objects.debug_navigation,objects.about_navigation};
    lv_obj_t *logos[]={objects.network_logo,objects.settings_logo,objects.debug_logo,objects.about_logo};
    const uint8_t pages[]={UI_PAGE_NETWORK,UI_PAGE_SETTINGS,UI_PAGE_DEBUG,UI_PAGE_ABOUT};
    for (unsigned i=0;i<4;++i) {
        lv_dropdown_set_options(menus[i],LV_SYMBOL_WIFI "  Network\n" LV_SYMBOL_SETTINGS "  Settings\n" LV_SYMBOL_EYE_OPEN "  Debug\n" LV_SYMBOL_FILE "  About");
        lv_dropdown_set_text(menus[i],page_icons[pages[i]]);
        lv_obj_add_event_cb(menus[i],navigate,LV_EVENT_VALUE_CHANGED,NULL);
        lv_image_set_src(logos[i],&ui_logo_header);
        dropdown_style(menus[i]);
    }
    lv_image_set_src(objects.about_symbol,&ui_logo_about);
    lv_image_set_src(objects.boot_symbol,&ui_logo_splash);
    lv_obj_add_event_cb(objects.rotation_select,choose_rotation,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_event_cb(objects.timeout_select,choose_timeout,LV_EVENT_VALUE_CHANGED,NULL);
    dropdown_style(objects.rotation_select);
    dropdown_style(objects.timeout_select);
    current_width=0;
    current_rotation=255;
}

void ui_view_update(const ui_model_t *m, const ui_diagnostics_t *d, uint32_t now)
{
    const int width=ui_model_width(m), height=ui_model_height(m);
    if (current_width!=width) resize(width,height);
    lv_obj_t *screens[]={objects.network,objects.debug,objects.settings,objects.about};
    lv_obj_t *screen=m->splash?objects.boot:screens[m->page<UI_PAGE_COUNT?m->page:0];
    if (lv_screen_active()!=screen) lv_screen_load(screen);
    /* 0->180 and 90->270 keep the same logical resolution. The adapter's
     * native pixel transform still changed, so every pixel must be sent. */
    if (current_rotation!=m->rotation) {
        lv_obj_invalidate(screen);
        current_rotation=m->rotation;
    }
    const bool fresh=ui_model_fresh(m,now), online=fresh&&(m->flags&UI_NETWORK_UP);
    label(objects.network_state,fresh?(online?"Network ready":"Network down"):(m->received?"Host data expired":"Waiting for host"));
    lv_obj_set_style_text_color(objects.network_state,lv_color_hex(online?0xA7E6AB:0xF3C575),0);
    label(objects.host_state,fresh?((m->flags&UI_MQTT_UP)?"USB / MQTT online":"MQTT disconnected"):"Connect PC host");
    number(objects.registered_value,m->joined,fresh);
    number(objects.channel_value,m->channel,fresh&&m->channel);
    number(objects.available_value,m->available,fresh);
    number(objects.stale_value,m->stale,fresh);
    char text[32];
    if (fresh) snprintf(text,sizeof text,"Unknown: %u",m->joined-m->available-m->stale);
    else strcpy(text,"Unknown: --");
    label(objects.unknown_value,text);
    label(objects.joining_value,fresh?((m->flags&UI_JOIN_OPEN)?"Joining: open":"Joining: closed"):"Joining: --");
    lv_obj_t *menus[]={objects.network_navigation,objects.debug_navigation,objects.settings_navigation,objects.about_navigation};
    const uint8_t selections[]={0,2,1,3};
    if (m->page<UI_PAGE_COUNT && !lv_dropdown_is_open(menus[m->page]))
        lv_dropdown_set_selected(menus[m->page],selections[m->page]);
    if (!lv_dropdown_is_open(objects.rotation_select)) lv_dropdown_set_selected(objects.rotation_select,m->rotation);
    if (!lv_dropdown_is_open(objects.timeout_select)) lv_dropdown_set_selected(objects.timeout_select,m->timeout_index);
    label(objects.save_state,d->settings_saved?"Saved":"Not saved");
    number(objects.touches_value,d->touches,true); number(objects.uptime_value,d->uptime_s,true);
    number(objects.frames_value,d->frames,true); number(objects.errors_value,d->draw_errors,true);
    number(objects.heap_value,d->heap_kib,true);
    label(objects.device_id,d->device_id[0]?d->device_id:"MAC unavailable");
}
