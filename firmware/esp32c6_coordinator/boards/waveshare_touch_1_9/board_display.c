#include "board_display.h"
#include "board_backlight.h"
#include "board_pins.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static esp_lcd_panel_handle_t panel;
static i2c_master_dev_handle_t touch;
static SemaphoreHandle_t transferred;
static bool touch_ready;

static bool transfer_done(esp_lcd_panel_io_handle_t io, esp_lcd_panel_io_event_data_t *event, void *ctx)
{
    BaseType_t wake = pdFALSE;
    xSemaphoreGiveFromISR(transferred, &wake);
    return wake == pdTRUE;
}

esp_err_t board_display_init(void)
{
    transferred = xSemaphoreCreateBinary();
    if (!transferred) return ESP_ERR_NO_MEM;
    esp_err_t err = board_backlight_init();
    if (err != ESP_OK) return err;
    spi_bus_config_t bus = {.mosi_io_num = BOARD_LCD_MOSI, .miso_io_num = -1,
        .sclk_io_num = BOARD_LCD_CLK, .quadwp_io_num = -1, .quadhd_io_num = -1,
        .max_transfer_sz = 320 * 16 * 2};
    if ((err = spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_CH_AUTO)) != ESP_OK) return err;
    esp_lcd_panel_io_handle_t io;
    esp_lcd_panel_io_spi_config_t spi = {.cs_gpio_num = BOARD_LCD_CS, .dc_gpio_num = BOARD_LCD_DC,
        .spi_mode = 0, .pclk_hz = 20000000, .trans_queue_depth = 1,
        .on_color_trans_done = transfer_done, .lcd_cmd_bits = 8, .lcd_param_bits = 8};
    if ((err = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &spi, &io)) != ESP_OK) return err;
    esp_lcd_panel_dev_config_t cfg = {.reset_gpio_num = BOARD_LCD_RESET,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB, .bits_per_pixel = 16};
    if ((err = esp_lcd_new_panel_st7789(io, &cfg, &panel)) != ESP_OK) return err;
    if ((err = esp_lcd_panel_reset(panel)) != ESP_OK) return err;
    if ((err = esp_lcd_panel_init(panel)) != ESP_OK) return err;
    if ((err = esp_lcd_panel_set_gap(panel, BOARD_LCD_COLUMN_OFFSET, 0)) != ESP_OK) return err;
    if ((err = esp_lcd_panel_invert_color(panel, true)) != ESP_OK) return err;
    if ((err = esp_lcd_panel_disp_on_off(panel, true)) != ESP_OK) return err;

    i2c_master_bus_handle_t i2c;
    i2c_master_bus_config_t i2c_cfg = {.i2c_port = I2C_NUM_0,
        .sda_io_num = BOARD_I2C_SDA, .scl_io_num = BOARD_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT, .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true};
    if (i2c_new_master_bus(&i2c_cfg, &i2c) == ESP_OK &&
        i2c_master_probe(i2c, BOARD_TOUCH_ADDRESS, 50) == ESP_OK) {
        i2c_device_config_t dev = {.dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = BOARD_TOUCH_ADDRESS, .scl_speed_hz = 200000};
        if (i2c_master_bus_add_device(i2c, &dev, &touch) == ESP_OK) {
            const uint8_t normal[] = {0, 0};
            touch_ready = i2c_master_transmit(touch, normal, sizeof normal, 50) == ESP_OK;
        }
    }
    return ESP_OK;
}

esp_err_t board_display_tile(int x0, int y0, int x1, int y1, const uint16_t *pixels)
{
    if (x0<0 || x1>170 || y0<0 || y1>320 || x1<=x0 || y1<=y0 ||
        (x1-x0)*(y1-y0)>320*16) return ESP_ERR_INVALID_ARG;
    esp_err_t err = esp_lcd_panel_draw_bitmap(panel, x0, y0, x1, y1, pixels);
    if (err != ESP_OK) return err;
    if (xSemaphoreTake(transferred, pdMS_TO_TICKS(500)) != pdTRUE) return ESP_ERR_TIMEOUT;
    return ESP_OK;
}

esp_err_t board_display_set_awake(bool awake)
{
    if (!awake) {
        const esp_err_t err=board_backlight_set_awake(false);
        if (err!=ESP_OK) return err;
    }
    const esp_err_t err = esp_lcd_panel_disp_on_off(panel, awake);
    if (err == ESP_OK && awake) return board_backlight_set_awake(true);
    return err;
}

esp_err_t board_display_set_brightness(uint8_t percent) { return board_backlight_set_brightness(percent); }

bool board_touch_present(void) { return touch_ready; }
bool board_touch_read(uint16_t *x, uint16_t *y)
{
    uint8_t reg = 0, data[7];
    if (!touch_ready || i2c_master_transmit_receive(touch, &reg, 1, data, sizeof data, 20) != ESP_OK)
        return false;
    if (!data[2]) return false;
    *x = ((uint16_t)(data[3] & 15) << 8) | data[4];
    *y = ((uint16_t)(data[5] & 15) << 8) | data[6];
    return *x < 170 && *y < 320;
}
