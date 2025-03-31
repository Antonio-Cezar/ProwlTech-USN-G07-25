#ifndef __CANBUS_H
#define __CANBUS_H
#include <zephyr/drivers/can.h>

//(FENRIS24) Commented and coded by OA

//(FENRIS24) can_dev
const struct device *const get_can_dev(void); // Function for getting the CAN device

//(FENRIS24) can_receive
#define RECEIVE_ID_STR 0x0000002 //(FENRIS24) RECEIVE ID matching the message ID of the motor frame
#define RECEIVE_ID_BYTE 0x0000003 //(FENRIS24) RECEIVE ID matching the message ID of the sensor frame
#define RECEIVE_ID_STR 0x0000004 //(PROWLTECH25) Receive id matching the message id for the Raspberry Pi 4 (GUI) 

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data); //(FENRIS24) Function for setting up the CAN filter

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data); // Function for looking at CAN frames on the bus, and retreiving them
uint8_t get_sensor_byte(); //(FENRIS24) Function for accessing the sensor byte sent over the bus from the sensor node
//can_send
#define MSG_ID 0x0000001 //(FENRIS24) Message ID of the frame

const char* can_state_to_str(enum can_state state); // Function that converts the CAN states to string
void send_custom_data (const struct device *can_dev, uint8_t js_bool, int16_t js_deg_int, int16_t js_spd_int); // Function for sending a CAN frame with joystick data 

void canBegin(void); //(FENRIS24) Function for initializing and starting the CAN device
#endif