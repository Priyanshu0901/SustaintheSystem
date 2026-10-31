#pragma once
#include <stdbool.h>
/** UI-task-only transient feedback, independently of model and NVS storage. */
void ui_feedback_init(void);
/** Reflow a visible toast when the logical display resolution changes. */
void ui_feedback_layout(void);
void ui_feedback_show(const char *message);
/** Success is silent; show failures and dismiss a stale save error on success. */
void ui_feedback_save_result(bool saved);
