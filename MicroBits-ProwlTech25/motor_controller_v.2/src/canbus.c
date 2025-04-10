#define M_PI 3.14159265358979323846
#include <math.h>
#include <stdio.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <stdbool.h>
#include <string.h>
#include "canbus.h"
#include "motorkontroller.h"
#include "motorstyring.h" 

 //lagt til av RAS (PROWLTECH25)
//Motorstyringen i denne filen er endret av RAS(PROWLTECH25)
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
        printf("CAN State: %s\n", can_state_to_str(state));
        printf("TX Errors: %d, RX Errors: %d\n", err_cnt.tx_err_cnt, err_cnt.rx_err_cnt);
        }
}

//RAS(PROWLTECH25) Laget ny versjon for å håndtere ny data
void process_can_data(void) {
    struct can_ret_data received_data;
    if (k_msgq_get(&can_msgq, &received_data, K_FOREVER) == 0) {
        printk("Prosesserer CAN data: Fart=%.2f, Vinkel=%.2f, Rotasjon=%.2f\n", received_data.fart, received_data.vinkel, received_data.rotasjon);
        kontroller_motorene(received_data.fart, received_data.vinkel, received_data.rotasjon);
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

// cezar husk å kommentere og endre
void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) {
    if (frame->id == RECEIVE_ID && frame->dlc == 6) {
        // Korrekt endian-konvertering
        int16_t fart_i     = sys_le16_to_cpu(*(int16_t *)&frame->data[0]);
        int16_t vinkel_i   = sys_le16_to_cpu(*(int16_t *)&frame->data[2]);
        int16_t rotasjon_i = sys_le16_to_cpu(*(int16_t *)&frame->data[4]);

        // Konverter til float
        float fart     = fart_i / 100.0f;
        float vinkel   = vinkel_i / 100.0f;
        float rotasjon = rotasjon_i / 100.0f;

        // Print verdier
        printf("[PI → Motor-MB] Mottatt:\n");
        printf("  Fart     : %.2f\n", fart);
        printf("  Vinkel   : %.2f\n", (vinkel * 180.0f / (float)M_PI));
        printf("  Rotasjon : %.2f\n", rotasjon);
        printf("-----------------------------\n");

        // Velg joystick-modus ut ifra rotasjon
        bool joystick_mode = (rotasjon == 0.0f);

        // Klargjør for motorstyring
        int vinkel_deg = (int)(vinkel * 180.0f / 3.14159f);
        int fart_int = (int)(fart * 100);

        control_motors(vinkel_deg, fart_int, joystick_mode);
    } else {
        printf("CAN frame ignorert: ID 0x%X, DLC %d\n", frame->id, frame->dlc);
    }
}

