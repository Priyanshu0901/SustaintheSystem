#include "ui_feedback.h"
#include "lvgl.h"

static lv_obj_t *toast;
static lv_timer_t *expiry;
static bool save_error;

static void hide(lv_timer_t *timer)
{
    lv_obj_add_flag(toast,LV_OBJ_FLAG_HIDDEN);
    save_error=false;
    lv_timer_pause(timer);
}

void ui_feedback_init(void)
{
    toast=lv_label_create(lv_layer_top());
    lv_label_set_long_mode(toast,LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(toast,&lv_font_montserrat_12,0);
    lv_obj_set_style_text_color(toast,lv_color_hex(0xF2F5F4),0);
    lv_obj_set_style_bg_color(toast,lv_color_hex(0x263D35),0);
    lv_obj_set_style_bg_opa(toast,LV_OPA_COVER,0);
    lv_obj_set_style_radius(toast,8,0);
    lv_obj_set_style_pad_all(toast,10,0);
    lv_obj_set_style_text_align(toast,LV_TEXT_ALIGN_CENTER,0);
    lv_obj_add_flag(toast,LV_OBJ_FLAG_HIDDEN);
    expiry=lv_timer_create(hide,3000,NULL);
    lv_timer_pause(expiry);
}

void ui_feedback_layout(void)
{
    const int width=lv_display_get_horizontal_resolution(NULL);
    lv_obj_set_width(toast,width-20);
    lv_obj_align(toast,LV_ALIGN_BOTTOM_MID,0,-10);
}

void ui_feedback_show(const char *message)
{
    save_error=false;
    lv_label_set_text(toast,message);
    lv_obj_remove_flag(toast,LV_OBJ_FLAG_HIDDEN);
    ui_feedback_layout();
    lv_timer_reset(expiry);
    lv_timer_resume(expiry);
}

void ui_feedback_save_result(bool saved)
{
    if (!saved) {
        ui_feedback_show(LV_SYMBOL_WARNING "  Could not save settings");
        save_error=true;
    } else if (save_error) hide(expiry); /* Dismiss stale failure, no success popup. */
}
