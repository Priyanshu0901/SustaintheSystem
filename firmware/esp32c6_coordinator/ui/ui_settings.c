#include "ui_settings.h"
#include "nvs.h"

static bool read_value(nvs_handle_t handle,const char *key,uint8_t *target,uint8_t min,uint8_t max)
{
    uint8_t value;
    const esp_err_t err=nvs_get_u8(handle,key,&value);
    if (err==ESP_ERR_NVS_NOT_FOUND) return true;
    if (err!=ESP_OK || value<min || value>max) return false;
    *target=value;
    return true;
}

bool ui_settings_load(ui_model_t *model)
{
    nvs_handle_t handle;
    const esp_err_t err=nvs_open("display",NVS_READONLY,&handle);
    if (err==ESP_ERR_NVS_NOT_FOUND) return true;
    if (err!=ESP_OK) return false;
    bool ok=read_value(handle,"rotation",&model->rotation,0,3);
    ok=read_value(handle,"timeout",&model->timeout_index,0,3) && ok;
    ok=read_value(handle,"brightness",&model->brightness,10,100) && ok;
    model->brightness_committed=model->brightness;
    nvs_close(handle);
    return ok;
}

bool ui_settings_save(const ui_model_t *model)
{
    nvs_handle_t handle;
    if (nvs_open("display",NVS_READWRITE,&handle)!=ESP_OK) return false;
    esp_err_t err=nvs_set_u8(handle,"rotation",model->rotation);
    if (err==ESP_OK) err=nvs_set_u8(handle,"timeout",model->timeout_index);
    if (err==ESP_OK) err=nvs_set_u8(handle,"brightness",model->brightness_committed);
    if (err==ESP_OK) err=nvs_commit(handle);
    nvs_close(handle);
    return err==ESP_OK;
}
