#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include "canbus.h"
#include <stdio.h>
#include "tone.h"
//(FENRIS24) Commented and coded by OA

uint8_t received_byte;

void canBegin(void){ //(FENRIS24) Function for startring up and initializing the CAN device
    const struct device *const can_dev = get_can_dev(); //(FENRIS24) Getting the CAN device connected to the system
    
    if (!can_dev) { //(FENRIS24) Test that checks is the CAN device is found. If not it will print the error message and return
        printk("Cannot find CAN device!\n");
        return;
    }

    int ret = can_start(can_dev); //(FENRIS24) Starting up the CAN device
    if (ret != 0) { //(FENRIS24) Check to see if the CAN device has started, if not it will print the error message and return
        printf("Failed to start CAN controller: Error %d\n", ret);
        return;
    }
    if (setup_can_filter(can_dev, can_rx_callback, NULL) < 0) { //(FENRIS24) Check to see if the CAN filter is set up or not
        printk("Failed to add CAN filter\n");
        return;
    }
}

uint8_t get_sensor_byte(){ //(FENRIS24) Function that allows other parts of the program to access the received sensor byte
    return received_byte;
}

const struct device *const get_can_dev(void){ //(FENRIS24) Fuction responsible for finding and accessing the CAN device
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus)); //(FENRIS24) Returns the CAN device by looking for it in the devicetree/ overlay file under "chosen"
} 



int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data){ //(FENRIS24) Function for setting up the can filter, taking in the CAN device, callback function and cb data
    struct can_filter filter_str = { //(FENRIS24) CAN filter for a string
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE, //(FENRIS24) Setting the flags for the filter, where the filter matches the data frames, and that the filter matches CAN frames with extended ID (29-bit)
	    .id = RECEIVE_ID_STR, //(FENRIS24) Filter ID set to match the sender ID of the string, defined in the header file.
	    .mask = CAN_EXT_ID_MASK //(FENRIS24) Filter bit mask for an extended 29-bit CAN ID
    };

    struct can_filter filter_byte = { //(FENRIS24) CAN filter for a byte
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE, //(FENRIS24) Setting the flags for the filter, where the filter matches the data frames, and that the filter matches CAN frames with extended ID (29-bit)
	    .id = RECEIVE_ID_BYTE, //(FENRIS24) Filter ID set to match the sender ID of the byte, defined in the header file.
	    .mask = CAN_EXT_ID_MASK //(FENRIS24) Filter bit mask for an extended 29-bit CAN ID
    };

    struct can_filter filter_pi = { //(PROWLTECH25) CAN filter for PI
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE,
        .id = RECEIVE_ID_PI,
        .mask = CAN_EXT_ID_MASK
    };

    struct can_filter filter_tone = { //(PROWLTECH25) fiktrere for tone cezar husk å kommenter
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE,
        .id = 0x0000005,  // Ny ID for tone
        .mask = CAN_EXT_ID_MASK
    };
    
    int filter_id_str = can_add_rx_filter(dev, rx_cb, cb_data, &filter_str); //(FENRIS24) Setting the filter for the string as a variable
    int filter_id_byte = can_add_rx_filter(dev, rx_cb, cb_data, &filter_byte); //(FENRIS24) Setting the filter for the byte as a variable
    int filter_id_pi = can_add_rx_filter(dev, rx_cb, cb_data, &filter_pi); //(PROWLTECH25) filter for kontroller values from PI 
    int filter_id_tone = can_add_rx_filter(dev, rx_cb, cb_data, &filter_tone); //(PROWLTECH25) Cezar husk å kommenter...

    return (filter_id_str >= 0 && filter_id_byte >= 0 && filter_id_pi >= 0) ? 0 : -1;  //(FENRIS24) Return 0 if both filters are added successfully //(PROWLTECH25) Pi also return to 0
}

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) { //(FENRIS24) Function for checking CAN frames on the bus
    if (frame->id == RECEIVE_ID_STR && frame->dlc <= 8) { //(FENRIS24) If test for checking if the frame-ID matches the receive ID for the string frame, and if the size of the frame is equal to or less than 8 byte
        char received_str[9]; //(FENRIS24) Character variable to put the string in
        memcpy(received_str, frame->data, frame->dlc); //(FENRIS24) Copies the data from the received CAN frame into received_str buffer
        received_str[frame->dlc] = '\0'; //(FENRIS24) NULL terminates it for safe processing
        //(FENRIS24) printk("Received string from ID 0x%X: %s\n", frame->id, received_str);
    } 
    //(FENRIS24) Checking if the frame on the BUS has the ID and frame data size as the one sent from the Sensor node
    else if (frame->id == RECEIVE_ID_BYTE && frame->dlc == 1) { 
        received_byte = frame->data[0];
        //(FENRIS24) printk("Received byte from ID 0x%X: %02X\n", frame->id, received_byte);
    } 
    //(PROWLTECH25) Sjekker fis frame fra BUS har den samme ID og frame data som blir sendt
    else if (frame->id == RECEIVE_ID_PI) {
        handle_pi_controller_data(frame); 
    }
    //(PROWLTECH25) Cezar husk å kommenter....
    else if (frame->id == 0x0000005 && frame->dlc == 1) {
        uint8_t play_tone = frame->data[0];
        if (play_tone) {
            play_tone();
        } else {
            stop_tone();
        }
    }    
    else {
        printk("Unexpected data format or ID\n");
    }
}

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

