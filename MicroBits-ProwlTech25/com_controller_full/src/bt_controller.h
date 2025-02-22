#ifndef BT_CONTROLLER_H
#define BT_CONTROLLER_H

#include <zephyr/types.h>
#include <stdbool.h>

// Commented and coded by OA

#define RING_BUF_SIZE 1024 // Defining the size of the ring-buffer - Fenri24

struct joystick_data { // Struct that defines what the joystick-data variable contains - Fenri24
    bool joystickId; // True (1) for the right joystick, False (0) for the left - Fenri24
    int x;
    int y;
};

const struct device *const get_uart_dev(void); // Getting the UART device - Fenri24
struct joystick_data get_js_data(); // Struct that makes the joystick data accessible - Fenri24
void configure_uart(const struct device *dev); // Configures the UART - Fenri24
void uart_fifo_callback(const struct device *dev, void *user_data); // Callback function for reading the UART buffer - Fenri24
void process_joystick_data(); // Function that checks if there is new available data - Fenri24
void btBegin(); // Function responcible for the initialization, configuration and startup of the UART/ HC-05 device - Fenri24

#endif 