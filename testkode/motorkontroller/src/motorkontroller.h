#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H
#include <stdbool.h>
// Function prototypes
void control_motors(int angle, int speed, bool joystick_nr);
int mapValue(int value, int direction);
int calculate_speed(int duty_cycle);
#endif
