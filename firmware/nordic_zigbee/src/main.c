#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "board_status.h"
#include "zigbee_transport.h"

LOG_MODULE_REGISTER(sustain_node, LOG_LEVEL_INF);

int main(void)
{
	int err = board_status_init();
	if (err) {
		LOG_ERR("Status indicator unavailable: %d", err);
		return err;
	}
	LOG_INF("SIMULATED observations; interval=%d s; role=%s",
		CONFIG_SUSTAIN_REPORT_INTERVAL_SECONDS,
		IS_ENABLED(CONFIG_SUSTAIN_SLEEPY) ? "sleepy end device" : "router");
	zigbee_transport_start();
	for (;;) {
		struct observation value = simulated_source_next(
			(uint32_t)(k_uptime_get() / 1000),
			IS_ENABLED(CONFIG_SUSTAIN_SLEEPY) ? 2 : 1);
		err = zigbee_transport_submit(&value);
		LOG_INF("Generated seq=%u simulated=1 enqueue=%d", value.sequence, err);
		k_sleep(K_SECONDS(CONFIG_SUSTAIN_REPORT_INTERVAL_SECONDS));
	}
}
