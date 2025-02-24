#include "bt_controller.h"
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <stdlib.h>
 // Commented and coded by OA  - Fenri24

const struct device *const get_uart_dev(void){ // Fuction responsible for finding and accessing the UART/ HC-05 device  - Fenri24
    return DEVICE_DT_GET(DT_NODELABEL(uart1)); // Returns the UART device by looking for it in the devicetree/ overlay file by label  - Fenri24
}


static uint8_t ring_buffer_storage[RING_BUF_SIZE]; // Variable for storing the ring-buffer and its size  - Fenri24
static struct ring_buf ringbuf; // Ring-buffer variable  - Fenri24
static struct joystick_data last_received_data; // Last-received data variable  - Fenri24
static bool new_data_available = false; // Variable used to represent if new data is available or not  - Fenri24

struct joystick_data get_js_data(){ // Function that allows other parts of the program to access the received joystick data  - Fenri24
    return last_received_data;
};

static void parse_joystick_data(char *data, struct joystick_data *joy_data) { // Function that parses/ filters out the isolated data from the uart callback function - Fenri24
    char *start = strchr(data, 'S'); // Searches for the first occurence of the character 'S' in the string, and the pointer 'start' will point to 'S' if found, or NULL if notv - Fenri24
    char *end = strchr(data, 'E'); // Searches for the first occurence of the character 'E' in the string, and the pointer 'end' will point to 'E' if found, or NULL if not - Fenri24
    if (start && end && start < end) { // If test to check if both 'start' and 'end' are not NULL, and that 'start' is less than 'end', to make sure it comes in the right order - Fenri24
        *end = '\0'; // Terminate the string at the end delimiter - Fenri24
        joy_data->joystickId = (*(start + 1) - '0') != 0; // Extracts the character directlyn after 'S', converts it from ASCII to and integer, and checks if it is not zero, then assigns the value to the variable - Fenri24
        joy_data->x = atoi(start + 3); // Parses an integer staring from three characters after 'S', and thn assigning the value to the variable - Fenri24
        joy_data->y = atoi(strchr(start + 3, ',') + 1); // Finds the comma (',') following the X-values, parses the integer directly after the comma, and assigns the value into the Y-value variable  - Fenri24
        new_data_available = true;  // Signal that new data is available  - Fenri24
        last_received_data = *joy_data;  // Store the last received data  - Fenri24
    }
}

void uart_fifo_callback(const struct device *dev, void *user_data) { // Function that reads data from UART FIFO buffer, processes it and isolates the desired data - Fenri24
    static char data_buffer[128]; // Buffer variable to hold incoming data - Fenri24
    static int data_pos = 0; // Variable for current position in buffer - Fenri24

    while (uart_irq_update(dev) && uart_irq_is_pending(dev)) { // While function that continues to run as long as there is available updates from the UART and there are pending interrupts - Fenri24
        if (uart_irq_rx_ready(dev)) { // If test to check if data is ready to be read - Fenri24
            uint8_t buffer[64]; // Temporary buffer to store data - Fenri24
            int recv_len = uart_fifo_read(dev, buffer, sizeof(buffer)); // Reads data into a temporary buffer and then returnes the number of bytes read - Fenri24
            if (recv_len > 0) { // If test checking if the length of the received buffer is greater than zero - Fenri24
                // Append data to buffer - Fenri24
                for (int i = 0; i < recv_len; i++) { // For-loop running as long as the position read in the buffer is less than the length of the buffer - Fenri24
                    if (buffer[i] == 'S') { // If test that checks for the start of a new packet - Fenri24
                        data_pos = 0; // Reset data position if found - Fenri24
                    }
                    if (data_pos < sizeof(data_buffer) - 1) { // If test that adds data to the data_buffer, while ensuring it does not overflow - Fenri24
                        data_buffer[data_pos++] = buffer[i];
                    }
                    if (buffer[i] == 'E') { // End of packet found, process data - Fenri24
                        data_buffer[data_pos] = '\0'; // Null terminate - Fenri24
                        parse_joystick_data(data_buffer, &last_received_data); // Call for using the function parse_joystick_data - Fenri24
                        data_pos = 0; // Reset for next packet - Fenri24
                    }
                }
            }
        }
    }
}


void process_joystick_data() { // Function that checks if there is new available data - Fenri24
    if (new_data_available) { // If test to check if there is new data available - Fenri24
        //printk("Using joystick data: ID=%d, X=%d, Y=%d\n", last_received_data.joystickId, last_received_data.x, last_received_data.y); - Fenri24
        new_data_available = false;  // Sets the new data variable to false, indicating that there is no new data available at this point - Fenri24
    }
}

void configure_uart(const struct device *dev) { // Fucntion for setting up interrupt-driven UART communication using a ring-buffer - Fenri24
    uart_irq_callback_set(dev, uart_fifo_callback); // Callback function that will be triggered by interrupts - Fenri24
    uart_irq_rx_enable(dev); // Function that enables the receive interrupt for the device, triggering the callback function when there is new data comming in - Fenri24
    ring_buf_init(&ringbuf, RING_BUF_SIZE, ring_buffer_storage); // Buffer used to store incoming data - Fenri24
}

void btBegin(){ // Function responcible for the initialization, configuration and startup of the UART/ HC-05 device - Fenri24
    const struct device *const uart_dev = get_uart_dev(); // Getting the UART device - Fenri24
    if (!device_is_ready(uart_dev)) { // If test checking if the UART device is ready or not - Fenri24
        printk("UART device is not ready\n");
        return;
    }

    configure_uart(uart_dev); // Calling upon the configure_uart function - Fenri24
    printk("Starting to receive data...\n");
}