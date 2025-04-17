#include "vesc_uart.h"
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <stdio.h>
#include <string.h>

#define UART_DEVICE_NODE DEVICE_DT_GET(DT_NODELABEL(uart0)) //kan endres til uart1

const struct device *uart_dev;

void uart_init(void) {
    uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));
    if (!device_is_ready(uart_dev)) {
        printf("Finner ikke UART enhet!\n");
    }else {
        printf("uart initialisert\n");
    }
    
}

void send_duty_uart_motor(int motor_id, float duty){
    if (!uart_dev) {
        printf("uart ikke initialisert\n");
        return;
    }

    if (motor_id < 0 || motor_id > 3) {
        printf("ugyldig motor id: %d\n", motor_id);
        return;
    }

    if (duty < -1.0f) duty = -1.0f;
    if (duty > 1.0f) duty = 1.0f;

    //pakker float duty i binært format

    uint8_t buffer[6];
    buffer[0] = 0xA5; //start byte (valgfri)
    buffer[1] = (uint8_t)motor_id;
    memcpy(&buffer[2], &duty, sizeof(float));

    for (int i = 0; i < sizeof(buffer); ++i) {
        uart_poll_out(uart_dev, buffer[i]);
    }

    printf("sendt til motpr %d: duty=%.2f\n", motor_id, duty);
}

void send_duty_uart(float duty){
    send_duty_uart_motor(0, duty); //bruker motor 0 som default
}

void send_duty_uart_alle_motor(float duty_array[4]){
    if (!uart_dev){
        printf("UART ikke initialisert\n");
        return;
    }

    uint8_t buffer[18];
    buffer[0] = 0xA5; // startbyte

    //pakk 4 floatverdier 4x4 byte
    for (int i = 0; i < 4; ++i){
        float d = duty_array[i];
        if (d < -1.0f) d = -1.0f;
        if (d > 1.0f) d = 1.0f;
        memcpy(&buffer[1 + i * 4], &d, sizeof(float));
    }

    //beregn checksum som XOR over byte 0-16
    uint8_t checksum = 0;
    for (int i = 0; i < 17; ++i) {
        checksum ^= buffer[i];

    } 
    buffer [17] = checksum; 
    
    //send alle bytes
    for (int i = 0; i < sizeof(buffer); ++i){
        uart_poll_out(uart_dev, buffer[i]);
    }

    printf("sendt 4 motorverdier med checksum: %.2f %.2f %.2f %.2f\n", duty_array[0], duty_array[1], duty_array[2], duty_array[3]);
}
