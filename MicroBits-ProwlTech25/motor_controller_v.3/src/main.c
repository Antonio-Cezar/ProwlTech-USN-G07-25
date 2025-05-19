#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <stdio.h>
#include "vesc_uart.h"

int main(void) {
    uart_init();

    const struct device *uart = get_uart_dev();
    if (!uart || !device_is_ready(uart)) {
        printf("UART-enhet ikke klar.\n");
        return -1;
    }

    printf("Starter VESC RPM-kontrollloop...\n");

    while (1) {
        // Send 1500 RPM i 5 sekunder
        printf("Setter RPM til 1500 i 5 sekunder\n");
        for (int i = 0; i < 50; i++) {  // 50 * 100ms = 5 sekunder
            send_set_rpm(5000);
            k_msleep(100);
        }

        // Stopp (RPM = 0) i 2 sekunder
        printf("Stopper motor i 2 sekunder\n");
        for (int i = 0; i < 20; i++) {  // 20 * 100ms = 2 sekunder
            send_set_rpm(0);
            k_msleep(100);
        }
    }

    return 0;
}
