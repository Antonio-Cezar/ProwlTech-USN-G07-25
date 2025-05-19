#include "vesc_uart.h"
#include <zephyr/drivers/uart.h>    // uart_poll_out
#include <zephyr/kernel.h>          // k_busy_wait, k_msleep
#include <zephyr/device.h>          // DEVICE_DT_GET, device_is_ready
#include <zephyr/devicetree.h>      // DEVICE_DT_GET
#include <string.h>                 // memcpy
#include <stdio.h>
#include <stdint.h>

#define UART_NODE_LABEL DT_NODELABEL(uart1)

static const struct device *uart_dev = NULL;

// CRC16‐ARC (polynom 0x1021) over data[]
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

void uart_init(void) {
    uart_dev = DEVICE_DT_GET(UART_NODE_LABEL);
    if (!device_is_ready(uart_dev)) {
        printf("ERROR: UART1 ikke klar\n");
    } else {
        printf("UART1 initialisert: %s\n", uart_dev->name);
    }
}

// Denne fungerte i prototypen din for master‐ESC
void send_set_rpm(int rpm) {
    if (!uart_dev) {
        printf("ERROR: UART not initialized\n");
        return;
    }

    // Bygg payload: [COMM_SET_RPM][4‐byte big‐endian rpm]
    uint8_t payload[1 + sizeof(int32_t)];
    payload[0] = COMM_SET_RPM;
    int32_t rpm_be = __builtin_bswap32(rpm);
    memcpy(&payload[1], &rpm_be, sizeof(rpm_be));

    uint16_t crc = compute_crc16(payload, sizeof(payload));

    // Bygg ramme: STX | LEN | payload | CRC_H | CRC_L | ETX
    uint8_t frame[2 + sizeof(payload) + 2 + 1];
    size_t idx = 0;
    frame[idx++] = VESC_STX;
    frame[idx++] = (uint8_t)sizeof(payload);
    memcpy(&frame[idx], payload, sizeof(payload)); idx += sizeof(payload);
    frame[idx++] = (uint8_t)(crc >> 8);
    frame[idx++] = (uint8_t)(crc & 0xFF);
    frame[idx++] = VESC_ETX;

    for (size_t i = 0; i < idx; i++) {
        uart_poll_out(uart_dev, frame[i]);
        k_busy_wait(100);  // 100 µs mellom bytes
    }

    printf("RPM-pakke sendt: %d RPM\n", rpm);
}

// Wrapper‐funksjon som pakker SET_RPM i en CAN‐forward‐kommando
void send_forwarded_rpm(uint8_t node_id, int32_t rpm) {
    if (!uart_dev) {
        return;
    }
    int32_t rpm_be = __builtin_bswap32(rpm);

    // Payload: [COMM_FORWARD_CAN][node_id][COMM_SET_RPM][4‐byte rpm]
    uint8_t payload[1 + 1 + 1 + sizeof(rpm_be)];
    size_t p = 0;
    payload[p++] = COMM_FORWARD_CAN;
    payload[p++] = node_id;
    payload[p++] = COMM_SET_RPM;
    memcpy(&payload[p], &rpm_be, sizeof(rpm_be));
    p += sizeof(rpm_be);

    uint16_t crc = compute_crc16(payload, p);

    uint8_t frame[2 + p + 2 + 1];
    size_t idx = 0;
    frame[idx++] = VESC_STX;
    frame[idx++] = (uint8_t)p;
    memcpy(&frame[idx], payload, p); idx += p;
    frame[idx++] = (uint8_t)(crc >> 8);
    frame[idx++] = (uint8_t)(crc & 0xFF);
    frame[idx++] = VESC_ETX;

    for (size_t i = 0; i < idx; i++) {
        uart_poll_out(uart_dev, frame[i]);
        k_busy_wait(100);
    }
}

// (Behold request_vesc_values og uart_rx_debug om du trenger dem)
