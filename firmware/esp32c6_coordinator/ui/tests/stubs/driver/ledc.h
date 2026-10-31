#pragma once
#include <stdint.h>
#include "esp_err.h"
enum { LEDC_LOW_SPEED_MODE=0, LEDC_CHANNEL_0=0, LEDC_TIMER_0=0,
    LEDC_TIMER_10_BIT=10, LEDC_AUTO_CLK=0, LEDC_INTR_DISABLE=0 };
typedef struct { int speed_mode,duty_resolution,timer_num,freq_hz,clk_cfg; } ledc_timer_config_t;
typedef struct { int gpio_num,speed_mode,channel,intr_type,timer_sel; uint32_t duty,hpoint;
    struct { unsigned output_invert:1; } flags; } ledc_channel_config_t;
esp_err_t ledc_timer_config(const ledc_timer_config_t *config);
esp_err_t ledc_channel_config(const ledc_channel_config_t *config);
esp_err_t ledc_set_duty(int mode,int channel,uint32_t duty);
esp_err_t ledc_update_duty(int mode,int channel);
