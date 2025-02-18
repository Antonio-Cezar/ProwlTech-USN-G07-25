#ifndef SENSORS_H
#define SENSORS_H
//This code is used with HC_SR04 sensors
#include <zephyr/types.h>

// Structure to hold sensor configuration
typedef struct {
    const struct device *gpio_dev;
    uint8_t trig_pin;
    uint8_t echo_pin;
} hc_sr04_t;

void hc_sr04_init(hc_sr04_t *sensor, const char *gpio_dev_name, uint8_t trig_pin, uint8_t echo_pin);
double hc_sr04_read_distance(hc_sr04_t *sensor);

#endif // HC_SR04_H