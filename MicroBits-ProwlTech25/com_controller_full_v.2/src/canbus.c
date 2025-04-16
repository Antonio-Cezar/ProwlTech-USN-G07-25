#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/device.h>
#include "canbus.h"
#include "tone.h"

static uint8_t received_byte;

void canBegin(void) {
    const struct device *const can_dev = get_can_dev();

    if (!can_dev) {
        printk("Cannot find CAN device!\n");
        return;
    }

    if (can_start(can_dev) != 0) {
        printk("Failed to start CAN controller\n");
        return;
    }

    if (setup_can_filter(can_dev, can_rx_callback, NULL) < 0) {
        printk("Failed to add CAN filters\n");
    }
}

const struct device *const get_can_dev(void) {
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
}

uint8_t get_sensor_byte(void) {
    return received_byte;
}

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data) {
    struct can_filter filter_byte = {
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE,
        .id = RECEIVE_ID_BYTE,
        .mask = CAN_EXT_ID_MASK
    };

    struct can_filter filter_tone = {
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE,
        .id = RECEIVE_ID_TONE,
        .mask = CAN_EXT_ID_MASK
    };

    int id_byte = can_add_rx_filter(dev, rx_cb, cb_data, &filter_byte);
    int id_tone = can_add_rx_filter(dev, rx_cb, cb_data, &filter_tone);

    return (id_byte >= 0 && id_tone >= 0) ? 0 : -1;
}

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) {
    if (frame->id == RECEIVE_ID_BYTE && frame->dlc == 1) {
        received_byte = frame->data[0];

    } else if (frame->id == RECEIVE_ID_TONE && frame->dlc == 1) {
        uint8_t tone_value = frame->data[0];
        printk("[CAN] Mottatt tone-kommando: 0x%02X\n", tone_value);

        if (tone_value) {
            printk("[CAN] → Spiller tone\n");
            play_tone();
        } else {
            printk("[CAN] → Stopper tone\n");
            stop_tone();
        }

    } else {
        printk("Unexpected CAN frame: ID=0x%X, DLC=%d\n", frame->id, frame->dlc);
    }
}

