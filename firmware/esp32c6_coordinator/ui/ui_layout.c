#include "ui_layout.h"
#include "generated/screens.h"

static void place(lv_obj_t *obj, int x, int y, int w, int h)
{
    lv_obj_set_pos(obj,x,y); lv_obj_set_size(obj,w,h);
}

void ui_layout_apply(int width, int height)
{
    const bool wide=width>height;
    lv_obj_t *screens[]={objects.network,objects.settings,objects.debug,objects.about,objects.boot};
    lv_obj_t *logos[]={objects.network_logo,objects.settings_logo,objects.debug_logo,objects.about_logo};
    for (unsigned i=0;i<5;++i) {
        lv_obj_set_size(screens[i],width,height);
        lv_obj_remove_flag(screens[i],LV_OBJ_FLAG_SCROLLABLE);
    }
    lv_obj_t *menus[]={objects.network_navigation,objects.settings_navigation,objects.debug_navigation,objects.about_navigation};
    const char *titles[]={LV_SYMBOL_WIFI "  Network",LV_SYMBOL_SETTINGS "  Settings",LV_SYMBOL_EYE_OPEN "  Debug",LV_SYMBOL_FILE "  About"};
    const char *icons[]={LV_SYMBOL_WIFI,LV_SYMBOL_SETTINGS,LV_SYMBOL_EYE_OPEN,LV_SYMBOL_FILE};
    for (unsigned i=0;i<4;++i) {
        /* Align using the image object's actual size, including non-square logos. */
        lv_obj_align(logos[i],LV_ALIGN_TOP_RIGHT,-10,10);
        place(menus[i],10,6,wide?154:40,36);
        lv_dropdown_set_text(menus[i],wide?titles[i]:icons[i]);
    }
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
    place(objects.timeout_select,wide?168:10,wide?81:163,wide?142:150,38);
    place(objects.sleep_info,wide?280:130,wide?47:129,30,32);
    place(objects.timeout_caption,wide?168:10,wide?57:139,wide?108:112,18);

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
    place(objects.firmware_version,wide?105:10,wide?120:232,wide?205:150,18);
    place(objects.library_version,wide?105:10,wide?145:264,wide?205:150,18);
    place(objects.firmware_role,10,294,150,18);
    if (wide) lv_obj_add_flag(objects.firmware_role,LV_OBJ_FLAG_HIDDEN);
    else lv_obj_remove_flag(objects.firmware_role,LV_OBJ_FLAG_HIDDEN);
    lv_obj_center(objects.boot_symbol);
}
