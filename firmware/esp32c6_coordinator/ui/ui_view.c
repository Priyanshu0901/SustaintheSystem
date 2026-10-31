#include "ui_view.h"
#include "ui_layout.h"
#include "ui_controls.h"
#include "ui_navigation.h"
#include "ui_feedback.h"
#include "firmware_version.h"
#include "generated/ui.h"
#include "ui_splash_asset.h"
#include <stdio.h>
#include <string.h>

static int current_width;
static uint8_t current_rotation;

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

void ui_view_create(ui_control_callback_t callback)
{
    ui_init();
    ui_navigation_init();
    ui_controls_init(callback);
    ui_feedback_init();
    lv_image_set_src(objects.network_logo,&ui_logo_header);
    lv_image_set_src(objects.settings_logo,&ui_logo_header);
    lv_image_set_src(objects.debug_logo,&ui_logo_header);
    lv_image_set_src(objects.about_logo,&ui_logo_header);
    lv_image_set_src(objects.about_symbol,&ui_logo_about);
    lv_image_set_src(objects.boot_symbol,&ui_logo_splash);
    label(objects.firmware_version,"Firmware " UI_FIRMWARE_VERSION);
    current_width=0;
    current_rotation=255;
}

void ui_view_update(const ui_model_t *m, const ui_diagnostics_t *d, uint32_t now)
{
    const int width=ui_model_width(m), height=ui_model_height(m);
    if (current_width!=width) {
        ui_controls_close(); ui_layout_apply(width,height); ui_feedback_layout(); current_width=width;
    }
    const unsigned page_index=ui_navigation_index(m->page);
    const ui_navigation_entry_t *page=ui_navigation_entry(page_index);
    lv_obj_t *screen=m->splash?objects.boot:page->screen;
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
    if (!lv_dropdown_is_open(page->menu)) lv_dropdown_set_selected(page->menu,page_index);
    if (!lv_slider_is_dragged(objects.brightness_slider)) lv_slider_set_value(objects.brightness_slider,m->brightness,LV_ANIM_OFF);
    snprintf(text,sizeof text,"%u%%",m->brightness);
    label(objects.brightness_value,text);
    if (!lv_dropdown_is_open(objects.rotation_select)) lv_dropdown_set_selected(objects.rotation_select,m->rotation);
    if (!lv_dropdown_is_open(objects.timeout_select)) lv_dropdown_set_selected(objects.timeout_select,m->timeout_index);
    number(objects.touches_value,d->touches,true); number(objects.uptime_value,d->uptime_s,true);
    number(objects.frames_value,d->frames,true); number(objects.errors_value,d->draw_errors,true);
    number(objects.heap_value,d->heap_kib,true);
}
