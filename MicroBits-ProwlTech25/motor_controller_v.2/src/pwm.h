#ifndef __PWM_H
#define __PWM_H
#include <stdint.h>

// Definerer perioden for hvert PWM-signal i nano-sekunder
#define PWM_PERIOD_NS 40000000

// Funksjon for å initialisere PWM og oppsett. Hvilke "channels" og "pins" som brukes.
int pwm_begin();

// Funksjon for å sette PWM signal på en pin manuelt. Den repeterer signalet helt til den
// får ordre om noe annet
void pwm_write(int motor_pin, uint32_t value);

// Definerer hvilke motor som bruker hvilken kanal på microbiten (Det er maks 4 PWM kanaler per microbit)
// Venstre fremre motor bruker kanal 0, høyre fremre bruker kanal 1,
// venstre bakre motor bruker kanal 2, og høyre bakre motor bruker kanal 3.
#define MOTOR_FRONT_LEFT 0
#define MOTOR_FRONT_RIGHT 1
#define MOTOR_REAR_LEFT 2
#define MOTOR_REAR_RIGHT 3

#endif