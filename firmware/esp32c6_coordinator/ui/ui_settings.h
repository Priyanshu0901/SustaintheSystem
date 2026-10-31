#pragma once
#include "ui_model.h"
/** Read only display fields; retain caller defaults for absent keys. No NVS init/erase. */
bool ui_settings_load(ui_model_t *model);
/** Persist a completed selection, never a brightness drag sample. */
bool ui_settings_save(const ui_model_t *model);
