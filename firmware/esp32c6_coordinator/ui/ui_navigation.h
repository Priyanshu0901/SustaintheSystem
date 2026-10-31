#pragma once
#include "ui_model.h"
#include "lvgl.h"
typedef struct {
    ui_page_t page;
    lv_obj_t *screen, *menu, *logo;
    const char *icon, *heading;
} ui_navigation_entry_t;
/** Ordered menu registry; initialize after EEZ creates named objects. */
void ui_navigation_init(void);
unsigned ui_navigation_count(void);
const ui_navigation_entry_t *ui_navigation_entry(unsigned index);
unsigned ui_navigation_index(uint8_t page);
const char *ui_navigation_options(void);
