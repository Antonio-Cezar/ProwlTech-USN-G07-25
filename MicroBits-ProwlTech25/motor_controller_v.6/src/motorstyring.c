#include "motorstyring.h"
#include "vesc_uart.h"

#include <math.h>      // sinf, cosf, fabsf, fmaxf
#include <stdio.h>     // printf
#include <zephyr/kernel.h>  // k_msleep()

#define NUM_MOTORS 4
#define MAX_RPM    5000    // Maks RPM‐skala for normaliserte duty‐verdier

// Beregner hjul‐bidrag basert på ønsket bevegelse + sving‐joystick
struct MotorVerdier kalkulerMotorVerdier(
    float fart,
    float vinkel,
    float rotasjon,
    float sving_js
) {
    // Slå sammen hovedrotasjon og joystick‐sving
    float rot_tot = rotasjon + sving_js;

    // Finn sideveis (x) og fremover (y) komposanter
    float x = fart * sinf(vinkel);  // vinkel=0 → x=0
    float y = fart * cosf(vinkel);  // vinkel=0 → y=fart

    // Juster rotasjon basert på fart
    float just_rot = rot_tot * fart;

    // Beregn bidrag for hvert hjul (mecanum‐matrise)
    float fl = y + x + just_rot;  // front left
    float fr = y - x - just_rot;  // front right
    float rl = y - x + just_rot;  // rear left
    float rr = y + x - just_rot;  // rear right

    // Normaliser hvis noen absolutt‐verdier overskrider 1.0
    float maxv = fmaxf(fmaxf(fabsf(fl), fabsf(fr)),
                       fmaxf(fabsf(rl), fabsf(rr)));
    if (maxv > 1.0f) {
        fl /= maxv;
        fr /= maxv;
        rl /= maxv;
        rr /= maxv;
    }

    // Returner strukturen med alle fire bidrag
    return (struct MotorVerdier){ fl, fr, rl, rr };
}

// Globale glatte‐variabler for jevn akselerasjon/stopp
static float gj_fart     = 0.0f;
static float gj_vinkel   = 0.0f;
static float gj_rotasjon = 0.0f;
static const float delta = 0.05f;  // endring per steg

// Høyere nivå: kall denne med rå input, så sendes RPM via UART→CAN→VESC
int kontroller_motorene(
    float ønsket_fart,
    float ønsket_vinkel,
    float ønsket_rotasjon,
    float ønsket_sving_js
) {
    // Debug‐print av input
    printf("kontrollerer: fart=%.2f vinkel=%.2f rot=%.2f sving_js=%.2f\n",
           ønsket_fart, ønsket_vinkel,
           ønsket_rotasjon, ønsket_sving_js);

    // --- SMOOTHING med delta ---
    if (gj_fart < ønsket_fart)
        gj_fart += delta;
    else if (gj_fart > ønsket_fart)
        gj_fart -= delta;

    if (gj_vinkel < ønsket_vinkel)
        gj_vinkel += delta;
    else if (gj_vinkel > ønsket_vinkel)
        gj_vinkel -= delta;

    if (gj_rotasjon < ønsket_rotasjon)
        gj_rotasjon += delta;
    else if (gj_rotasjon > ønsket_rotasjon)
        gj_rotasjon -= delta;

    // Kalkuler motorbidrag basert på de glatte verdiene
    struct MotorVerdier mv = kalkulerMotorVerdier(
        gj_fart,
        gj_vinkel,
        gj_rotasjon,
        ønsket_sving_js  // Denne glattes ikke her, men du kan utvide smoothing om ønsket
    );

    // Skaler til RPM
    int32_t rpms[NUM_MOTORS] = {
        (int32_t)(mv.front_left  * MAX_RPM),
        (int32_t)(mv.front_right * MAX_RPM),
        (int32_t)(mv.rear_left   * MAX_RPM),
        (int32_t)(mv.rear_right  * MAX_RPM),
    };
    printf("sender rpms: %ld, %ld, %ld, %ld\n",
           (long)rpms[0], (long)rpms[1],
           (long)rpms[2], (long)rpms[3]);

    // Send RPM til riktig motor via riktig metode
    // ID 0 er master (direkte UART), ID 1-3 er slaver (CAN-forward)
    uint8_t ids[NUM_MOTORS] = {0, 1, 2, 3};
    for (int i = 0; i < NUM_MOTORS; i++) {
        if (ids[i] == 0) {
            // Master VESC (foran venstre) – UART direkte
            send_set_rpm(rpms[i]);
        } else {
            // Slave VESC – via CAN-forward
            send_forwarded_rpm(ids[i], rpms[i]);
        }
        k_msleep(1);  // liten pause mellom meldinger
    }

    return 0;
}
