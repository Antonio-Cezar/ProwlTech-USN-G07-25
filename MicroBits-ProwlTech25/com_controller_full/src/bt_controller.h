#ifndef BT_CONTROLLER_H
#define BT_CONTROLLER_H

#include <zephyr/types.h>
#include <stdbool.h>

// Commented and coded by OA

#define RING_BUF_SIZE 1024 // Defining the size of the ring-buffer

struct joystick_data { // Struct that defines what the joystick-data variable contains
    bool joystickId; // True (1) for the right joystick, False (0) for the left
    int x;
    int y;
};

const struct device *const get_uart_dev(void); // Getting the UART device
struct joystick_data get_js_data(); // Struct that makes the joystick data accessible
void configure_uart(const struct device *dev); // Configures the UART
void uart_fifo_callback(const struct device *dev, void *user_data); // Callback function for reading the UART buffer
void process_joystick_data(); // Function that checks if there is new available data
void btBegin(); // Function responcible for the initialization, configuration and startup of the UART/ HC-05 device

#endif 