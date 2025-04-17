#define M_PI 3.14159265358979323846
#include <math.h>
#include <stdio.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <stdbool.h>
#include <string.h>

#include "canbus.h"
#include "motorstyring.h"

// Global variabel for siste tidspunkt vi mottok en melding
static int64_t siste_mottatt_tid = 0;

// Meldingskø for CAN-data
K_MSGQ_DEFINE(can_msgq, sizeof(struct can_ret_data), CAN_MSGQ_SIZE, 4);

// Hent CAN-enhet fra devicetree
const struct device *const get_can_dev(void) {
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
}

// Initialiserer og starter CAN
void canBegin(const struct device *can_dev) {
    if (!can_dev) {
        printf("Fant ikke CAN-enhet!\n");
        return;
    }

    int ret = can_start(can_dev);
    if (ret != 0) {
        printf("Feil ved oppstart av CAN: %d\n", ret);
        return;
    }

    if (setup_can_filter(can_dev, can_rx_callback, NULL) < 0) {
        printf("Feil: klarte ikke å legge til CAN-filter\n");
        return;
    }

    printf("CAN er startet og filter er satt opp\n");
}

// Returner CAN-state som tekst
const char* can_state_to_str(enum can_state state) {
    switch (state) {
        case CAN_STATE_ERROR_ACTIVE: return "ERROR ACTIVE";
        case CAN_STATE_ERROR_WARNING: return "ERROR WARNING";
        case CAN_STATE_ERROR_PASSIVE: return "ERROR PASSIVE";
        case CAN_STATE_BUS_OFF: return "BUS OFF";
        case CAN_STATE_STOPPED: return "STOPPED";
        default: return "UKJENT";
    }
}

// Skriv ut CAN status
void canState(const struct device *can_dev) {
    enum can_state state;
    struct can_bus_err_cnt err_cnt;

    if (can_get_state(can_dev, &state, &err_cnt) != 0) {
        printf("Kunne ikke hente CAN-status\n");
    } else {
        printf("CAN-status: %s\n", can_state_to_str(state));
        printf("TX-feil: %d, RX-feil: %d\n", err_cnt.tx_err_cnt, err_cnt.rx_err_cnt);
    }
}

// Oppsett av CAN-filter
int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data) {
    struct can_filter filter = {
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE,
        .id = RECEIVE_ID,
        .mask = CAN_EXT_ID_MASK
    };
    return can_add_rx_filter(dev, rx_cb, cb_data, &filter);
}

// Callback når en CAN-melding mottas
void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data) {
    // Sjekk at vi faktisk fikk 8 byte som forventet
    if (frame->id == RECEIVE_ID && frame->dlc == 8) {

        siste_mottatt_tid = k_uptime_get();
        sett_nødstopp(false);  

        // Debug: print rå data
        printf("RÅ CAN-data (hex): ");
        for (int i = 0; i < frame->dlc; i++) {
            printf("%02X ", frame->data[i]);
        }
        printf("\n");

        // Lese inn 4x int16_t fra frame->data, liten endian
        int16_t fart_i      = sys_le16_to_cpu(*(int16_t *)&frame->data[0]);
        int16_t vinkel_i    = sys_le16_to_cpu(*(int16_t *)&frame->data[2]);
        int16_t rotasjon_i  = sys_le16_to_cpu(*(int16_t *)&frame->data[4]);
        int16_t sving_js_i  = sys_le16_to_cpu(*(int16_t *)&frame->data[6]);

        float fart      = fart_i / 100.0f;
        float vinkel    = vinkel_i / 100.0f;
        float rotasjon  = rotasjon_i / 100.0f;
        float sving_js  = sving_js_i / 100.0f;

        printf("[PI → Motor-MB] Mottatt:\n");
        printf("  Fart      : %.2f\n", fart);
        printf("  Vinkel    : %.2f°\n", (vinkel * 180.0f / (float)M_PI));
        printf("  Rotasjon  : %.2f\n", rotasjon);
        printf("  Sving JS  : %.2f\n", sving_js);
        printf("-----------------------------\n");

        kontroller_motorene(fart, vinkel, rotasjon); // legg evt. til sving_js

    } else {
        printf("CAN frame ignorert: ID 0x%X, DLC %d\n", frame->id, frame->dlc);
    }
}

// Henter melding fra køen og styrer motorene
void process_can_data(void) {
    struct can_ret_data data;
    if (k_msgq_get(&can_msgq, &data, K_NO_WAIT) == 0) {
        kontroller_motorene(data.fart, data.vinkel, data.rotasjon);
    }

    // Sjekk om det er lenge siden siste melding → aktiver nødstopp
    //int64_t nåtid = k_uptime_get();
    //if ((nåtid - siste_mottatt_tid) > 1000) {
        //if (!er_nødstopp_aktivert()) {
            //printf("Advarsel: Ingen meldinger på over 1 sekund → nødstopp aktivert\n");
            //sett_nødstopp(true);
        //}
    //}
}

// Send en streng over CAN (brukes til testing)
void send_string(const struct device *can_dev, const char *str) {
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.flags = CAN_FRAME_IDE;
    frame.id = MSG_ID;
    frame.dlc = strlen(str);
    memcpy(frame.data, str, frame.dlc);

    if (can_send(can_dev, &frame, K_MSEC(100), NULL, NULL) != 0) {
        printf("Feil: Klarte ikke å sende melding på CAN\n");
    }
}
