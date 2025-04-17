#ifndef VESC_UART_H
#define VESC_UART_H

#include <zephyr/device.h>

void uart_init(void);
void send_duty_uart(float duty); // duty mellom -1.0 og 1.0
void send_duty_uart_motor(int motor_id, float duty); //sender en float duty [-1.0 1.0] til en motor id (0-3)
void send_duty_uart_alle_motor(float duty_array[4]);
#endif //VESC_UART_H


