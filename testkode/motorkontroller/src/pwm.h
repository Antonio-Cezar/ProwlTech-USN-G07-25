#ifndef __PWM_H
#define __PWM_H
#include <stdint.h>
// Set PWM period to 1ms (1 million nano seconds)
#define PWM_PERIOD_NS 40000000

// Function declarations
int pwm_begin();
//int pwm_init_channels();
void pwm_write(int motor_pin, uint32_t value);

// Define PWM channels for each motor
#define MOTOR_FRONT_LEFT 0
#define MOTOR_FRONT_RIGHT 1
#define MOTOR_REAR_LEFT 2
#define MOTOR_REAR_RIGHT 3

#endif