void send_custom_data (const struct device *can_dev, uint8_t js_bool, int16_t js_deg_int, int16_t js_spd_int){ //(FENRIS24) Function for sending joystick data over the bus
    struct can_frame frame; //(FENRIS24) Defining a CAN frame called frame
    memset(&frame, 0, sizeof(frame)); //(FENRIS24) Zeros out the frame to ensure that all bits starts in a knwown state
        
    frame.flags = CAN_FRAME_IDE; //(FENRIS24) Use extended identifier
    frame.id = MSG_ID; //(FENRIS24) Using the message ID for the frame that is defined in the header file
    frame.dlc = 5; //(FENRIS24) Defines the length of the byte in the frame to 5

    frame.data[0] = js_bool; //(FENRIS24) Places the boolean value of the joystick used into the first byte of the data field
    memcpy(&frame.data[1], &js_deg_int, sizeof(js_deg_int)); //(FENRIS24) Copies the 2 bytes of the joystick-degree integer, starting at the second byte
    memcpy(&frame.data[3], &js_spd_int, sizeof(js_spd_int)); //(FENRIS24) Copies the 2 bytes of the joystick-speed integer, starting at the fourth byte

    if (can_send(can_dev, &frame, K_SECONDS(10), NULL, NULL) != 0) { //(FENRIS24) If test to check if the CAN frame is sent or not
            printk("Failed to send frame\n");
        } /*else {
            printk("Frame sent\n");
        }*/
}

void get_pi_controller_data(struct can_frame *frame) {
    if (frame->dlc != 8) {
        printk("Feil: Forventet 8 byte fra Pi, fikk %d\n", frame->dlc);
        return;
    }

    uint8_t knapper = frame->data[0];
    uint8_t status = frame->data[1];
    int16_t venstre_y, høyre_y;
    uint8_t lt = frame->data[6];
    uint8_t rt = frame->data[7];

    memcpy(&venstre_y, &frame->data[2], sizeof(int16_t));
    memcpy(&høyre_y, &frame->data[4], sizeof(int16_t));

    // Print or handle
    printk("[PI] Knapper: 0x%02X, Status: 0x%02X\n", knapper, status);
    printk("[PI] Venstre Y: %d, Høyre Y: %d, LT: %u, RT: %u\n", venstre_y, høyre_y, lt, rt);

    // TODO: Use values to control motors, LEDs, etc.
}
