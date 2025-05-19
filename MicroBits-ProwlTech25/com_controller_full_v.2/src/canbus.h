#ifndef __CANBUS_H
#define __CANBUS_H

#include <zephyr/drivers/can.h>

#define RECEIVE_ID_BYTE 0x0000003  // Sensor node
#define RECEIVE_ID_TONE 0x0000020  // Tone kommando fra Pi

const struct device *const get_can_dev(void);
uint8_t get_sensor_byte(void);
int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data);
void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data);
void canBegin(void);
const char* can_state_to_str(enum can_state state);


#endif
