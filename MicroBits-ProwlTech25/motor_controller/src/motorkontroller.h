#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H
#include <stdbool.h> //bibliotek for å kunne bruke bools - Fenri24

//funksjon for å kontrollere motorene - Fenri24
int control_motors(int angle, int speed, bool joystick_nr);

//funksjon for å skalere og velge rotasjonsretning til motor - Fenri24
int mapValue(int value, int direction);

//funksjon for å skalere ønsket duty cycle i prosent til en brøk av perioden. - Fenri24
int speed_to_ds(int duty_cycle);
#endif
