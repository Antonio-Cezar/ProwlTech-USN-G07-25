#ifndef BT_CONTROLLER_H
#define BT_CONTROLLER_H

#include <zephyr/types.h>
#include <stdbool.h>

struct joystick_data {
    bool joystickId; // True (1) for the right joystick, False (0) for the left
    int x;
    int y;
};

const struct device *const get_uart_dev(void);
struct joystick_data get_js_data();
void configure_uart(const struct device *dev);
void uart_fifo_callback(const struct device *dev, void *user_data);
void process_joystick_data();
void btBegin();

#endif 