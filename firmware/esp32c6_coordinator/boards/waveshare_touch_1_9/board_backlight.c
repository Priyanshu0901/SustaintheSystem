#include "board_backlight.h"
#include "board_pins.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

static uint8_t brightness=100;
static bool awake;

static esp_err_t apply(bool enabled, uint8_t percent)
{
    const uint32_t duty=enabled?((uint32_t)percent*1023U+50U)/100U:0;
    esp_err_t err=ledc_set_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,duty);
    if (err==ESP_OK) err=ledc_update_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0);
    return err;
}

esp_err_t board_backlight_init(void)
{
    /* Hold the P-channel transistor off before routing the inverted PWM. */
    gpio_config_t gpio={.pin_bit_mask=1ULL<<BOARD_LCD_BACKLIGHT,.mode=GPIO_MODE_OUTPUT};
    esp_err_t err=gpio_config(&gpio);
    if (err!=ESP_OK) return err;
    if ((err=gpio_set_level(BOARD_LCD_BACKLIGHT,1))!=ESP_OK) return err;
    ledc_timer_config_t timer={.speed_mode=LEDC_LOW_SPEED_MODE,.duty_resolution=LEDC_TIMER_10_BIT,
        .timer_num=LEDC_TIMER_0,.freq_hz=10000,.clk_cfg=LEDC_AUTO_CLK};
    if ((err=ledc_timer_config(&timer))!=ESP_OK) return err;
    ledc_channel_config_t channel={.gpio_num=BOARD_LCD_BACKLIGHT,.speed_mode=LEDC_LOW_SPEED_MODE,
        .channel=LEDC_CHANNEL_0,.intr_type=LEDC_INTR_DISABLE,.timer_sel=LEDC_TIMER_0,
        .duty=0,.hpoint=0,.flags.output_invert=1};
    err=ledc_channel_config(&channel);
    if (err==ESP_OK) { awake=false; brightness=100; }
    return err;
}

esp_err_t board_backlight_set_brightness(uint8_t percent)
{
    if (percent<10 || percent>100) return ESP_ERR_INVALID_ARG;
    const esp_err_t err=apply(awake,percent);
    if (err==ESP_OK) brightness=percent;
    return err;
}

esp_err_t board_backlight_set_awake(bool enabled)
{
    const esp_err_t err=apply(enabled,brightness);
    if (err==ESP_OK) awake=enabled;
    return err;
}
