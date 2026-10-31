#include "ui_controls.h"
#include "ui_feedback.h"
#include "ui_navigation.h"
#include "generated/screens.h"

static ui_control_callback_t on_control;
static lv_obj_t *dropdowns[6];
static bool constraining;
static unsigned dropdown_count;

static void navigate(lv_event_t *event)
{
    const uint32_t selected=lv_dropdown_get_selected(lv_event_get_target_obj(event));
    if (selected<ui_navigation_count() && on_control) on_control(UI_CONTROL_PAGE,ui_navigation_entry(selected)->page);
}
static void choose_rotation(lv_event_t *event)
{
    if (on_control) on_control(UI_CONTROL_ROTATION,(uint8_t)lv_dropdown_get_selected(lv_event_get_target_obj(event)));
}
static void choose_timeout(lv_event_t *event)
{
    if (on_control) on_control(UI_CONTROL_TIMEOUT,(uint8_t)lv_dropdown_get_selected(lv_event_get_target_obj(event)));
}
static void brightness_changed(lv_event_t *event)
{
    const ui_control_t control=lv_event_get_code(event)==LV_EVENT_VALUE_CHANGED?
        UI_CONTROL_BRIGHTNESS_PREVIEW:UI_CONTROL_BRIGHTNESS;
    if (on_control) on_control(control,(uint8_t)lv_slider_get_value(lv_event_get_target_obj(event)));
}
static void reset_display(lv_event_t *event)
{
    (void)event;
    if (on_control) on_control(UI_CONTROL_RESET_DISPLAY,0);
    ui_controls_close();
    lv_obj_scroll_to_y(objects.settings_content,0,LV_ANIM_OFF);
}
static void restart_device(lv_event_t *event)
{
    (void)event;
    if (on_control) on_control(UI_CONTROL_RESTART,0);
}
static void sleep_help(lv_event_t *event)
{
    (void)event;
    ui_feedback_show("Touch to wake.\nRadio stays active.");
}

static void opening(lv_event_t *event)
{
    lv_obj_t *list=lv_dropdown_get_list(lv_event_get_target_obj(event));
    const int width=lv_display_get_horizontal_resolution(NULL);
    const int height=lv_display_get_vertical_resolution(NULL);
    lv_obj_set_style_max_width(list,width-20,0);
    lv_obj_set_style_max_height(list,height-20,0);
}

/* LVGL aligns a dropdown list to its button without clamping horizontal
 * overflow. Run after layout, when both its final size and position are known.
 * Only change a coordinate when necessary, so the next layout converges. */
static void constrain_popups(lv_event_t *event)
{
    (void)event;
    if (constraining) return;
    constraining=true;
    const int width=lv_display_get_horizontal_resolution(NULL);
    const int height=lv_display_get_vertical_resolution(NULL);
    for (unsigned i=0;i<dropdown_count;++i) {
        if (!lv_dropdown_is_open(dropdowns[i])) continue;
        lv_obj_t *list=lv_dropdown_get_list(dropdowns[i]);
        const int w=lv_obj_get_width(list),h=lv_obj_get_height(list);
        int x=lv_obj_get_x(list),y=lv_obj_get_y(list);
        if (x+w>width-10) x=width-10-w;
        if (y+h>height-10) y=height-10-h;
        if (x<10) x=10;
        if (y<10) y=10;
        if (x!=lv_obj_get_x(list) || y!=lv_obj_get_y(list)) lv_obj_set_pos(list,x,y);
    }
    constraining=false;
}

void ui_controls_close(void)
{
    for (unsigned i=0;i<dropdown_count;++i) lv_dropdown_close(dropdowns[i]);
}

void ui_controls_init(ui_control_callback_t callback)
{
    on_control=callback;
    const unsigned pages=ui_navigation_count();
    for (unsigned i=0;i<pages;++i) dropdowns[i]=ui_navigation_entry(i)->menu;
    dropdowns[pages]=objects.rotation_select;
    dropdowns[pages+1]=objects.timeout_select;
    dropdown_count=pages+2;
    for (unsigned i=0;i<dropdown_count;++i) {
        lv_obj_t *list=lv_dropdown_get_list(dropdowns[i]);
        lv_obj_set_style_text_font(list,&lv_font_montserrat_14,0);
        lv_obj_set_style_min_width(list,140,0);
        lv_obj_set_scrollbar_mode(list,LV_SCROLLBAR_MODE_ACTIVE);
        lv_obj_set_style_pad_all(list,8,0);
        lv_obj_set_style_text_line_space(list,8,0);
        lv_obj_set_style_bg_color(list,lv_color_hex(0x202624),0);
        lv_obj_set_style_border_color(list,lv_color_hex(0x4C615A),0);
        lv_obj_set_style_text_color(list,lv_color_hex(0xF2F5F4),0);
        lv_obj_set_style_bg_color(list,lv_color_hex(0x335D51),LV_PART_SELECTED|LV_STATE_CHECKED);
        lv_obj_set_style_text_color(list,lv_color_hex(0xF2F5F4),LV_PART_SELECTED|LV_STATE_CHECKED);
        lv_obj_add_event_cb(dropdowns[i],opening,LV_EVENT_READY,NULL);
        if (i<pages) {
            lv_dropdown_set_options(dropdowns[i],ui_navigation_options());
            lv_dropdown_set_symbol(dropdowns[i],NULL);
            lv_obj_add_event_cb(dropdowns[i],navigate,LV_EVENT_VALUE_CHANGED,NULL);
        }
    }
    lv_obj_add_event_cb(objects.rotation_select,choose_rotation,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_event_cb(objects.timeout_select,choose_timeout,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_event_cb(objects.reset_button,reset_display,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(objects.restart_button,restart_device,LV_EVENT_CLICKED,NULL);
    lv_obj_set_ext_click_area(objects.sleep_info,8);
    lv_obj_set_ext_click_area(objects.brightness_slider,12);
    lv_obj_add_event_cb(objects.brightness_slider,brightness_changed,LV_EVENT_VALUE_CHANGED,NULL);
    lv_obj_add_event_cb(objects.brightness_slider,brightness_changed,LV_EVENT_RELEASED,NULL);
    lv_obj_add_event_cb(objects.brightness_slider,brightness_changed,LV_EVENT_PRESS_LOST,NULL);
    lv_obj_add_event_cb(objects.sleep_info,sleep_help,LV_EVENT_CLICKED,NULL);
    lv_display_add_event_cb(lv_display_get_default(),constrain_popups,LV_EVENT_UPDATE_LAYOUT_COMPLETED,NULL);
}
