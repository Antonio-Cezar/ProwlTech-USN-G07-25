#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <stdbool.h>
#include <string.h>
#include "canbus.h"

struct can_ret_data drive_command;
enum can_state state;
struct can_bus_err_cnt err_cnt;

//Can_dev functions
const struct device *const get_can_dev(void){
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
}
struct can_ret_data get_drive_command(){
    return drive_command;
}
enum can_state get_can_state(){
    return state;
}
struct can_bus_err_cnt get_can_err_cnt(){
    return err_cnt;
}


//can_dev initialization
void can_begin(void){
    const struct device *const can_dev = get_can_dev();
    if (!can_dev) {
        printk("Cannot find CAN device!\n");
        return;
    }
    if (setup_can_filter(can_dev, can_rx_callback, NULL) < 0) {
        printk("Failed to add CAN filter\n");
        return;
    }
    int ret = can_start(can_dev);
    if (ret != 0) {
        printf("Failed to start CAN controller: Error %d\n", ret);
        return;
    }
}

//Can_receive functions
K_MSGQ_DEFINE(can_msgq, sizeof(struct can_ret_data), CAN_MSGQ_SIZE, 4);

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data){
    struct can_filter filter = {
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE,
	    .id = RECEIVE_ID,
	    .mask = CAN_EXT_ID_MASK
    };
    return can_add_rx_filter(dev, rx_cb, cb_data, &filter);
}

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) {
    printk("Received frame with ID: 0x%x\n", frame->id);
    if (frame->dlc == 5) { // Expecting 5 byte of data
        struct can_ret_data js_data;
        
         js_data.js_bool_data = (frame->data[0] != 0);  // True if non-zero, false otherwise

        // Convert int16_t to int (assuming the system is little-endian or matching endianness)
        int16_t js_deg_int, js_spd_int;
        memcpy(&js_deg_int, &frame->data[1], sizeof(js_deg_int));
        memcpy(&js_spd_int, &frame->data[3], sizeof(js_spd_int));
        
        js_data.js_deg_int_data = js_deg_int;  // Automatic promotion to int
        js_data.js_spd_int_data = js_spd_int;  // Automatic promotion to int

        drive_command.js_bool_data = js_data.js_bool_data;
        drive_command.js_deg_int_data = js_data.js_deg_int_data;
        drive_command.js_spd_int_data = js_data.js_spd_int_data;

        printk("Received data: js_bool=%d, js_deg=%d, js_spd=%d\n",
               js_data.js_bool_data, js_data.js_deg_int_data, js_data.js_spd_int_data);

        if (k_msgq_put(&can_msgq, &js_data, K_NO_WAIT) != 0) {
            printk("Failed to put data in queue\n");
        }
    }
}


//Can_send functions
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

void send_string(const struct device *can_dev, const char *str){
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));

    frame.flags = CAN_FRAME_IDE; // Use extended identifier
    frame.id = MSG_ID;
    frame.dlc = strlen(str);  // Set the DLC to the string length
    memcpy(frame.data, str, frame.dlc);  // Copy string to CAN frame data field

    if (can_send(can_dev, &frame, K_SECONDS(10), NULL, NULL) != 0) {
            printk("Failed to send frame\n");
        } /*else {
            printk("Frame sent\n");
        }*/
}