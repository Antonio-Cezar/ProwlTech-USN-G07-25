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
#define VESC_CMD_SET_RPM   5       // Kommando‑ID for COMM_SET_RPM
#define VESC_CMD_GET_VALUES 4

#define VESC_STX_SHORT     0x02
#define VESC_ETX           0x03
#define VESC_CMD_FORWARD_CAN 33
#define VESC_CMD_SET_RPM     5

#include <zephyr/drivers/pwm.h>
const struct device *speaker_dev;

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

void send_set_rpm(int rpm)
{
    if (!uart_dev) {
        printf("ERROR: UART not initialized\n");
        return;
    }

    // Bygg payload: [kommando‑byte, 4 byte int32]
    uint8_t payload[1 + sizeof(int32_t)];
    payload[0] = VESC_CMD_SET_RPM;
    int32_t rpm_be = __builtin_bswap32(rpm);
    memcpy(&payload[1], &rpm_be, sizeof(int32_t));

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
        k_busy_wait(100);  // Wait 100us between bytes
    }

    printf("RPM-pakke sendt: %d RPM\n", rpm);
}

void uart_rx_debug(void)
{
    printf("Starter UART RX-debug. Leser innkommende bytes:\n");

    while (1) {
        uint8_t byte;
        int ret = uart_poll_in(uart_dev, &byte);

        if (ret == 0) {
            printf("Mottatt byte: 0x%02X (%c)\n", byte, byte >= 32 && byte <= 126 ? byte : '.');
            if (ret == 0) {
                printf("Mottatt byte: 0x%02X (%c)\n", byte, byte >= 32 && byte <= 126 ? byte : '.');
            
                if (device_is_ready(speaker_dev)) {
                    pwm_set_cycles(speaker_dev, 0, 5000, 2500, 0);  // 1000 Hz, 50%
                    k_msleep(100);
                    pwm_set_cycles(speaker_dev, 0, 5000, 0, 0);     // stopp
                } else {
                    printf("Speaker not ready!\n");
                }
            }
        } else {
            k_msleep(10);  // Ikke blokker CPU
        }
    }    
}

void request_vesc_values(void)
{
    if (!uart_dev) {
        printf("ERROR: UART not initialized\n");
        return;
    }

    uint8_t payload[1];
    payload[0] = VESC_CMD_GET_VALUES;

    uint16_t crc = compute_crc16(payload, sizeof(payload));

    uint8_t frame[6];
    size_t idx = 0;
    frame[idx++] = VESC_STX_SHORT;         // Start
    frame[idx++] = sizeof(payload);        // Length
    frame[idx++] = payload[0];             // Command
    frame[idx++] = (uint8_t)(crc >> 8);    // CRC High
    frame[idx++] = (uint8_t)(crc & 0xFF);  // CRC Low
    frame[idx++] = VESC_ETX;               // End

    printf("Sender COMM_GET_VALUES-forespørsel...\n");
    for (size_t i = 0; i < idx; i++) {
        uart_poll_out(uart_dev, frame[i]);
    }
}

void speaker_init(void) {
    speaker_dev = DEVICE_DT_GET(DT_ALIAS(pwm_speaker));
    if (!device_is_ready(speaker_dev)) {
        printf("Speaker not ready!\n");
    } else {
        printf("Speaker ready: %s\n", speaker_dev->name);
    }
}

void send_forwarded_set_rpm(uint8_t can_id, int rpm) {
    if (!uart_dev) {
        printf("ERROR: UART not initialized\n");
        return;
    }

    // Lag innerste RPM-payload
    uint8_t rpm_payload[1 + sizeof(int32_t)];
    rpm_payload[0] = VESC_CMD_SET_RPM;
    int32_t rpm_be = __builtin_bswap32(rpm);
    memcpy(&rpm_payload[1], &rpm_be, sizeof(int32_t));

    // Lag ytre CAN-forward-payload
    uint8_t can_payload[1 + 1 + sizeof(rpm_payload)];
    can_payload[0] = VESC_CMD_FORWARD_CAN;
    can_payload[1] = can_id;  // Mottaker-VESC
    memcpy(&can_payload[2], rpm_payload, sizeof(rpm_payload));

    // Beregn CRC
    uint16_t crc = compute_crc16(can_payload, sizeof(can_payload));

    // Lag hele rammen
    uint8_t frame[2 + sizeof(can_payload) + 2 + 1];
    size_t idx = 0;
    frame[idx++] = VESC_STX_SHORT;
    frame[idx++] = (uint8_t)sizeof(can_payload);
    memcpy(&frame[idx], can_payload, sizeof(can_payload));
    idx += sizeof(can_payload);
    frame[idx++] = (uint8_t)(crc >> 8);
    frame[idx++] = (uint8_t)(crc & 0xFF);
    frame[idx++] = VESC_ETX;

    for (size_t i = 0; i < idx; i++) {
        uart_poll_out(uart_dev, frame[i]);
        k_busy_wait(100);  // 100 µs pause
    }

    printf("Sendte %d RPM til VESC med CAN ID %d\n", rpm, can_id);
}

void run_can_rpm_test_sequence(void) {
    for (uint8_t can_id = 0; can_id <= 3; can_id++) {
        printf("Starter test for VESC ID %d\n", can_id);

        if (can_id == 0) {
            send_set_rpm(5000);  // UART direkte
        } else {
            send_forwarded_set_rpm(can_id, 5000);  // via CAN
        }

        k_msleep(2000);  // kjør i 2 sek

        if (can_id == 0) {
            send_set_rpm(0);
        } else {
            send_forwarded_set_rpm(can_id, 0);
        }

        k_msleep(1000);  // pause
    }

    printf("Testsekvens fullført\n");
}