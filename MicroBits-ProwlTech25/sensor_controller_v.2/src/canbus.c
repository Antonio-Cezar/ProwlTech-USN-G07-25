#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <stdbool.h>
#include <string.h>
#include "canbus.h"

// Commented and coded by OA

// can_dev
const struct device *const get_can_dev(void){ // Fuction responsible for finding and accessing the CAN device
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus)); // Returns the CAN device by looking for it in the devicetree/ overlay file under "chosen"
} 

// can_receive
int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data){ // Function for setting up the can filter, taking in the CAN device, callback function and cb data
    struct can_filter filter_str = { // CAN filter for a string
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE, // Setting the flags for the filter, where the filter matches the data frames, and that the filter matches CAN frames with extended ID (29-bit)
	    .id = RECEIVE_ID_STR, // Filter ID set to match the sender ID of the string, defined in the header file.
	    .mask = CAN_EXT_ID_MASK // Filter bit mask for an extended 29-bit CAN ID
    };
    return can_add_rx_filter(dev, rx_cb, cb_data, &filter_str); // Adds a callback function for the given filter
}

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) {  // Function for checking CAN frames on the bus
    char received_str[9];  // Buffer to store received string, +1 for NULL termination

    if (frame->dlc <= 8) { // If test for checking if the frame-ID matches the receive ID for the string frame, and if the size of the frame is equal to or less than 8 byte
        memcpy(&received_str, &frame->data, frame->dlc); // Copies the data from the received CAN frame into received_str buffer
        received_str[frame->dlc] = '\0';  // NULL-terminate the string
        //printk("Received string: %s\n", received_str);
    }
}


// can_send
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

void send_byte(const struct device *can_dev, uint8_t sensor_byte){ // Function for sending the sensor-byte data over the bus
    struct can_frame frame; // Defining a CAN frame called frame
    memset(&frame, 0, sizeof(frame)); // Zeros out the frame to ensure that all bits starts in a knwown state

    frame.flags = CAN_FRAME_IDE; // Use extended identifier
    frame.id = MSG_ID; // Using defined message ID
    frame.dlc = 1;  // Defines the length of the byte in the frame to 1
    
    frame.data[0] = sensor_byte;  // Places the boolean value from the sensor into the first byte of the data field

    //printk("Sending byte");
    if (can_send(can_dev, &frame, K_SECONDS(10), NULL, NULL) != 0) { // If test to check if the CAN frame is sent or not
            printk("Failed to send frame\n");
        } /*else {
            printk("Frame sent\n");
        }*/
}