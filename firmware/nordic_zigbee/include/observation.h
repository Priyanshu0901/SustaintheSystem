/** @file
 * @brief Portable observation model, simulator and wire encoder.
 */
#ifndef SUSTAIN_OBSERVATION_H
#define SUSTAIN_OBSERVATION_H

#include <stddef.h>
#include <stdint.h>

#define OBSERVATION_WIRE_VERSION 1U /**< Supported wire schema. */
#define OBSERVATION_WIRE_SIZE 16U /**< Encoded record size, without ZCL length. */
#define OBSERVATION_FLAG_SIMULATED 0x01U /**< Data came from a synthetic source. */
#define OBSERVATION_FLAG_ANOMALY 0x02U /**< Producer classified an anomaly. */

/**
 * @brief Application snapshot independent of Zephyr, board and Zigbee types.
 *
 * This is an in-memory model. Use observation_encode() instead of casting it
 * to bytes; C structure layout is not the wire representation.
 */
struct observation {
	uint32_t sequence; /**< Generated counter; restarts at boot and wraps. */
	uint32_t uptime_s; /**< Seconds since boot, modulo 2^32. */
	uint16_t score_milli; /**< 0..1000; not a calibrated probability. */
	uint16_t direction_deg; /**< 0..359; synthetic in the test application. */
	uint8_t flags; /**< Combination of OBSERVATION_FLAG_*; other bits zero. */
	uint8_t role; /**< 1 = powered router; 2 = sleepy end device. */
};

/**
 * @brief Encode a validated snapshot as a fixed-size little-endian v1 record.
 * @param[in] value Snapshot to validate; never retained or modified.
 * @param[out] output Caller-owned buffer of at least OBSERVATION_WIRE_SIZE bytes.
 * @retval 0 Record written, including its zero reserved byte.
 * @retval -EINVAL Null pointer, invalid score/direction/role or reserved flag bits.
 * @note Validation completes before output is written. No heap or radio APIs.
 */
int observation_encode(const struct observation *value,
		       uint8_t output[OBSERVATION_WIRE_SIZE]);

/**
 * @brief Advance the deterministic, single-producer simulated source.
 * @param uptime_s Caller-supplied uptime in seconds.
 * @param role Caller-supplied role; use 1 or 2 for an encodable record.
 * @return New snapshot with simulated flag set; sequence starts at 1.
 * @note Uses a static counter; call from one producer thread. Score is 900 every
 * tenth generated record and 100 otherwise; direction cycles in 15-degree steps.
 */
struct observation simulated_source_next(uint32_t uptime_s, uint8_t role);

#endif
