#include "sensors.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys_clock.h>
#include <zephyr/timing/timing.h>

void hc_sr04_init(hc_sr04_t *sensor, const char *gpio_dev_name, uint8_t trig_pin, uint8_t echo_pin) {
    sensor->gpio_dev = device_get_binding(gpio_dev_name);
    __ASSERT(sensor->gpio_dev != NULL, "Failed to bind to GPIO device");

    sensor->trig_pin = trig_pin;
    sensor->echo_pin = echo_pin;

    gpio_pin_configure(sensor->gpio_dev, sensor->trig_pin, GPIO_OUTPUT);
    gpio_pin_configure(sensor->gpio_dev, sensor->echo_pin, GPIO_INPUT);
}

double hc_sr04_read_distance(hc_sr04_t *sensor) {
    uint32_t start_time;
    uint32_t stop_time;
    uint32_t cycles_spent;
    uint64_t time_spent;
    double distance;

    gpio_pin_set(sensor->gpio_dev, sensor->trig_pin, 1);
    k_sleep(K_USEC(10));
    gpio_pin_set(sensor->gpio_dev, sensor->trig_pin, 0);

    while (gpio_pin_get(sensor->gpio_dev, sensor->echo_pin) == 0);
    start_time = k_cycle_get_32();

    while (gpio_pin_get(sensor->gpio_dev, sensor->echo_pin) == 1) {
        stop_time = k_cycle_get_32();
        cycles_spent = stop_time - start_time;
        if (cycles_spent > 1113600) {
            printk("Timeout reached, no object detected\n");
            return -1.0; // Error value
        }
    }

    time_spent = k_cyc_to_us_floor64(cycles_spent); // Time spent in microseconds
    distance = ((double)time_spent * 0.0343) / 2.0; // Calculate distance in cm

    return distance;
}