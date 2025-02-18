#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <stdbool.h>
#include <string.h>
#include "canbus.h"
#include "motor_controller.h"
// Commented and coded by OA



void canBegin(const struct device *can_dev){ // Function for startring up and initializing the CAN device
    //const struct device *const can_dev = get_can_dev(); // Getting the CAN device connected to the system
    
    if (!can_dev) { // Test that checks if the CAN device is found. If not it will print the error message and return
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

void canState(const struct device *can_dev){ // Function for checking the CAN state
    enum can_state state; // Defines the state of the CAN controller
    struct can_bus_err_cnt err_cnt; // CAN controller error counter
    if (can_get_state(can_dev, &state, &err_cnt) != 0) { // If test to check the state of the CAN controller
            printk("Failed to get CAN state\n");
        } else {
        //printf("CAN State: %s\n", can_state_to_str(state));
        //printf("TX Errors: %d, RX Errors: %d\n", err_cnt.tx_err_cnt, err_cnt.rx_err_cnt);
        }
}

void process_can_data(void) { // Function for processing the receievd CAN data 
    struct can_ret_data received_data; // structure for received CAN data
    if (k_msgq_get(&can_msgq, &received_data, K_FOREVER) == 0) { // If test that receives a message from the message queue, and if it is zero (successfull), it will then use said data
        //printk("Processing CAN data: Bool=%d, Degree=%d, Speed=%d\n",
        received_data.js_bool_data,
        received_data.js_deg_int_data,
        received_data.js_spd_int_data;
        control_motors(received_data.js_deg_int_data, received_data.js_spd_int_data, received_data.js_bool_data); // Sends the received joystick data into the control_motors function
    }
}

//can_dev functions
const struct device *const get_can_dev(void){ // Fuction responsible for finding and accessing the CAN device
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus)); // Returns the CAN device by looking for it in the devicetree/ overlay file under "chosen"
} 

//can_receive functions
K_MSGQ_DEFINE(can_msgq, sizeof(struct can_ret_data), CAN_MSGQ_SIZE, 4); // Message queue function for putting the joystick values received from the com controller node in

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data){ // Function for setting up the can filter, taking in the CAN device, callback function and cb data
    struct can_filter filter = { // CAN filter for the joystick values
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE, // Setting the flags for the filter, where the filter matches the data frames, and that the filter matches CAN frames with extended ID (29-bit)
	    .id = RECEIVE_ID, // Filter ID set to match the sender ID of the joystick values, defined in the header file
	    .mask = CAN_EXT_ID_MASK // Filter bit mask for an extended 29-bit CAN ID
    };
    return can_add_rx_filter(dev, rx_cb, cb_data, &filter); // Returning the rx filter
}

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) { // Function for checking CAN frames on the bus
    //printk("Received frame with ID: 0x%x\n", frame->id);
    if (frame->id == RECEIVE_ID && frame->dlc == 5) { // If test for checking if the frame-ID matches the receive ID for the joystick frame, and if the size of the frame is equal to 5 byte in size
        struct can_ret_data js_data; // Struct for the joystick data, set up in the header file
        
        js_data.js_bool_data = (frame->data[0] != 0);  // Puts the joystick data into the variable if the value in the first position is not NULL

        int16_t js_deg_int, js_spd_int; // Convert int16_t to int 
        memcpy(&js_deg_int, &frame->data[1], sizeof(js_deg_int)); // Copies the data from the received CAN frame in position 1 to the js_deg_int variable
        memcpy(&js_spd_int, &frame->data[3], sizeof(js_spd_int)); // Copies the data from the received CAN frame in position 3 to the js_spd_int variable
        
        js_data.js_deg_int_data = js_deg_int;  // Automatically turnes the received variable into full int
        js_data.js_spd_int_data = js_spd_int;  // Automatically turnes the received variable into full int

        printk("Received data: js_bool=%d, js_deg=%d, js_spd=%d\n",
               js_data.js_bool_data, js_data.js_deg_int_data, js_data.js_spd_int_data); // Printing the received joystick data to the Serial Monitor

        if (k_msgq_put(&can_msgq, &js_data, K_NO_WAIT) != 0) {
            printk("Failed to put data in queue\n"); // If test that checks if there is an error when putting the joystick values into the message queue
        }
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

void send_string(const struct device *can_dev, const char *str){ // Function for sending a string over the bus
    struct can_frame frame; // Using a CAN frame struct called frame
    memset(&frame, 0, sizeof(frame)); // Zeros out the frame to ensure that all bits starts in a knwown state

    frame.flags = CAN_FRAME_IDE; // Use extended identifier
    frame.id = MSG_ID; // Using the message ID for the frame that is defined in the header file
    frame.dlc = strlen(str);  // Set the DLC to the string length
    memcpy(frame.data, str, frame.dlc);  // Copies the string to the CAN frame data field

    if (can_send(can_dev, &frame, K_SECONDS(10), NULL, NULL) != 0) { // If test that sends the frame on the bus, and prints out a warning if it failes
            printk("Failed to send frame\n");
        } /*else {
            printk("Frame sent\n");
        }*/
}

