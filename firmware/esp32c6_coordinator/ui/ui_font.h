#pragma once
#include <stdint.h>

typedef enum { UI_FONT_SMALL, UI_FONT_NORMAL, UI_FONT_NUMBER } ui_font_size_t;
typedef struct {
    uint32_t offset;
    uint8_t width, height;
    int8_t left, top;
    uint8_t advance;
} ui_font_glyph_t;
typedef struct {
    const uint8_t *bitmap;
    const ui_font_glyph_t *glyphs;
    uint8_t ascent, line_height;
} ui_font_t;
const ui_font_t *ui_font_get(ui_font_size_t size);
const ui_font_glyph_t *ui_font_glyph(const ui_font_t *font, char character);
int ui_font_width(ui_font_size_t size, const char *text);
