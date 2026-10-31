#pragma once
#include <stdbool.h>
/** Read the six-byte base address into a 17-character string plus terminator.
 * The SDK buffer reserves its full eight-byte maximum on IEEE802.15.4 SoCs. */
bool board_identity_read(char output[18]);
