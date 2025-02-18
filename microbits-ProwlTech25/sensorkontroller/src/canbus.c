#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <stdbool.h>
#include <string.h>
#include "canbus.h"

//can_dev functions
const struct device *const get_can_dev(void){
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
} 

//can_receive functions
int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data){
    struct can_filter filter = {
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE,
	    .id = RECEIVE_ID,
	    .mask = CAN_EXT_ID_MASK
    };
    return can_add_rx_filter(dev, rx_cb, cb_data, &filter);
}

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) {
    char received_str[9];  // Buffer to store received string, +1 for NULL termination

    if (frame->dlc <= 8) {
        memcpy(&received_str, &frame->data, frame->dlc);
        received_str[frame->dlc] = '\0';  // NULL-terminate the string
        printk("Received string: %s\n", received_str);
    }
}

//can_send functions
/* Function to convert CAN state to readable string */
const char* can_state_to_str(enum can_state state) {
    switch (state) {
        case CAN_STATE_ERROR_ACTIVE:
            return "ERROR ACTIVE";
        case CAN_STATE_ERROR_WARNING:
            return "ERROR WARNING";
        case CAN_STATE_ERROR_PASSIVE:
            return "ERROR PASSIVE";
        case CAN_STATE_BUS_OFF:
            return "BUS OFF";
        case CAN_STATE_STOPPED:
            return "STOPPED";
        default:
            return "UNKNOWN";
    }
}

void send_byte(const struct device *can_dev, uint8_t sensor_byte){
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));

    frame.flags = CAN_FRAME_IDE; // Use extended identifier
    frame.id = MSG_ID;
    frame.dlc = 1;  // Set the DLC to the byte length
    
    frame.data[0] = sensor_byte;  // Copy string to CAN frame data field

    if (can_send(can_dev, &frame, K_SECONDS(10), NULL, NULL) != 0) {
            printk("Failed to send frame\n");
        } /*else {
            printk("Frame sent\n");
        }*/
}
