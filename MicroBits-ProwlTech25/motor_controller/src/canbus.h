#ifndef CANBUS_H
#define CANBUS_H
#include <zephyr/drivers/can.h>
// Commented and coded by OA

void canBegin(const struct device *can_dev); // Function for initializing and starting the CAN device - Fenri24
 
void canState(const struct device *can_dev); // Function for checking the CAN state - Fenri24

void process_can_data(void); // Function for processing the receievd CAN data - Fenri24

//can_dev functions
const struct device *const get_can_dev(void); // Function for getting the CAN device - Fenri24

//can_receive functions
#define RECEIVE_ID 0x0000001 // RECEIVE ID - Fenri24
#define CAN_MSGQ_SIZE 10 // Message queue size - Fenri24

struct can_ret_data { // Struct for the received data - Fenri24
    bool js_bool_data;     // Converted from uint8_t to bool - Fenri24
    int js_deg_int_data;   // Converted from int16_t to int - Fenri24
    int js_spd_int_data;   // Converted from int16_t to int - Fenri24
};

extern struct k_msgq can_msgq; // Message queue structure - Fenri24

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data); // Function for setting up the can filter, taking in the CAN device, callback function and cb data - Fenri24

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data); // Function for checking CAN frames on the bus - Fenri24


//can_send functions
#define MSG_ID 0x0000002 // Message ID - Fenri24

const char* can_state_to_str(enum can_state state); // Function that converts the CAN states to string - Fenri24
void send_string(const struct device *can_dev, const char *str); // Function for sending a string over the bus - Fenri24


#endif