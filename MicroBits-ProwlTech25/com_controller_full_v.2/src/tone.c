#include <zephyr/kernel.h>          // Zephyr-kjernefunksjoner (for eksempel printk)
#include <zephyr/device.h>          // Generelle enhetsdrivere
#include <zephyr/drivers/pwm.h>     // PWM-driver – for å kunne generere lydsignaler
#include "tone.h"                   // Egen header-fil for tonefunksjoner (deklarasjoner)

// (PROWLTECH25 - CA)

// Definerer hvilken PWM-enhet vi bruker basert på devicetree-navn
#define SPEAKER_PWM_NODE DT_NODELABEL(pwm0) // Bruker pwm0 (fra devicetree)
// PWM-kanal vi bruker – pinne 0 på pwm0 tilsvarer channel 0
#define SPEAKER_CHANNEL 0  // P0 = channel 0 på pwm0

// Henter en peker til PWM-enheten
const struct device *pwm_dev = DEVICE_DT_GET(SPEAKER_PWM_NODE);

//--------------------------------------------------------------
// Initialiserer tone-modulen – sjekker at PWM-enheten er klar
//--------------------------------------------------------------
void tone_init(void) {
    if (!device_is_ready(pwm_dev)) {
        printk("PWM device not ready!\n");
    }
}

//--------------------------------------------------------------
// Spiller en tone ved hjelp av PWM
//-------------------------------------------------------------
void play_tone(void) {
    if (!device_is_ready(pwm_dev)) return; // Sjekk at enheten er klar

    // Sett en PWM på 750 Hz:
    pwm_set(pwm_dev, SPEAKER_CHANNEL, 1333333U, 800000U, 0);
}

//--------------------------------------------------------------
// Stopper lyden ved å sette duty cycle til 0
//--------------------------------------------------------------
void stop_tone(void) {
    if (!device_is_ready(pwm_dev)) return; // Sjekk at enheten er klar

    // Stopp tone (duty = 0)
    pwm_set(pwm_dev, SPEAKER_CHANNEL, 1000000U, 0, 0);
}
