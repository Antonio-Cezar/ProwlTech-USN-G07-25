#include <zephyr/kernel.h>             // Inkluderer Zephyr sitt kjernesystem
#include <zephyr/drivers/can.h>        // Inkluderer støtte for CAN-kommunikasjon
#include <zephyr/device.h>             // For å hente tilgang til enheter
#include "canbus.h"                    // Egen header for CAN-definisjoner.
#include "tone.h"                      // Funksjoner for å spille og stoppe tone

// (PROWLTECH25 - CA)

// Variabel for å holde på sist mottatte byte fra CAN
static uint8_t received_byte;

//--------------------------------------------------------------
// Initialiserer og starter CAN-kommunikasjon
//--------------------------------------------------------------
void canBegin(void) {
    const struct device *const can_dev = get_can_dev(); // Hent peker til CAN-enhet

    if (!can_dev) {
        printk("Cannot find CAN device!\n");
        return;
    }

    if (can_start(can_dev) != 0) {
        printk("Failed to start CAN controller\n");
        return;
    }

    enum can_state state;
    can_get_state(can_dev, &state, NULL);
    printk("CAN-bus status etter start: %s\n", can_state_to_str(state));

    if (setup_can_filter(can_dev, can_rx_callback, NULL) < 0) {
        printk("Failed to add CAN filters\n");
    }
}

const char* can_state_to_str(enum can_state state) {
    switch (state) {
        case CAN_STATE_ERROR_ACTIVE: return "ERROR_ACTIVE";
        case CAN_STATE_ERROR_PASSIVE: return "ERROR_PASSIVE";
        case CAN_STATE_BUS_OFF: return "BUS_OFF";
        case CAN_STATE_STOPPED: return "STOPPED";
        default: return "UNKNOWN";
    }
}


//--------------------------------------------------------------
// Returnerer peker til riktig CAN-enhet definert i device tree
//--------------------------------------------------------------
const struct device *const get_can_dev(void) {
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
}

//--------------------------------------------------------------
// Returnerer siste mottatte byte (brukes av annen logikk)
//--------------------------------------------------------------
uint8_t get_sensor_byte(void) {
    return received_byte;
}

//--------------------------------------------------------------
// Legger til CAN-filtre for IDene vi ønsker å ta imot
//--------------------------------------------------------------
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


//--------------------------------------------------------------
// Callback-funksjon når CAN-melding mottas
//--------------------------------------------------------------
void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) {
    printk("[CAN] Mottok melding: ID=0x%X, DLC=%d\n", frame->id, frame->dlc);
    for (int i = 0; i < frame->dlc; i++) {
        printk("  Data[%d] = 0x%02X\n", i, frame->data[i]);
    }

    if (frame->id == RECEIVE_ID_BYTE && frame->dlc == 1) {
        received_byte = frame->data[0];

    } else if (frame->id == RECEIVE_ID_TONE && frame->dlc == 1) {
        uint8_t tone_value = frame->data[0];

        if (tone_value) {
            printk("[CAN] → Spiller tone\n");
            play_tone(); // Aktiver tone-funksjon
        } else {
            printk("[CAN] → Stopper tone\n");
            stop_tone(); // Deaktiver tone-funksjon
        }
    } 
    
    // Uventet melding
    else {
        printk("Unexpected CAN frame: ID=0x%X, DLC=%d\n", frame->id, frame->dlc);
    }
}

