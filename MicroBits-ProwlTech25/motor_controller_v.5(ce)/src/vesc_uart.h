#ifndef VESC_UART_H
#define VESC_UART_H
#include <stdint.h>

void uart_init(void);
void send_set_duty(float duty);
void send_set_rpm(int rpm);  // <-- NY!
void uart_rx_debug(void);
void request_vesc_values(void);

void send_forwarded_set_rpm(uint8_t can_id, int rpm);
void run_can_rpm_test_sequence(void);


#endif // VESC_UART_H
