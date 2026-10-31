#include "ui_font.h"
#include "fonts/ui_font_data.h"

const ui_font_t *ui_font_get(ui_font_size_t size)
{
    return &ui_fonts[size<=UI_FONT_NUMBER?(unsigned)size:0];
}
const ui_font_glyph_t *ui_font_glyph(const ui_font_t *font, char c)
{
    const unsigned code=(unsigned char)c;
    return &font->glyphs[(code>=32 && code<=126)?code-32:'?'-32];
}
int ui_font_width(ui_font_size_t size, const char *text)
{
    const ui_font_t *font=ui_font_get(size);
    int width=0;
    for (;*text;++text) width+=ui_font_glyph(font,*text)->advance;
    return width;
}
