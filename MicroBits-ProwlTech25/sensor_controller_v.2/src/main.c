#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include "hc_sr04.h"

// Struktur for sensorer
hc_sr04_t sensors[4];

int main(void) {
    printk("System booting...\n");

    // Init sensorer
    hc_sr04_init(&sensors[0], "GPIO_0", 4, 3);
    hc_sr04_init(&sensors[1], "GPIO_0", 10, 9);
    hc_sr04_init(&sensors[2], "GPIO_0", 31, 28);
    hc_sr04_init(&sensors[3], "GPIO_0", 11, 30);

    printk("Sensorer initialisert. Starter målinger...\n");

    while (1) {
        double distances[4];

        for (int i = 0; i < 4; i++) {
            distances[i] = hc_sr04_read_distance(&sensors[i]);

            if (distances[i] < 0) {
                printk("Sensor %d: Ingen objekt (timeout)\n", i + 1);
            } else {
                printk("Sensor %d: Avstand = %.2f cm\n", i + 1, distances[i]);
            }
        }

        k_msleep(500); // Vent 0.5 sek mellom målinger
    }
}
