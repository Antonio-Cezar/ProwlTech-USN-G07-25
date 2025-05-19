#ifndef VESC_UART_H
#define VESC_UART_H

void uart_init(void);
void send_set_duty(float duty);
void send_set_rpm(int rpm);  // <-- NY!
void uart_rx_debug(void);
void request_vesc_values(void);
// Legg til i vesc_uart.h
const struct device *get_uart_dev(void);


#endif // VESC_UART_H
