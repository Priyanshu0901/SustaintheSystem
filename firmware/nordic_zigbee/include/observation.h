#ifndef SUSTAIN_OBSERVATION_H
#define SUSTAIN_OBSERVATION_H

#include <stddef.h>
#include <stdint.h>

#define OBSERVATION_WIRE_VERSION 1U
#define OBSERVATION_WIRE_SIZE 16U
#define OBSERVATION_FLAG_SIMULATED 0x01U
#define OBSERVATION_FLAG_ANOMALY 0x02U

/* Application data, independent of Zephyr, board and Zigbee types.
 * Sequence restarts at boot; uptime is seconds since boot.
 */
struct observation {
	uint32_t sequence;
	uint32_t uptime_s;
	uint16_t score_milli;      /* 0..1000, NOT a calibrated probability */
	uint16_t direction_deg;   /* 0..359; no localization is measured here */
	uint8_t flags;
	uint8_t role;             /* 1=router, 2=sleepy end device */
};

/* Encodes a fixed-size little-endian v1 record; rejects invalid values. */
int observation_encode(const struct observation *value,
		       uint8_t output[OBSERVATION_WIRE_SIZE]);
struct observation simulated_source_next(uint32_t uptime_s, uint8_t role);

#endif
