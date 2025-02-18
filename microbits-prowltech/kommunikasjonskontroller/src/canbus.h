#ifndef __CANBUS_H
#define __CANBUS_H
#include <zephyr/drivers/can.h>

//can.dev
const struct device *const get_can_dev(void);

//can_receive
#define RECEIVE_ID_STR 0x0000002 // RECEIVE ID for motor controller
#define RECEIVE_ID_BYTE 0x0000003 // RECEIVE ID for sensor 

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data);

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data);
uint8_t get_sensor_byte();
//can_send
#define MSG_ID 0x0000001 // Message ID

const char* can_state_to_str(enum can_state state);
void send_custom_data (const struct device *can_dev, uint8_t js_bool, int16_t js_deg_int, int16_t js_spd_int);

void canBegin(void);
#endif