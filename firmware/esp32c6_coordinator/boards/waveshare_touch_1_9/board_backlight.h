#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
/** UI-task-only PWM backlight; starts off, active-low GPIO15. */
esp_err_t board_backlight_init(void);
/** 10..100 percent. Changing a sleeping screen's level never illuminates it. */
esp_err_t board_backlight_set_brightness(uint8_t percent);
/** Restore the selected brightness on wake; zero duty while asleep. */
esp_err_t board_backlight_set_awake(bool awake);
