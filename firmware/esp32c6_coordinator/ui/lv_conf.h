#ifndef LV_CONF_H
#define LV_CONF_H

/* Identical on host and ESP-IDF. Only the UI task calls LVGL. */
#define LV_COLOR_DEPTH 16
#define LV_USE_OS LV_OS_NONE
#define LV_USE_STDLIB_MALLOC LV_STDLIB_BUILTIN
#define LV_MEM_SIZE (72U * 1024U)
#define LV_USE_LOG 0
#define LV_USE_ASSERT_NULL 1
#define LV_USE_ASSERT_MALLOC 1
#define LV_USE_DRAW_SW 1
#define LV_DRAW_SW_DRAW_UNIT_CNT 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_DEFAULT &lv_font_montserrat_14
#define LV_USE_DROPDOWN 1
#define LV_USE_LABEL 1
#define LV_USE_IMAGE 1
#define LV_USE_BUTTON 1
#define LV_USE_FLEX 1
#define LV_USE_GRID 1
#define LV_BUILD_EXAMPLES 0
#define LV_BUILD_DEMOS 0
#define LV_USE_THORVG 0
#define LV_USE_LODEPNG 0
#define LV_USE_TJPGD 0
#define LV_USE_LIBPNG 0
#define LV_USE_LIBJPEG_TURBO 0
#define LV_USE_FFMPEG 0
#define LV_USE_FREETYPE 0
#define LV_USE_TINY_TTF 0
#define LV_USE_XML 0
#define LV_USE_SDL 0
#define LV_USE_LINUX_FBDEV 0
#define LV_USE_LINUX_DRM 0
#define LV_USE_WAYLAND 0
#define LV_USE_OPENGLES 0
#endif
