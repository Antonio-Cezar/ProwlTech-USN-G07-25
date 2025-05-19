#ifndef CANBUS_H
#define CANBUS_H

#include <zephyr/drivers/can.h>

// =====================
// CAN-konfigurasjon
// =====================
#define RECEIVE_ID 0x00000001     // ID for mottak av joystick-data (PI → motor-MB)
#define MSG_ID     0x00000002     // ID for sending testdata (motor-MB → PI)
#define CAN_MSGQ_SIZE 10          // Størrelse på CAN message queue

// =====================
// Strukturer
// =====================

// Struktur for å lagre mottatt joystick-data
struct can_ret_data {
    float vinkel;
    float fart;
    float rotasjon;
};

// =====================
// Globale deklarasjoner
// =====================
extern struct k_msgq can_msgq;  // Message queue for mottatte CAN-meldinger

// =====================
// Funksjonsprototyper
// =====================

// Oppstart og status
void canBegin(const struct device *can_dev);
void canState(const struct device *can_dev);
const struct device *const get_can_dev(void);

// Mottak og behandling
void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data);
int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data);
void process_can_data(void);

// Sending
void send_string(const struct device *can_dev, const char *str);
const char* can_state_to_str(enum can_state state);

#endif // CANBUS_H
