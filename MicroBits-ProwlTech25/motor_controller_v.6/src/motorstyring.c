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
    float rot_tot = rotasjon + sving_js;

    float x = fart * sinf(vinkel);
    float y = fart * cosf(vinkel);
    float just_rot = rot_tot * fart;

    float fl = y + x + just_rot;
    float fr = y - x - just_rot;
    float rl = y - x + just_rot;
    float rr = y + x - just_rot;

    float maxv = fmaxf(fmaxf(fabsf(fl), fabsf(fr)),
                       fmaxf(fabsf(rl), fabsf(rr)));
    if (maxv > 1.0f) {
        fl /= maxv;
        fr /= maxv;
        rl /= maxv;
        rr /= maxv;
    }

    return (struct MotorVerdier){ fl, fr, rl, rr };
}

// Globale glatte‐variabler for jevn akselerasjon/stopp
static float gj_fart     = 0.0f;
static float gj_vinkel   = 0.0f;
static float gj_rotasjon = 0.0f;
static float gj_sving_js = 0.0f;   // ← lagt til for smoothing av sving_js
static const float delta = 0.2f;   // ← økt for raskere respons

int kontroller_motorene(
    float ønsket_fart,
    float ønsket_vinkel,
    float ønsket_rotasjon,
    float ønsket_sving_js
) {
    printf("kontrollerer: fart=%.2f vinkel=%.2f rot=%.2f sving_js=%.2f\n",
           ønsket_fart, ønsket_vinkel,
           ønsket_rotasjon, ønsket_sving_js);

    // SMOOTHING for alle fire verdier
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

    if (gj_sving_js < ønsket_sving_js)
        gj_sving_js += delta;
    else if (gj_sving_js > ønsket_sving_js)
        gj_sving_js -= delta;

    // Kalkuler motorbidrag basert på glatte verdier
    struct MotorVerdier mv = kalkulerMotorVerdier(
        gj_fart,
        gj_vinkel,
        gj_rotasjon,
        gj_sving_js
    );

    int32_t rpms[NUM_MOTORS] = {
        (int32_t)(mv.front_left  * MAX_RPM),
        (int32_t)(mv.front_right * MAX_RPM),
        (int32_t)(mv.rear_left   * MAX_RPM),
        (int32_t)(mv.rear_right  * MAX_RPM),
    };
    
    // Hvis fart = 0, nullstill alt (uansett smoothing)
    if (ønsket_fart == 0.0f) {
        for (int i = 0; i < NUM_MOTORS; i++) {
            rpms[i] = 0;
        }
    } else {
        // Ellers bruk minimum RPM hvis nødvendig
        const int32_t MIN_RPM = 1000;
        for (int i = 0; i < NUM_MOTORS; i++) {
            if (rpms[i] != 0 && abs(rpms[i]) < MIN_RPM) {
                rpms[i] = (rpms[i] > 0) ? MIN_RPM : -MIN_RPM;
            }
        }
    }

    printf("sender rpms: %ld, %ld, %ld, %ld\n",
           (long)rpms[0], (long)rpms[1],
           (long)rpms[2], (long)rpms[3]);

    uint8_t ids[NUM_MOTORS] = {0, 1, 2, 3};
    for (int i = 0; i < NUM_MOTORS; i++) {
        if (ids[i] == 0) {
            send_set_rpm(rpms[i]); // Master via UART
        } else {
            send_forwarded_rpm(ids[i], rpms[i]); // Slaver via CAN-forward
        }
        k_msleep(1);
    }

    return 0;
}
