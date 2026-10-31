#include "ui_navigation.h"
#include "ui_config.h"
#include "generated/screens.h"

static ui_navigation_entry_t entries[4];
void ui_navigation_init(void)
{
    entries[0]=(ui_navigation_entry_t){UI_PAGE_NETWORK,objects.network,objects.network_navigation,objects.network_logo,LV_SYMBOL_WIFI,LV_SYMBOL_WIFI "  Network"};
    entries[1]=(ui_navigation_entry_t){UI_PAGE_SETTINGS,objects.settings,objects.settings_navigation,objects.settings_logo,LV_SYMBOL_SETTINGS,LV_SYMBOL_SETTINGS "  Settings"};
    entries[2]=(ui_navigation_entry_t){UI_PAGE_ABOUT,objects.about,objects.about_navigation,objects.about_logo,LV_SYMBOL_FILE,LV_SYMBOL_FILE "  About"};
#if UI_SHOW_DEBUG_PAGE
    entries[3]=(ui_navigation_entry_t){UI_PAGE_DEBUG,objects.debug,objects.debug_navigation,objects.debug_logo,LV_SYMBOL_EYE_OPEN,LV_SYMBOL_EYE_OPEN "  Debug"};
#endif
}
unsigned ui_navigation_count(void) { return UI_SHOW_DEBUG_PAGE?4:3; }
const ui_navigation_entry_t *ui_navigation_entry(unsigned index)
{
    return &entries[index<ui_navigation_count()?index:0];
}
unsigned ui_navigation_index(uint8_t page)
{
    for(unsigned i=0;i<ui_navigation_count();++i) if(entries[i].page==page) return i;
    return 0;
}
const char *ui_navigation_options(void)
{
    return LV_SYMBOL_WIFI "  Network\n" LV_SYMBOL_SETTINGS "  Settings\n" LV_SYMBOL_FILE "  About"
#if UI_SHOW_DEBUG_PAGE
        "\n" LV_SYMBOL_EYE_OPEN "  Debug"
#endif
        ;
}
