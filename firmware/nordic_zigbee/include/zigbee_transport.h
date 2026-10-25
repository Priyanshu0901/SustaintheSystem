#ifndef SUSTAIN_ZIGBEE_TRANSPORT_H
#define SUSTAIN_ZIGBEE_TRANSPORT_H
#include "observation.h"
void zigbee_transport_start(void);
/* Thread-safe latest-value handoff. Returns -ENOTCONN while unjoined,
 * -EAGAIN when scheduler is full. No offline history/delivery guarantee.
 * Accepted observations may coalesce while a transmission is in flight.
 */
int zigbee_transport_submit(const struct observation *value);
#endif
