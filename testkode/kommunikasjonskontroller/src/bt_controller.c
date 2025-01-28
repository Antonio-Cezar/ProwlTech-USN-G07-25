#include "bt_controller.h"
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <stdlib.h>

#define RING_BUF_SIZE 1024

const struct device *const get_uart_dev(void){
    return DEVICE_DT_GET(DT_NODELABEL(uart1));
}


static uint8_t ring_buffer_storage[RING_BUF_SIZE];
static struct ring_buf ringbuf;
static struct joystick_data last_received_data;
static bool new_data_available = false;

struct joystick_data get_js_data(){
    return last_received_data;
};

static void parse_joystick_data(char *data, struct joystick_data *joy_data) {
    char *start = strchr(data, 'S');
    char *end = strchr(data, 'E');
    if (start && end && start < end) {
        *end = '\0'; // Terminate the string at the end delimiter
        joy_data->joystickId = (*(start + 1) - '0') != 0;
        joy_data->x = atoi(start + 3);
        joy_data->y = atoi(strchr(start + 3, ',') + 1);
        new_data_available = true;  // Signal that new data is available
        last_received_data = *joy_data;  // Store the last received data
    }
}

void uart_fifo_callback(const struct device *dev, void *user_data) {
    static char data_buffer[128]; // Buffer to hold incoming data
    static int data_pos = 0; // Current position in buffer

    while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
        if (uart_irq_rx_ready(dev)) {
            uint8_t buffer[64];
            int recv_len = uart_fifo_read(dev, buffer, sizeof(buffer));
            if (recv_len > 0) {
                // Append data to buffer
                for (int i = 0; i < recv_len; i++) {
                    if (buffer[i] == 'S') {
                        // Start of new packet, reset data position
                        data_pos = 0;
                    }
                    if (data_pos < sizeof(data_buffer) - 1) {
                        data_buffer[data_pos++] = buffer[i];
                    }
                    if (buffer[i] == 'E') {
                        // End of packet, process data
                        data_buffer[data_pos] = '\0'; // Null terminate
                        parse_joystick_data(data_buffer, &last_received_data);
                        data_pos = 0; // Reset for next packet
                        // At this point, `joy_data` can be passed to a motor controller or other components
                    }
                }
            }
        }
    }
}


void process_joystick_data() {
    if (new_data_available) {
        printk("Using joystick data: ID=%d, X=%d, Y=%d\n", last_received_data.joystickId, last_received_data.x, last_received_data.y);
        new_data_available = false;  // Reset the flag after processing
    }
}

void configure_uart(const struct device *dev) {
    uart_irq_callback_set(dev, uart_fifo_callback);
    uart_irq_rx_enable(dev);
    ring_buf_init(&ringbuf, RING_BUF_SIZE, ring_buffer_storage);
}

void btBegin(){
    const struct device *const uart_dev = get_uart_dev();
    if (!device_is_ready(uart_dev)) {
        printk("UART device is not ready\n");
        return;
    }

    configure_uart(uart_dev);
    printk("Starting to receive data...\n");
}