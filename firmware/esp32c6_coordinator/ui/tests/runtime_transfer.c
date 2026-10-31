/* Execute the real UI task/flush callbacks with deferred DMA completion.
 * Unlike the preview driver this keeps wire storage busy across flushes. */
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include "../ui_runtime.c"
#include "generated/screens.h"

static jmp_buf task_exit;
static unsigned saved_rotation,mode,submitted,completed,waits,allocations;
static void *allocated[3];
static const uint16_t *pending_pixels,*previous_pixels;
static uint16_t pending_copy[BOARD_LCD_TILE_PIXELS];
static unsigned pending_count,total_pixels;
static bool lit,deleted;

void *heap_caps_malloc(size_t bytes,unsigned caps)
{
    assert(allocations<3 && bytes==UI_MAX_WIDTH*UI_TILE_ROWS*2);
    assert(caps==(allocations?MALLOC_CAP_DMA:MALLOC_CAP_8BIT));
    return allocated[allocations++]=malloc(bytes);
}
void heap_caps_free(void *ptr)
{
    for(unsigned i=0;i<allocations;++i) if(allocated[i]==ptr) allocated[i]=NULL;
    free(ptr);
}
uint32_t esp_get_free_heap_size(void) { return 140*1024; }
int64_t esp_timer_get_time(void) { return 100000; }
void esp_restart(void) { assert(false); }
void vTaskDelay(uint32_t ticks) { assert(ticks>=1 && lit);longjmp(task_exit,1); }
void vTaskDelete(void *task) { assert(task==NULL);deleted=true;longjmp(task_exit,1); }
int xTaskCreate(void (*entry)(void *),const char *name,unsigned stack,void *arg,unsigned priority,void *handle)
{
    (void)name;(void)stack;(void)priority;(void)handle;entry(arg);assert(false);return 0;
}
bool board_identity_read(char out[18]) { strcpy(out,"AC:EB:E6:39:FE:EC");return true; }
bool ui_settings_load(ui_model_t *m) { m->rotation=saved_rotation;return true; }
bool ui_settings_save(const ui_model_t *m) { (void)m;assert(false);return false; }
esp_err_t board_display_init(void) { return ESP_OK; }
bool board_touch_present(void) { return true; }
bool board_touch_read(uint16_t *x,uint16_t *y) { (void)x;(void)y;return false; }
esp_err_t board_display_set_brightness(uint8_t percent) { assert(percent==100 && !lit);return ESP_OK; }
esp_err_t board_display_set_awake(bool awake)
{
    assert(awake && !pending_pixels && total_pixels==UI_WIDTH*UI_HEIGHT);
    assert(completed==submitted && diagnostics.frames>=1);lit=true;return ESP_OK;
}
esp_err_t board_display_submit(int x0,int y0,int x1,int y1,const uint16_t *pixels)
{
    assert(!pending_pixels && pixels!=previous_pixels);
    assert(x0>=0 && y0>=0 && x1<=UI_WIDTH && y1<=UI_HEIGHT);
    if(mode==2) return ESP_ERR_INVALID_STATE;
    pending_count=(x1-x0)*(y1-y0);
    assert(pending_count<=BOARD_LCD_TILE_PIXELS);
    pending_pixels=previous_pixels=pixels;
    memcpy(pending_copy,pixels,pending_count*2);++submitted;
    /* Return with the data still owned by DMA. */
    return ESP_OK;
}
esp_err_t board_display_wait(void)
{
    ++waits;
    if(!pending_pixels) return ESP_OK;
    assert(!memcmp(pending_pixels,pending_copy,pending_count*2));
    if(mode==1) return ESP_ERR_TIMEOUT;
    total_pixels+=pending_count;pending_pixels=NULL;++completed;return ESP_OK;
}
int main(int argc,char **argv)
{
    assert(argc==3);saved_rotation=(unsigned)atoi(argv[1]);mode=(unsigned)atoi(argv[2]);
    if(!setjmp(task_exit)) coordinator_ui_start();
    assert(model.rotation==saved_rotation && frame_rotation==saved_rotation);
    if(mode) {
        assert(deleted && !lit && diagnostics.draw_errors==1 && render_failed);
        const unsigned count=submitted;
        lv_area_t tile={0,0,0,0};uint16_t pixel=0;
        flush_pixels(lv_display_get_default(),&tile,(uint8_t *)&pixel);
        assert(submitted==count && diagnostics.draw_errors==1);
        if(pending_pixels) assert(!memcmp(pending_pixels,pending_copy,pending_count*2));
        assert(allocated[1] && allocated[2]); /* Retained after failure. */
    } else {
        assert(lit && !deleted && !diagnostics.draw_errors && submitted>1);
        assert(waits>submitted && completed==submitted);
        assert(lv_screen_active()==objects.boot);
        lv_area_t logo;lv_obj_get_coords(objects.boot_symbol,&logo);
        assert(abs(logo.x1+logo.x2+1-ui_model_width(&model))<=1);
        assert(abs(logo.y1+logo.y2+1-ui_model_height(&model))<=1);
    }
    lv_deinit();
    for(unsigned i=0;i<allocations;++i) free(allocated[i]);
    fprintf(stderr,"PASS: real UI boot/async DMA ownership rotation=%u mode=%u submissions=%u completions=%u\n",
        saved_rotation,mode,submitted,completed);
    return 0;
}
