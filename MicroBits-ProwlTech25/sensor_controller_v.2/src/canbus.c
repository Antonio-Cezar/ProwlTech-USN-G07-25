#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <string.h>
#include "canbus.h"

const struct device *const get_can_dev(void) {
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
}

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data) {
    struct can_filter filter = {
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE,
        .id = RECEIVE_ID_STR,
        .mask = CAN_EXT_ID_MASK
    };
    return can_add_rx_filter(dev, rx_cb, cb_data, &filter);
}

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) {
    char received_str[9];
    if (frame->dlc <= 8) {
        memcpy(received_str, frame->data, frame->dlc);
        received_str[frame->dlc] = '\0';
        // printk("Mottatt: %s\n", received_str);
    }
}
