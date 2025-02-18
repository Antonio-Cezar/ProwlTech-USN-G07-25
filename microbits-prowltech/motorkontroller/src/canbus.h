#ifndef CANBUS_H
#define CANBUS_H
#include <zephyr/drivers/can.h>


//can_dev functions
const struct device *const get_can_dev(void);
struct can_ret_data get_drive_command();
enum can_state get_can_state();
struct can_bus_err_cnt get_can_err_cnt();
//can_initialization
void can_begin(void);

//can_receive functions
#define RECEIVE_ID 0x0000001 // RECEIVE ID
#define CAN_MSGQ_SIZE 10

struct can_ret_data {
    bool js_bool_data;     // Converted from uint8_t to bool
    int js_deg_int_data;   // Converted from int16_t to int
    int js_spd_int_data;   // Converted from int16_t to int
};

extern struct k_msgq can_msgq;

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data);

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data);

//Can_send functions
#define MSG_ID 0x0000002 // Message ID

const char* can_state_to_str(enum can_state state);
void send_string(const struct device *can_dev, const char *str);

#endif