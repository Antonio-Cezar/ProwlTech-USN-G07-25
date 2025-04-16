#include <zephyr/kernel.h>
#include "canbus.h"
#include "tone.h"

int main(void) {
    canBegin();
    tone_init();  // Hvis du ønsker å teste lyd direkte

    while (1) {
        k_msleep(500);
    }

    return 0;
}
