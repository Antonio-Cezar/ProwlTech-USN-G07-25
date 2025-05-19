#ifndef CANBUS_H
#define CANBUS_H

#include <zephyr/drivers/can.h>  // Zephyr CAN‐API
#include <zephyr/kernel.h>       // Zephyr kjernedefinisjoner
#include <stdint.h>              // Standard faste heltallstyper

// =====================
// CAN-konfigurasjon
// =====================
#define RECEIVE_ID     0x00000001  // CAN‐ID for mottak av joystick‐data
#define MSG_ID         0x00000002  // CAN‐ID for test‐sending
#define CAN_MSGQ_SIZE  10          // Antall elementer i CAN‐melding-kø

// =====================
// Datatype for mottatt CAN‐data (4 felt)
// =====================
struct can_ret_data {
    float fart;      // ønsket fremdrift (0.0–1.0)
    float vinkel;    // retning i radianer (−π til +π)
    float rotasjon;  // rotasjonskomponent (−1.0–1.0)
    float sving_js;  // ekstra joystick‐sving (−1.0–1.0)
};

// =====================
// Global meldingkø‐deklarasjon
// =====================
extern struct k_msgq can_msgq;   // definert i canbus.c

// =====================
// Funksjonsprototyper
// =====================

// Initialiserer og starter CAN‐hardware
void canBegin(const struct device *can_dev);

// Skriver ut CAN‐status (error count, bus state)
void canState(const struct device *can_dev);

// Henter pekeren til CAN‐enheten fra devicetree
const struct device *const get_can_dev(void);

// Setter opp filter og callback for innkommende CAN‐meldinger
int setup_can_filter(const struct device *dev,
                     can_rx_callback_t rx_cb,
                     void *cb_data);

// Callback‐funksjon som kalles for hver mottatt CAN‐frame
void can_rx_callback(const struct device *dev,
                     struct can_frame *frame,
                     void *user_data);

// Henter fra kø og utfører motorstyring
void process_can_data(void);

// Sender en test‐streng over CAN (valgfritt)
void send_string(const struct device *can_dev, const char *str);

int64_t get_last_can_rx_time(void);  // ← ny funksjon

// Hjelpefunksjon: konverterer CAN‐state‐enum til tekst
const char* can_state_to_str(enum can_state state);

#endif // CANBUS_H
