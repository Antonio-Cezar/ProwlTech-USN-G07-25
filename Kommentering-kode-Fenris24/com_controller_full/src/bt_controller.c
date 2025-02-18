#include "bt_controller.h"
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <stdlib.h>
 // Commented and coded by OA

const struct device *const get_uart_dev(void){ // Fuction responsible for finding and accessing the UART/ HC-05 device
    return DEVICE_DT_GET(DT_NODELABEL(uart1)); // Returns the UART device by looking for it in the devicetree/ overlay file by label
}


static uint8_t ring_buffer_storage[RING_BUF_SIZE]; // Variable for storing the ring-buffer and its size
static struct ring_buf ringbuf; // Ring-buffer variable
static struct joystick_data last_received_data; // Last-received data variable
static bool new_data_available = false; // Variable used to represent if new data is available or not

struct joystick_data get_js_data(){ // Function that allows other parts of the program to access the received joystick data
    return last_received_data;
};

static void parse_joystick_data(char *data, struct joystick_data *joy_data) { // Function that parses/ filters out the isolated data from the uart callback function
    char *start = strchr(data, 'S'); // Searches for the first occurence of the character 'S' in the string, and the pointer 'start' will point to 'S' if found, or NULL if not
    char *end = strchr(data, 'E'); // Searches for the first occurence of the character 'E' in the string, and the pointer 'end' will point to 'E' if found, or NULL if not
    if (start && end && start < end) { // If test to check if both 'start' and 'end' are not NULL, and that 'start' is less than 'end', to make sure it comes in the right order
        *end = '\0'; // Terminate the string at the end delimiter
        joy_data->joystickId = (*(start + 1) - '0') != 0; // Extracts the character directlyn after 'S', converts it from ASCII to and integer, and checks if it is not zero, then assigns the value to the variable
        joy_data->x = atoi(start + 3); // Parses an integer staring from three characters after 'S', and thn assigning the value to the variable
        joy_data->y = atoi(strchr(start + 3, ',') + 1); // Finds the comma (',') following the X-values, parses the integer directly after the comma, and assigns the value into the Y-value variable
        new_data_available = true;  // Signal that new data is available
        last_received_data = *joy_data;  // Store the last received data
    }
}

void uart_fifo_callback(const struct device *dev, void *user_data) { // Function that reads data from UART FIFO buffer, processes it and isolates the desired data
    static char data_buffer[128]; // Buffer variable to hold incoming data
    static int data_pos = 0; // Variable for current position in buffer

    while (uart_irq_update(dev) && uart_irq_is_pending(dev)) { // While function that continues to run as long as there is available updates from the UART and there are pending interrupts
        if (uart_irq_rx_ready(dev)) { // If test to check if data is ready to be read
            uint8_t buffer[64]; // Temporary buffer to store data
            int recv_len = uart_fifo_read(dev, buffer, sizeof(buffer)); // Reads data into a temporary buffer and then returnes the number of bytes read
            if (recv_len > 0) { // If test checking if the length of the received buffer is greater than zero
                // Append data to buffer
                for (int i = 0; i < recv_len; i++) { // For-loop running as long as the position read in the buffer is less than the length of the buffer
                    if (buffer[i] == 'S') { // If test that checks for the start of a new packet
                        data_pos = 0; // Reset data position if found
                    }
                    if (data_pos < sizeof(data_buffer) - 1) { // If test that adds data to the data_buffer, while ensuring it does not overflow
                        data_buffer[data_pos++] = buffer[i];
                    }
                    if (buffer[i] == 'E') { // End of packet found, process data
                        data_buffer[data_pos] = '\0'; // Null terminate
                        parse_joystick_data(data_buffer, &last_received_data); // Call for using the function parse_joystick_data
                        data_pos = 0; // Reset for next packet
                    }
                }
            }
        }
    }
}


void process_joystick_data() { // Function that checks if there is new available data
    if (new_data_available) { // If test to check if there is new data available
        //printk("Using joystick data: ID=%d, X=%d, Y=%d\n", last_received_data.joystickId, last_received_data.x, last_received_data.y);
        new_data_available = false;  // Sets the new data variable to false, indicating that there is no new data available at this point
    }
}

void configure_uart(const struct device *dev) { // Fucntion for setting up interrupt-driven UART communication using a ring-buffer
    uart_irq_callback_set(dev, uart_fifo_callback); // Callback function that will be triggered by interrupts
    uart_irq_rx_enable(dev); // Function that enables the receive interrupt for the device, triggering the callback function when there is new data comming in
    ring_buf_init(&ringbuf, RING_BUF_SIZE, ring_buffer_storage); // Buffer used to store incoming data
}

void btBegin(){ // Function responcible for the initialization, configuration and startup of the UART/ HC-05 device
    const struct device *const uart_dev = get_uart_dev(); // Getting the UART device
    if (!device_is_ready(uart_dev)) { // If test checking if the UART device is ready or not
        printk("UART device is not ready\n");
        return;
    }

    configure_uart(uart_dev); // Calling upon the configure_uart function
    printk("Starting to receive data...\n");
}