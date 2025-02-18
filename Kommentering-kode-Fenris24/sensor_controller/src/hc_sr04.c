#include "hc_sr04.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys_clock.h>
#include <zephyr/timing/timing.h>
// Commented and coded by OA

void hc_sr04_init(hc_sr04_t *sensor, const char *gpio_dev_name, uint8_t trig_pin, uint8_t echo_pin) { //Function for initialization of the sensor(s), parameters are which sensor to use, the GPIO name/ label, followed by which pin is to be used as trigger and echo
    sensor->gpio_dev = device_get_binding(gpio_dev_name); // Getting the GPIO name
    __ASSERT(sensor->gpio_dev != NULL, "Failed to bind to GPIO device"); // Check to let the user know if the GPIO device was unable to be binded

    sensor->trig_pin = trig_pin; // Setting the chosen sensors trigger pin as the inputed trigger pin
    sensor->echo_pin = echo_pin; // Setting the chosen sensors echo pin as the inputed echo pin

    gpio_pin_configure(sensor->gpio_dev, sensor->trig_pin, GPIO_OUTPUT); // Configuring the chosen sensors trigger pin as an OUTPUT pin
    gpio_pin_configure(sensor->gpio_dev, sensor->echo_pin, GPIO_INPUT); // Configuring the chosen sensors echo pin as an INPUT pin
}

double hc_sr04_read_distance(hc_sr04_t *sensor) { // Function used to read distance, uses one parameter, which is which sensor to use
    uint32_t start_time;
    uint32_t stop_time;
    uint32_t cycles_spent;
    uint64_t time_spent;
    double distance;

    gpio_pin_set(sensor->gpio_dev, sensor->trig_pin, 1); // Setting trigger pin high to send out ultrasonic sound wave
    k_sleep(K_USEC(10)); // Allowing it to remain high for 10 micro seconds
    gpio_pin_set(sensor->gpio_dev, sensor->trig_pin, 0); // Setting the trigger pin low to stop sending sound waves

    while (gpio_pin_get(sensor->gpio_dev, sensor->echo_pin) == 0); // Remain in while when the echo pin is low
    start_time = k_cycle_get_32(); // Setting the start time as the current cyckle

    while (gpio_pin_get(sensor->gpio_dev, sensor->echo_pin) == 1) { //While the echo pin is high:
        stop_time = k_cycle_get_32(); // Set the stop time as the current cycle
        cycles_spent = stop_time - start_time; // Findig out how many cycles that have passed by subtrackting the stop time from start time
        if (cycles_spent > 1600000) { // If the cycles spent is above 1600000 then there is no object detected. 1600000 cycles is equal to 25ms at 64 MHz, and then 425 cm using the formula for calculating distance of the HC-SR04
            printk("Timeout reached, no object detected\n");
            return -1.0; // Error value
        }
    }

    time_spent = k_cyc_to_us_floor64(cycles_spent); // Time spent in microseconds using cycles spent
    distance = ((double)time_spent * 0.0343) / 2.0; // Calculate distance in cm

    return distance;
}
