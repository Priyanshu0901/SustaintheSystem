/** @file
 * @brief Application-thread interface to the ZBOSS observation transport.
 */
#ifndef SUSTAIN_ZIGBEE_TRANSPORT_H
#define SUSTAIN_ZIGBEE_TRANSPORT_H
#include "observation.h"
/**
 * @brief Register the endpoint, configure the selected role and enable Zigbee.
 * @note Call once during startup. Uses normal persisted ZBOSS network state;
 * starting does not establish that commissioning or joining has completed.
 */
void zigbee_transport_start(void);

/**
 * @brief Copy a validated observation into the thread-safe latest-value slot.
 * @param[in] value Snapshot to encode; the pointer is not retained.
 * @retval 0 A ZBOSS callback was scheduled; delivery is not guaranteed.
 * @retval -EINVAL The snapshot fails observation_encode() validation.
 * @retval -ENOTCONN Node is not joined; pending data is not updated.
 * @retval -EAGAIN Callback scheduling failed after updating the pending slot.
 * @note Call from an application thread. Packet APIs run only in ZBOSS context.
 * Accepted observations may coalesce while a transmission is in flight. There
 * is no offline history or application acknowledgement. A later periodic
 * submission retries scheduling; no automatic retry timer is installed here.
 */
int zigbee_transport_submit(const struct observation *value);
#endif
