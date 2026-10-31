idf_component_register(SRCS "board_display.c" "board_identity.c" INCLUDE_DIRS "."
                       REQUIRES esp_lcd esp_driver_spi esp_driver_gpio esp_driver_i2c esp_hw_support)
