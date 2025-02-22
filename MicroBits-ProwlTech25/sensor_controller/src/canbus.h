#ifndef __CANBUS_H
#define __CANBUS_H
#include <zephyr/drivers/can.h>

// Commented and coded by OA - Fenri24

//can_dev
const struct device *const get_can_dev(void); // Function for getting the CAN device - Fenri24

//can_receive
#define RECEIVE_ID_STR 0x0000002 // RECEIVE ID matching the message ID of the motor frame - Fenri24

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data); // Function for setting up the CAN filter - Fenri24

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data); // Function for looking at CAN frames on the bus, and retreiving them - Fenri24

// can_send - Fenri24
#define MSG_ID 0x0000003 // Message ID - Fenri24

const char* can_state_to_str(enum can_state state);
void send_byte(const struct device *can_dev, uint8_t sensor_byte);
#endif