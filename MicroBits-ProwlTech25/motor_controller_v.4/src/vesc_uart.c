#include "vesc_uart.h"

#include <zephyr/drivers/uart.h>    // uart_poll_out, uart_poll_in
#include <zephyr/kernel.h>          // k_busy_wait, k_msleep
#include <zephyr/device.h>          // device_is_ready
#include <zephyr/devicetree.h>     // DEVICE_DT_GET
#include <string.h>                 // memcpy
#include <stdio.h>
#include <stdint.h>

#define UART_NODE_LABEL DT_NODELABEL(uart1)

// Strukturen som peker til UART‐driveren
static const struct device *uart_dev = NULL;

// CRC16‐algoritme for VESC‐pakker
static uint16_t compute_crc16(const uint8_t *data, size_t len) {
    uint16_t crc = 0;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

// Åpner UART1 og sjekker at den er klar
void uart_init(void) {
    uart_dev = DEVICE_DT_GET(UART_NODE_LABEL);
    if (!device_is_ready(uart_dev)) {
        printf("ERROR: UART1 ikke klar\n");
    } else {
        printf("UART1 initialisert: %s\n", uart_dev->name);
    }
}

// Ubrukt i din kjede, men standard SET_DUTY‐pakke
void send_set_duty(float duty) {
    // ... som tidligere ...
}

// Ubrukt i din kjede, men standard SET_RPM‐pakke
void send_set_rpm(int32_t rpm) {
    // ... som tidligere ...
}

// Hovedmetode: pakker SET_RPM i FORWARD_CAN‐ramme
void send_forwarded_rpm(uint8_t node_id, int32_t rpm) {
    if (!uart_dev) return;

    // Konverter til big‐endian før packing
    int32_t rpm_be = __builtin_bswap32(rpm);

    // Lag payload: [COMM_FORWARD_CAN][node_id][COMM_SET_RPM][rpm_be…]
    uint8_t payload[1+1+1+4];
    size_t p = 0;
    payload[p++] = COMM_FORWARD_CAN;
    payload[p++] = node_id;
    payload[p++] = COMM_SET_RPM;
    memcpy(&payload[p], &rpm_be, sizeof(rpm_be));
    p += sizeof(rpm_be);

    // Beregn CRC over payload
    uint16_t crc = compute_crc16(payload, p);

    // Bygg ramme: STX, lengde, payload, CRC_hi, CRC_lo, ETX
    uint8_t frame[2 + p + 2 + 1];
    size_t idx = 0;
    frame[idx++] = VESC_STX;
    frame[idx++] = (uint8_t)p;
    memcpy(&frame[idx], payload, p); idx += p;
    frame[idx++] = crc >> 8;
    frame[idx++] = crc & 0xFF;
    frame[idx++] = VESC_ETX;

    // Send ramme byte‐for‐byte med liten delay
    for (size_t i = 0; i < idx; i++) {
        uart_poll_out(uart_dev, frame[i]);
        k_busy_wait(100);
    }
}

// Spørring og RX‐debug (likt som før)
void request_vesc_values(void) { /* … */ }
void uart_rx_debug(void)      { /* … */ }
