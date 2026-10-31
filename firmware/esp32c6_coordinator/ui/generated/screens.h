#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_NETWORK = 1,
    SCREEN_ID_SETTINGS = 2,
    SCREEN_ID_DEBUG = 3,
    SCREEN_ID_ABOUT = 4,
    SCREEN_ID_BOOT = 5,
    _SCREEN_ID_LAST = 5
};

typedef struct _objects_t {
    lv_obj_t *network;
    lv_obj_t *settings;
    lv_obj_t *debug;
    lv_obj_t *about;
    lv_obj_t *boot;
    lv_obj_t *network_navigation;
    lv_obj_t *network_logo;
    lv_obj_t *network_card;
    lv_obj_t *network_state;
    lv_obj_t *host_state;
    lv_obj_t *registered_caption;
    lv_obj_t *registered_value;
    lv_obj_t *channel_caption;
    lv_obj_t *channel_value;
    lv_obj_t *available_caption;
    lv_obj_t *available_value;
    lv_obj_t *stale_caption;
    lv_obj_t *stale_value;
    lv_obj_t *unknown_value;
    lv_obj_t *joining_value;
    lv_obj_t *settings_navigation;
    lv_obj_t *settings_logo;
    lv_obj_t *rotation_caption;
    lv_obj_t *rotation_select;
    lv_obj_t *timeout_caption;
    lv_obj_t *timeout_select;
    lv_obj_t *sleep_info;
    lv_obj_t *debug_navigation;
    lv_obj_t *debug_logo;
    lv_obj_t *touches_caption;
    lv_obj_t *touches_value;
    lv_obj_t *uptime_caption;
    lv_obj_t *uptime_value;
    lv_obj_t *frames_caption;
    lv_obj_t *frames_value;
    lv_obj_t *errors_caption;
    lv_obj_t *errors_value;
    lv_obj_t *heap_caption;
    lv_obj_t *heap_value;
    lv_obj_t *about_navigation;
    lv_obj_t *about_logo;
    lv_obj_t *about_symbol;
    lv_obj_t *maker_caption;
    lv_obj_t *maker_name;
    lv_obj_t *device_name;
    lv_obj_t *firmware_version;
    lv_obj_t *library_version;
    lv_obj_t *firmware_role;
    lv_obj_t *boot_symbol;
} objects_t;

extern objects_t objects;

void create_screen_network();
void tick_screen_network();

void create_screen_settings();
void tick_screen_settings();

void create_screen_debug();
void tick_screen_debug();

void create_screen_about();
void tick_screen_about();

void create_screen_boot();
void tick_screen_boot();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/