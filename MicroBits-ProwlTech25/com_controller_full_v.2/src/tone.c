#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include "tone.h"

#define SPEAKER_PWM_NODE DT_NODELABEL(pwm0)
#define SPEAKER_CHANNEL 0  // P0 = channel 0 på pwm0

const struct device *pwm_dev = DEVICE_DT_GET(SPEAKER_PWM_NODE);

void tone_init(void) {
    if (!device_is_ready(pwm_dev)) {
        printk("PWM device not ready!\n");
    }
}

void play_tone(void) {
    if (!device_is_ready(pwm_dev)) return;

    // 1 kHz tone, 50% duty cycle
    pwm_set(pwm_dev, SPEAKER_CHANNEL, 1000000U, 500000U, 0);
}

void stop_tone(void) {
    if (!device_is_ready(pwm_dev)) return;

    // Stopp tone (duty = 0)
    pwm_set(pwm_dev, SPEAKER_CHANNEL, 1000000U, 0, 0);
}
