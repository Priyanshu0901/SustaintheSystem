#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void coordinator_ui_start(void);
/** Handle only UI vendor requests. Returns common-status + response bytes,
 * or zero for all other commands. Never calls the radio or serial driver. */
size_t coordinator_ui_command(uint16_t command, const uint8_t *request, size_t length,
                              uint8_t *response, size_t capacity);
#ifdef __cplusplus
}
#endif
