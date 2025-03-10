#include <zephyr/kernel.h>
#include "pwm.h"

void main(void) {
    int ret = pwm_begin();
    if (ret < 0) {
        printk("Feil ved initialisering av PWM, kode: %d\n", ret);
        return;
    }

    while (1) {
        // Kjør fremover
        printk("Kjører fremover\n");
        pwm_write(MOTOR_FRONT_LEFT, (uint32_t)speed_to_ds(mapValue(50, 1))); 
        pwm_write(MOTOR_FRONT_RIGHT, (uint32_t)speed_to_ds(mapValue(50, 1)));
        pwm_write(MOTOR_REAR_LEFT, (uint32_t)speed_to_ds(mapValue(50, 1)));
        pwm_write(MOTOR_REAR_RIGHT, (uint32_t)speed_to_ds(mapValue(50, 1)));
        k_sleep(K_SECONDS(3));

        // Stopp
        printk("Stopp\n");
        pwm_write(MOTOR_FRONT_LEFT, 0);
        pwm_write(MOTOR_FRONT_RIGHT, 0);
        pwm_write(MOTOR_REAR_LEFT, 0);
        pwm_write(MOTOR_REAR_RIGHT, 0);
        k_sleep(K_SECONDS(1));

        // Kjør bakover
        printk("Kjører bakover\n");
        pwm_write(MOTOR_FRONT_LEFT, (uint32_t)speed_to_ds(mapValue(50, 2))); 
        pwm_write(MOTOR_FRONT_RIGHT, (uint32_t)speed_to_ds(mapValue(50, 2)));
        pwm_write(MOTOR_REAR_LEFT, (uint32_t)speed_to_ds(mapValue(50, 2)));
        pwm_write(MOTOR_REAR_RIGHT, (uint32_t)speed_to_ds(mapValue(50, 2)));
        k_sleep(K_SECONDS(3));

        // Stopp
        printk("Stopp\n");
        pwm_write(MOTOR_FRONT_LEFT, 0);
        pwm_write(MOTOR_FRONT_RIGHT, 0);
        pwm_write(MOTOR_REAR_LEFT, 0);
        pwm_write(MOTOR_REAR_RIGHT, 0);
        k_sleep(K_SECONDS(1));
    }
}
