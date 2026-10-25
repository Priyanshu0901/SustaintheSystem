#include "board_status.h"
#include <zephyr/kernel.h>
#if defined(CONFIG_SUSTAIN_STATUS_RGB)
#include <zephyr/drivers/pwm.h>

static const struct pwm_dt_spec red = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
static const struct pwm_dt_spec blue = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led1));
static const struct pwm_dt_spec green = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led2));

int board_status_init(void)
{
	if (!pwm_is_ready_dt(&red) || !pwm_is_ready_dt(&blue) ||
	    !pwm_is_ready_dt(&green)) {
		return -ENODEV;
	}
	int err = pwm_set_pulse_dt(&red, 0);
	if (!err) { err = pwm_set_pulse_dt(&green, 0); }
	if (!err) { err = pwm_set_pulse_dt(&blue, blue.period / 16); }
	return err;
}

void board_status_joined(bool joined)
{
	/* Deliberately dim; this debug indicator is not a power measurement mode. */
	(void)pwm_set_pulse_dt(&blue, joined ? 0 : blue.period / 16);
	(void)pwm_set_pulse_dt(&green, joined ? green.period / 16 : 0);
}
#else
int board_status_init(void) { return 0; }
void board_status_joined(bool joined) { ARG_UNUSED(joined); }
#endif
