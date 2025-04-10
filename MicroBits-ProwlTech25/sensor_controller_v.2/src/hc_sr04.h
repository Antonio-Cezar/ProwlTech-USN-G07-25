#ifndef HC_SR04_H
#define HC_SR04_H

#include <zephyr/types.h>
// Commented and coded by OA

// Structure to hold sensor configuration
typedef struct {
    const struct device *gpio_dev;
    uint8_t trig_pin;
    uint8_t echo_pin;
} hc_sr04_t;

void hc_sr04_init(hc_sr04_t *sensor, const char *gpio_dev_name, uint8_t trig_pin, uint8_t echo_pin); //Function for initialization of the sensor(s), parameters are which sensor to use, the GPIO name/ label, followed by which pin is to be used as trigger and echo
double hc_sr04_read_distance(hc_sr04_t *sensor); // Function used to read distance, uses one parameter, which is which sensor to use

#endif // HC_SR04_H
