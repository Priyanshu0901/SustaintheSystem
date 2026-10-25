#include "observation.h"
#include <errno.h>

static void put16(uint8_t *out, uint16_t value)
{
	out[0] = (uint8_t)value;
	out[1] = (uint8_t)(value >> 8);
}

static void put32(uint8_t *out, uint32_t value)
{
	put16(out, (uint16_t)value);
	put16(out + 2, (uint16_t)(value >> 16));
}

int observation_encode(const struct observation *value,
		       uint8_t output[OBSERVATION_WIRE_SIZE])
{
	if (value == NULL || output == NULL || value->score_milli > 1000 ||
	    value->direction_deg >= 360 || value->role < 1 || value->role > 2 ||
	    (value->flags & ~0x03U) != 0) {
		return -EINVAL;
	}
	output[0] = OBSERVATION_WIRE_VERSION;
	output[1] = value->flags;
	put32(output + 2, value->sequence);
	put32(output + 6, value->uptime_s);
	put16(output + 10, value->score_milli);
	put16(output + 12, value->direction_deg);
	output[14] = value->role;
	output[15] = 0; /* reserved */
	return 0;
}
