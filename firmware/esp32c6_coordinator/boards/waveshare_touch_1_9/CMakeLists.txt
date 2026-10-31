idf_component_register(SRCS "board_display.c" "board_identity.c" "board_backlight.c" INCLUDE_DIRS "."
                       REQUIRES esp_lcd esp_driver_spi esp_driver_gpio esp_driver_i2c esp_driver_ledc esp_hw_support)
