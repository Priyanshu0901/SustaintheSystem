#pragma once
#include "ui_view.h"
/** Bind generated controls and constrain every popup to the display gutter. */
void ui_controls_init(ui_control_callback_t callback);
/** Close popups before a logical-resolution change. */
void ui_controls_close(void);
