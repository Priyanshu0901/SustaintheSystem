#include "board_backlight.h"
#include "board_pins.h"
#include "ui_settings.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "nvs.h"
#include <assert.h>
#include <string.h>

static uint32_t duty;
static bool set_fails,update_fails,timer_fails;
static bool present[3];
static uint8_t values[3];
static int open_error,commit_error;
unsigned test_nvs_commits;

esp_err_t gpio_config(const gpio_config_t *config)
{
    assert(config->pin_bit_mask==(1ULL<<BOARD_LCD_BACKLIGHT) && config->mode==GPIO_MODE_OUTPUT);
    return ESP_OK;
}
esp_err_t gpio_set_level(int pin,unsigned level) { assert(pin==BOARD_LCD_BACKLIGHT && level==1);return ESP_OK; }
esp_err_t ledc_timer_config(const ledc_timer_config_t *config)
{
    assert(config->speed_mode==LEDC_LOW_SPEED_MODE && config->duty_resolution==10 && config->freq_hz==10000);
    return timer_fails?-5:ESP_OK;
}
esp_err_t ledc_channel_config(const ledc_channel_config_t *config)
{
    assert(config->gpio_num==BOARD_LCD_BACKLIGHT && config->flags.output_invert && config->duty==0);
    duty=config->duty;return ESP_OK;
}
esp_err_t ledc_set_duty(int mode,int channel,uint32_t value)
{
    assert(mode==LEDC_LOW_SPEED_MODE && channel==LEDC_CHANNEL_0 && value<=1023);
    if (set_fails) { set_fails=false;return -7; }
    duty=value;return ESP_OK;
}
esp_err_t ledc_update_duty(int mode,int channel)
{
    assert(mode==LEDC_LOW_SPEED_MODE && channel==LEDC_CHANNEL_0);
    if (update_fails) { update_fails=false;return -8; }
    return ESP_OK;
}
static unsigned key_index(const char *key)
{
    const char *keys[]={"rotation","timeout","brightness"};
    for(unsigned i=0;i<3;++i) if(!strcmp(keys[i],key)) return i;
    assert(false);return 0;
}
esp_err_t nvs_open(const char *name,int mode,nvs_handle_t *handle)
{
    assert(!strcmp(name,"display") && (mode==NVS_READONLY || mode==NVS_READWRITE));
    *handle=1;return open_error;
}
esp_err_t nvs_get_u8(nvs_handle_t handle,const char *key,uint8_t *value)
{
    assert(handle==1);const unsigned i=key_index(key);
    if(!present[i]) return ESP_ERR_NVS_NOT_FOUND;
    *value=values[i];return ESP_OK;
}
esp_err_t nvs_set_u8(nvs_handle_t handle,const char *key,uint8_t value)
{
    assert(handle==1);const unsigned i=key_index(key);present[i]=true;values[i]=value;return ESP_OK;
}
esp_err_t nvs_commit(nvs_handle_t handle) { assert(handle==1);++test_nvs_commits;return commit_error; }
void nvs_close(nvs_handle_t handle) { assert(handle==1); }

void hardware_settings_tests(void)
{
    timer_fails=true;assert(board_backlight_init()==-5);timer_fails=false;
    assert(board_backlight_init()==ESP_OK && duty==0);
    assert(board_backlight_set_brightness(35)==ESP_OK && duty==0);
    assert(board_backlight_set_awake(true)==ESP_OK && duty==358);
    uint32_t previous=0;
    for(unsigned i=10;i<=100;++i) {
        assert(board_backlight_set_brightness(i)==ESP_OK && duty>previous);previous=duty;
    }
    assert(duty==1023);
    assert(board_backlight_set_brightness(0)==ESP_ERR_INVALID_ARG);
    assert(board_backlight_set_brightness(101)==ESP_ERR_INVALID_ARG);
    set_fails=true;assert(board_backlight_set_awake(false)==-7);
    assert(board_backlight_set_brightness(20)==ESP_OK && duty==205); /* Still awake after failure. */
    update_fails=true;assert(board_backlight_set_brightness(30)==-8);
    assert(board_backlight_set_awake(false)==ESP_OK && duty==0);
    assert(board_backlight_set_awake(true)==ESP_OK && duty==205); /* Failed update did not replace preference. */
    assert(board_backlight_set_awake(false)==ESP_OK);
    assert(board_backlight_set_brightness(72)==ESP_OK && duty==0);
    assert(board_backlight_set_awake(true)==ESP_OK && duty==737);

    ui_model_t model;ui_model_init(&model,0);
    open_error=ESP_ERR_NVS_NOT_FOUND;assert(ui_settings_load(&model) && model.brightness==100);
    open_error=0;present[0]=present[1]=true;values[0]=1;values[1]=3;
    assert(ui_settings_load(&model) && model.rotation==1 && model.timeout_index==3 && model.brightness==100);
    present[2]=true;values[2]=9;assert(!ui_settings_load(&model) && model.brightness==100);
    values[2]=72;assert(ui_settings_load(&model) && model.brightness==72 && model.brightness_committed==72);
    assert(ui_model_control(&model,UI_CONTROL_BRIGHTNESS_PREVIEW,25)==UI_ACTION_REDRAW);
    assert(ui_settings_save(&model) && values[2]==72); /* A different control cannot persist a drag sample. */
    assert(ui_model_control(&model,UI_CONTROL_BRIGHTNESS,25)==UI_ACTION_SETTINGS);
    assert(ui_settings_save(&model) && values[2]==25);
    commit_error=-9;assert(!ui_settings_save(&model));commit_error=0;
    open_error=-10;assert(!ui_settings_load(&model) && !ui_settings_save(&model));open_error=0;
    test_nvs_commits=0;
}
