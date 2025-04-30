#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdio.h>
#include <stdint.h> 
#include <stddef.h>  

#include "vesc_uart.h"
#include "canbus.h"

void main(void) {
    /* 1) Init UART (til master‐ESC) */
    uart_init();
    k_msleep(100);

    /* 2) Init CAN på master */
    const struct device *const can_dev = get_can_dev();
    if (!can_dev) {
        printf("Error: fant ikke CAN-enhet\n");
        return;
    }
    canBegin(can_dev);
    k_msleep(100);

    printf("UART+CAN klar. Starter loop for master+slaver...\n\n");

    /* 3) Liste over ESC‐ID-er og ønskede RPM */
    // 0 = master, 1–3 = slave-ESC-er
    const uint8_t  ids[]  = {0, 1, 2, 3};
    const int32_t  rpms[] = {4000, 4000, 4000, 4000};
    const size_t   n     = sizeof(ids) / sizeof(ids[0]);

    /* 4) Evig loop: send til hver ESC i rekkefølge */
    while (1) {
        while (1) {
            process_can_data();
            k_msleep(10);
        }
        
    }
}
