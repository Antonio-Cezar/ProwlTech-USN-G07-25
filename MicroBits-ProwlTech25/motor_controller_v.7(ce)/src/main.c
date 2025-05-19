#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdio.h>
#include <stdint.h> 
#include <stddef.h>  
#include <string.h>  // for memcmp

#include "vesc_uart.h"
#include "canbus.h"
#include "motorstyring.h"

int main(void) {
    uart_init();
    k_msleep(100);

    const struct device *const can_dev = get_can_dev();
    if (!can_dev) {
        printf("Error: fant ikke CAN-enhet\n");
        return -1;
    }
    canBegin(can_dev);
    k_msleep(100);

    printf("UART+CAN klar. Starter loop for master+slaver...\n\n");

    const int TIMEOUT_MS = 200;
    static int null_sent = 0;
    static struct can_ret_data forrige_data = {0};

    while (1) {
        struct can_ret_data data;

        // Hent ut ALLE meldinger i køen
        while (k_msgq_get(&can_msgq, &data, K_NO_WAIT) == 0) {
            if (memcmp(&data, &forrige_data, sizeof(data)) != 0) {
                kontroller_motorene(data.fart, data.vinkel, data.rotasjon, data.sving_js);
                forrige_data = data;
            }
        }

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

        k_msleep(5);  // 200 Hz hovedloop
    }

    return 0;
}
