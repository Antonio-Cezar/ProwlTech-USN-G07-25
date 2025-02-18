#ifndef CANBUS_H
#define CANBUS_H
#include <zephyr/drivers/can.h>
//can_dev functions
const struct device *const get_can_dev(void);

//can_receive functions
#define RECEIVE_ID 0x0000002 // RECEIVE ID

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data);

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data);


//can_send functions
#define MSG_ID 0x0000003 // Message ID

const char* can_state_to_str(enum can_state state);
void send_byte(const struct device *can_dev, uint8_t sensor_byte);

#endif