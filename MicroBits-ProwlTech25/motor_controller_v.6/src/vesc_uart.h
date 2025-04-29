#ifndef VESC_UART_H
#define VESC_UART_H

#include <stdint.h>

// Start‐ og slutt‐bytes i VESC‐UART‐rammer
#define VESC_STX 0x02
#define VESC_ETX 0x03

// Kommando‐ID’er i offisiell VESC‐firmware
#define COMM_GET_VALUES        4
#define COMM_SET_DUTY          5
#define COMM_SET_CURRENT       6
#define COMM_SET_CURRENT_BRAKE 7
#define COMM_SET_RPM           8
#define COMM_FORWARD_CAN      34  // Wrapper for å forwarde kommando over CAN

// Initialiserer UART1
void uart_init(void);

// Eksisterende metoder (ikke brukt i din kjede, men tilgjengelig)
void send_set_duty(float duty);
void send_set_rpm(int32_t rpm);

// Ny wrapper-metode som pakker SET_RPM i FORWARD_CAN
void send_forwarded_rpm(uint8_t node_id, int32_t rpm);

// Spørring og debug
void request_vesc_values(void);
void uart_rx_debug(void);

#endif // VESC_UART_H
