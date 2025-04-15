#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include "tone.h"

#define PWM_CTLR_NODE DT_ALIAS(pwm0)
const struct device *pwm_dev = DEVICE_DT_GET(PWM_CTLR_NODE);

void tone_init(void) {
    if (!device_is_ready(pwm_dev)) {
        printk("PWM device not ready!\n");
    }
}

void play_tone(void) {
    if (!device_is_ready(pwm_dev)) return;
    pwm_set(pwm_dev, 0, 1000000U, 500000U, 0);  // 1kHz tone, 50% duty
}

void stop_tone(void) {
    if (!device_is_ready(pwm_dev)) return;
    pwm_set(pwm_dev, 0, 1000000U, 0, 0);  // 0% duty = av
}
