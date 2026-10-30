/** @file
 * @brief Optional board-profile status indicator, independent of report logic.
 */
#ifndef SUSTAIN_BOARD_STATUS_H
#define SUSTAIN_BOARD_STATUS_H
#include <stdbool.h>
/**
 * @brief Initialize optional PWM indicators and show the unjoined state.
 * @retval 0 Initialization succeeded, or RGB support is disabled.
 * @retval -ENODEV A configured PWM device is unavailable.
 * @return A PWM driver error if a pulse could not be configured.
 */
int board_status_init(void);

/**
 * @brief Update the optional blue/green connection indication.
 * @param joined True for joined (green), false for unjoined (blue).
 * @note No-op without RGB support. PWM update errors are deliberately ignored;
 * this debug indicator does not control commissioning or radio behavior.
 */
void board_status_joined(bool joined);
#endif
