#ifndef CANBUS_H
#define CANBUS_H
#include <zephyr/drivers/can.h>
// Commented and coded by OA

void canBegin(const struct device *can_dev); // Function for initializing and starting the CAN device

void canState(const struct device *can_dev); // Function for checking the CAN state

void process_can_data(void); // Function for processing the receievd CAN data 

//can_dev functions
const struct device *const get_can_dev(void); // Function for getting the CAN device

//can_receive functions
#define RECEIVE_ID 0x0000001 // RECEIVE ID
#define CAN_MSGQ_SIZE 10 // Message queue size

//RAS(PROWLTECH) laget ny struct for å ta imot verdier på vår måte, fart, vinkel og rotasjon
struct can_ret_data {  //struct for å motta data
    float vinkel;
    float fart;
    float rotasjon;

};


extern struct k_msgq can_msgq; // Message queue structure

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data); // Function for setting up the can filter, taking in the CAN device, callback function and cb data

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data); // Function for checking CAN frames on the bus


//can_send functions
#define MSG_ID 0x0000002 // Message ID 

const char* can_state_to_str(enum can_state state); // Function that converts the CAN states to string
void send_string(const struct device *can_dev, const char *str); // Function for sending a string over the bus


#endif