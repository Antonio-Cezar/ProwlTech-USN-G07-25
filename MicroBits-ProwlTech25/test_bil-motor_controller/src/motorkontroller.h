#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H
#include <stdbool.h> //bibliotek for å kunne bruke bools

//funksjon for å kontrollere motorene
int control_motors(int angle, int speed, bool joystick_nr);

//funksjon for å skalere og velge rotasjonsretning til motor
int mapValue(int value, int direction);

//funksjon for å skalere ønsket duty cycle i prosent til en brøk av perioden.
int speed_to_ds(int duty_cycle);
#endif
