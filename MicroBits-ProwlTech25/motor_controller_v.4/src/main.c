#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdio.h>

#include "canbus.h"
#include "vesc_uart.h"

// Test 2: CAN-forwarding mot flere VESC-er
static void can_forward_test(void) {
    printf("=== CAN-forwarding test ===\n");
    // Bruk samme uart_init hvis du ikke har kalt det før
    // uart_init();

    // Init CAN (master-VESC)
    const struct device * const can_dev = get_can_dev();
    if (!can_dev) {
        printf("Error: fant ikke CAN-enhet\n");
        return;
    }
    canBegin(can_dev);
    k_msleep(100);

    // Sett ID-ene du har konfigurert på slave-VESC-ene
    const uint8_t ids[]  = {0, 1, 2, 3};
    const int32_t rpms[] = {1000, 2000, 3000, 4000};
    const size_t n = sizeof(ids)/sizeof(ids[0]);

    for (size_t i = 0; i < n; i++) {
        printf("Tester slave ID %u med %ld RPM\n",
               ids[i], (long)rpms[i]);
        send_forwarded_rpm(ids[i], rpms[i]);  // wraps COMM_SET_RPM i COMM_FORWARD_CAN :contentReference[oaicite:0]{index=0}&#8203;:contentReference[oaicite:1]{index=1}
        k_msleep(500);

        // Stopp denne motoren
        send_forwarded_rpm(ids[i], 0);
        k_msleep(200);
    }
    printf("CAN-forwarding test ferdig.\n\n");
}

void main(void) {
    // Kjør først enkel UART-test
    direct_rpm_test();

    // Så kjør CAN-forwarding-test
    can_forward_test();

    // Etter testene: gå over i normal drift
    printf("Starter normal drift (prosesser CAN → motorstyring)...\n");
    while (1) {
        process_can_data();  // Håndterer joystick-meldinger som før
        k_msleep(1);
    }
}
