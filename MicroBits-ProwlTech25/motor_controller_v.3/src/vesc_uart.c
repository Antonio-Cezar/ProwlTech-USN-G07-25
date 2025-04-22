// vesc_uart.c
// Enkel VESC UART‑protokoll for å sette duty‑cycle på VESC via COMM_SET_DUTY

#include "vesc_uart.h"
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define UART_NODE_LABEL    DT_NODELABEL(uart1)
#define VESC_STX_SHORT     0x02    // Startbyte for korte pakker
#define VESC_ETX           0x03    // Sluttbyte
#define VESC_CMD_SET_DUTY  5       // Kommando‑ID for COMM_SET_DUTY

static const struct device *uart_dev = NULL;

/**
 * compute_crc16() - Beregn CRC16‑ARC over en byte‑array
 * @data: peker til data
 * @len:  lengde på data i bytes
 *
 * Returnerer 16‑bits CRC.
 */
static uint16_t compute_crc16(const uint8_t *data, size_t len)
{
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

/**
 * uart_init() - Henter og sjekker at UART1 finnes
 */
void uart_init(void)
{
    uart_dev = DEVICE_DT_GET(UART_NODE_LABEL);
    if (!device_is_ready(uart_dev)) {
        printf("ERROR: UART1 device not ready\n");
    } else {
        printf("UART1 initialized: %s\n", uart_dev->name);
    }
}

/**
 * send_set_duty() - Sender COMM_SET_DUTY‑pakke med duty [-1.0 .. 1.0]
 * @duty: ønsket duty cycle, clamped til [-1, +1]
 */
void send_set_duty(float duty)
{
    if (!uart_dev) {
        printf("ERROR: UART not initialized\n");
        return;
    }

    // Klamp duty-verdien
    if (duty < -1.0f) duty = -1.0f;
    if (duty >  1.0f) duty =  1.0f;

    // Bygg payload: [kommando‑byte, 4 byte float]
    uint8_t payload[1 + sizeof(float)];
    payload[0] = VESC_CMD_SET_DUTY;
    memcpy(&payload[1], &duty, sizeof(float));

    // Beregn CRC over payload
    uint16_t crc = compute_crc16(payload, sizeof(payload));

    // Bygg hele UART‑rammen: STX | LEN | payload | CRC_H | CRC_L | ETX
    uint8_t frame[2 + sizeof(payload) + 2 + 1];
    size_t idx = 0;
    frame[idx++] = VESC_STX_SHORT;        // Startbyte
    frame[idx++] = (uint8_t)sizeof(payload); // Payload‑lengde
    memcpy(&frame[idx], payload, sizeof(payload));
    idx += sizeof(payload);
    frame[idx++] = (uint8_t)(crc >> 8);   // CRC MSB
    frame[idx++] = (uint8_t)(crc & 0xFF); // CRC LSB
    frame[idx++] = VESC_ETX;              // Endbyte

    // Send pakke byte for byte
    for (size_t i = 0; i < idx; i++) {
        uart_poll_out(uart_dev, frame[i]);
    }
}
