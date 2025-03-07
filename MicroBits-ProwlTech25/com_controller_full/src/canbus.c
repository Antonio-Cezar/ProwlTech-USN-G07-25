#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include "canbus.h"
#include <stdio.h>
// Commented and coded by OA

uint8_t received_byte;

void canBegin(void){ // Function for startring up and initializing the CAN device
    const struct device *const can_dev = get_can_dev(); // Getting the CAN device connected to the system
    
    if (!can_dev) { // Test that checks is the CAN device is found. If not it will print the error message and return
        printk("Cannot find CAN device!\n");
        return;
    }

    int ret = can_start(can_dev); // Starting up the CAN device
    if (ret != 0) { // Check to see if the CAN device has started, if not it will print the error message and return
        printf("Failed to start CAN controller: Error %d\n", ret);
        return;
    }
    if (setup_can_filter(can_dev, can_rx_callback, NULL) < 0) { // Check to see if the CAN filter is set up or not
        printk("Failed to add CAN filter\n");
        return;
    }
}

uint8_t get_sensor_byte(){ // Function that allows other parts of the program to access the received sensor byte
    return received_byte;
}

const struct device *const get_can_dev(void){ // Fuction responsible for finding and accessing the CAN device
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus)); // Returns the CAN device by looking for it in the devicetree/ overlay file under "chosen"
} 



int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data){ // Function for setting up the can filter, taking in the CAN device, callback function and cb data
    struct can_filter filter_str = { // CAN filter for a string
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE, // Setting the flags for the filter, where the filter matches the data frames, and that the filter matches CAN frames with extended ID (29-bit)
	    .id = RECEIVE_ID_STR, // Filter ID set to match the sender ID of the string, defined in the header file.
	    .mask = CAN_EXT_ID_MASK // Filter bit mask for an extended 29-bit CAN ID
    };

    struct can_filter filter_byte = { // CAN filter for a byte
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE, // Setting the flags for the filter, where the filter matches the data frames, and that the filter matches CAN frames with extended ID (29-bit)
	    .id = RECEIVE_ID_BYTE, // Filter ID set to match the sender ID of the byte, defined in the header file.
	    .mask = CAN_EXT_ID_MASK // Filter bit mask for an extended 29-bit CAN ID
    };

    int filter_id_str = can_add_rx_filter(dev, rx_cb, cb_data, &filter_str); // Setting the filter for the string as a variable
    int filter_id_byte = can_add_rx_filter(dev, rx_cb, cb_data, &filter_byte); // Setting the filter for the byte as a variable

    return (filter_id_str >= 0 && filter_id_byte >= 0) ? 0 : -1;  // Return 0 if both filters are added successfully
}

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) { // Function for checking CAN frames on the bus
    if (frame->id == RECEIVE_ID_STR && frame->dlc <= 8) { // If test for checking if the frame-ID matches the receive ID for the string frame, and if the size of the frame is equal to or less than 8 byte
        char received_str[9]; // Character variable to put the string in
        memcpy(received_str, frame->data, frame->dlc); // Copies the data from the received CAN frame into received_str buffer
        received_str[frame->dlc] = '\0'; // NULL terminates it for safe processing
        //printk("Received string from ID 0x%X: %s\n", frame->id, received_str);
    } 
    // Checking if the frame on the BUS has the ID and frame data size as the one sent from the Sensor node
    else if (frame->id == RECEIVE_ID_BYTE && frame->dlc == 1) { 
        received_byte = frame->data[0];
        //printk("Received byte from ID 0x%X: %02X\n", frame->id, received_byte);
    } 
    else {
        printk("Unexpected data format or ID\n");
    }
}

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

void send_custom_data (const struct device *can_dev, uint8_t js_bool, int16_t js_deg_int, int16_t js_spd_int){ // Function for sending joystick data over the bus
    struct can_frame frame; // Defining a CAN frame called frame
    memset(&frame, 0, sizeof(frame)); // Zeros out the frame to ensure that all bits starts in a knwown state
        
    frame.flags = CAN_FRAME_IDE; // Use extended identifier
    frame.id = MSG_ID; // Using the message ID for the frame that is defined in the header file
    frame.dlc = 5; // Defines the length of the byte in the frame to 5

    frame.data[0] = js_bool; // Places the boolean value of the joystick used into the first byte of the data field
    memcpy(&frame.data[1], &js_deg_int, sizeof(js_deg_int)); // Copies the 2 bytes of the joystick-degree integer, starting at the second byte
    memcpy(&frame.data[3], &js_spd_int, sizeof(js_spd_int)); // Copies the 2 bytes of the joystick-speed integer, starting at the fourth byte

    if (can_send(can_dev, &frame, K_SECONDS(10), NULL, NULL) != 0) { // If test to check if the CAN frame is sent or not
            printk("Failed to send frame\n");
        } /*else {
            printk("Frame sent\n");
        }*/
}