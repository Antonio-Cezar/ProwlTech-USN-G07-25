#include <zephyr/kernel.h>     // Inkluderer Zephyr sitt kjernebibliotek for f.eks. tid og trådkontroll
#include "canbus.h"            // henter for CAN-funksjoner (canBegin, filters, callback.)
#include "tone.h"              // Henter for tone-funksjoner (init, play, stop.)

// (PROWLTECH25 - CA)

//--------------------------------------------------------------
// Hovedfunksjon – kjøres ved oppstart
//--------------------------------------------------------------
int main(void) {

    canBegin(); // Starter opp og konfigurerer CAN-bussen (mottak av meldinger)

    tone_init(); // Initialiserer tone-modulen

    // while løkke – holder programmet kjørende
    while (1) {
        k_msleep(500); // Venter i 500 ms mellom hver iterasjon
    }
    return 0;
}
