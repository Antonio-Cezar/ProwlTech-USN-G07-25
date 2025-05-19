#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdio.h>
#include <stdint.h> 
#include <stddef.h>  

#include "vesc_uart.h"
#include "canbus.h"
#include "motorstyring.h"  // <- nødvendig for kontroller_motorene()

void main(void) {
    uart_init();
    k_msleep(100);

    const struct device *const can_dev = get_can_dev();
    if (!can_dev) {
        printf("Error: fant ikke CAN-enhet\n");
        return;
    }
    canBegin(can_dev);
    k_msleep(100);

    printf("UART+CAN klar. Starter loop for master+slaver...\n\n");

    const int TIMEOUT_MS = 200;
    static int null_sent = 0;

    while (1) {
        process_can_data();  // håndterer ny CAN-data

        int64_t nå = k_uptime_get();
        int64_t sist = get_last_can_rx_time();

        if ((nå - sist > TIMEOUT_MS) && !null_sent) {
            printf("Timeout – sender null RPM!\n");
            kontroller_motorene(0.0f, 0.0f, 0.0f, 0.0f);
            null_sent = 1;
        }

        if (nå - sist <= TIMEOUT_MS) {
            null_sent = 0;
        }

        k_busy_wait(200);  // du kan evt. gjøre dette raskere (f.eks. 200 µs)
    }
}
