#include "observation.h"

struct observation simulated_source_next(uint32_t uptime_s, uint8_t role)
{
	static uint32_t sequence;
	const uint32_t current = ++sequence;
	const uint16_t score = (current % 10U == 0U) ? 900U : 100U;

	return (struct observation) {
		.sequence = current,
		.uptime_s = uptime_s,
		.score_milli = score,
		.direction_deg = (uint16_t)((current % 24U) * 15U),
		.flags = OBSERVATION_FLAG_SIMULATED |
			 ((score >= 800U) ? OBSERVATION_FLAG_ANOMALY : 0U),
		.role = role,
	};
}
