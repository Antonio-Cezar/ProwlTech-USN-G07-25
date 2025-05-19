#include "canbus.h"
#include "motorstyring.h"

#include <zephyr/sys/byteorder.h>  // sys_le16_to_cpu()
#include <zephyr/kernel.h>         // K_MSGQ_DEFINE, k_uptime_get()
#include <zephyr/drivers/can.h>    // CAN‐API
#include <stdio.h>
#include <string.h>
#include <math.h>

// Lager en meldingskø for inntil CAN_MSGQ_SIZE meldinger
K_MSGQ_DEFINE(can_msgq,
              sizeof(struct can_ret_data),
              CAN_MSGQ_SIZE, 4);

// Henter CAN‐enhet fra devicetree alias "zephyr,canbus"
const struct device *const get_can_dev(void) {
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));
}

static int64_t last_can_rx_time = 0;

int64_t get_last_can_rx_time(void) {
    return last_can_rx_time;
}

// Starter CAN‐hardware og legger til filter/callback
void canBegin(const struct device *can_dev) {
    if (!can_dev) {
        printf("Fant ikke CAN-enhet!\n");
        return;
    }
    // Prøver å starte CAN‐driveren
    if (can_start(can_dev)) {
        printf("Feil ved oppstart av CAN\n");
        return;
    }
    // Legger på filter som kun aksepterer meldinger med RECEIVE_ID
    if (setup_can_filter(can_dev, can_rx_callback, NULL) < 0) {
        printf("Klarte ikke å legge til CAN-filter\n");
        return;
    }
    printf("CAN startet og filter OK\n");
}

// Setter opp filter: ID = RECEIVE_ID, standard format
int setup_can_filter(const struct device *dev,
                     can_rx_callback_t rx_cb,
                     void *cb_data) {
    struct can_filter filter = {
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE,
        .id    = RECEIVE_ID,
        .mask  = CAN_EXT_ID_MASK,
    };
    return can_add_rx_filter(dev, rx_cb, cb_data, &filter);
}

// Callback for hver mottatt CAN‐frame
void can_rx_callback(const struct device *dev,
                     struct can_frame *frame,
                     void *user_data) {
    // Sjekk at ID og data‐lengde er korrekt (4×2 bytes = 8)
    if (frame->id == RECEIVE_ID && frame->dlc == 8) {
        // Dekode fire int16_t verdier, små‐endian til CPU‐endian
        int16_t f_i = sys_le16_to_cpu(*(int16_t *)&frame->data[0]);
        int16_t v_i = sys_le16_to_cpu(*(int16_t *)&frame->data[2]);
        int16_t r_i = sys_le16_to_cpu(*(int16_t *)&frame->data[4]);
        int16_t s_i = sys_le16_to_cpu(*(int16_t *)&frame->data[6]);

        // Pakk om til flytall (skaler fra 0–100 til 0.0–1.0)
        struct can_ret_data data = {
            .fart     = f_i  / 100.0f,
            .vinkel   = v_i  / 100.0f,
            .rotasjon = r_i  / 100.0f,
            .sving_js = s_i  / 100.0f,
        };

        last_can_rx_time = k_uptime_get();  // ← registrer tid
        // Legg meldingen i kø, håndteres i hovedløkken
        k_msgq_put(&can_msgq, &data, K_NO_WAIT);
    }
}

// Hovedløkken henter fra kø og kaller motorstyring
void process_can_data(void) {
    struct can_ret_data data;
    // Prøv å ta neste melding (ikke blokkér)
    if (k_msgq_get(&can_msgq, &data, K_NO_WAIT) == 0) {
        // Debug‐print av mottatt data
        printf("[PI→MB] fart=%.2f, vinkel=%.2f, rot=%.2f, sving_js=%.2f\n",
               data.fart, data.vinkel,
               data.rotasjon, data.sving_js);

        // Kall motor‐kontroll med alle fire parametre
        kontroller_motorene(
            data.fart,
            data.vinkel,
            data.rotasjon,
            data.sving_js
        );
    }
}

// Omskriv disse om du ønsker full CAN‐state‐rapportering
void canState(const struct device *can_dev) { /* ... */ }
const char* can_state_to_str(enum can_state state) { /* ... */ }
void send_string(const struct device *can_dev, const char *str) { /* ... */ }
