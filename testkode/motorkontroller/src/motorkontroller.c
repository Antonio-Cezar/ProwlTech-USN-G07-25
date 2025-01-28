#include "pwm.h"
#include "motorkontroller.h"
#include <stdio.h>
#include <stdbool.h>


// Function to control motors based on joystick input
void control_motors(int angle, int speed, bool joystick_nr)
{
    if (joystick_nr == true){

        if (speed > 10){
        // Determine motor control based on angle
            if (angle >= 68 && angle < 113) {
            // Forward motion
            // Adjust speed based on joystick radius
                pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
                pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(speed, 1))));
                pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
                pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
                printf("FORWARD\n");

            } else if(angle >= 113 && angle < 158){
            // Diagonal left-forward motion
                pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
                pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(0, 1))));
                pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(0, 1))));
                pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
                printf("Right-Forward diagonal");

            } else if (angle >= 158 && angle < 203) {
            // Left strafe motion
            // Implement left strafe motor control
                pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
                pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(speed, 2))));
                pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
                pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
                printf("RIGHT STRAFE\n");

            } else if (angle >= 203 && angle < 248){
            //Diagonal left-backward motion
                pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(0, 2))));
                pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(speed, 2))));
                pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
                pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(0, 2))));
                printf("Right-Backward diagonal");

            } else if (angle >= 248 && angle < 292) {
            // Backward motion
            // Implement backward motor control
                pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
                pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(speed, 2))));
                pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
                pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
                printf("BACKWARDS\n");

            } else if (angle >= 292 && angle < 337) {
            //Diagonal right-backward motion
                pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
                pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(0, 2))));
                pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(0, 2))));
                pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
                printf("Left-Backward diagonal");

            } else if ((angle >= 337 && angle < 360) || (angle > 0 && angle < 22)){
            // Right strafe motion
            // Implement right strafe motor control
                pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
                pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(speed, 1))));
                pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
                pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
                printf("LEFT STRAFE\n");

            } else if (angle >= 22 && angle < 68){
            // diagonal right-forward motion
                pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(0, 1))));
                pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(speed, 1))));
                pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
                pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(0, 1))));
                printf("Left-Forward diagonal");

            }

        }
    
        else {
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(0, 1))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(0, 1))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(0, 1))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(0, 1))));
            printf("Standing still\n");
        }
    }

    else if(joystick_nr == false){
        if (speed > 10){
            if ((angle <= 90 && angle <= 0) || (angle >= 270 && angle <= 360)){
            //Right rotate?
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(speed, 2))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
            printf("RIGHT Rotate\n");
            } 
        else if (angle > 90 && angle <= 270){
            //Left rotate?
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(speed, 1))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(speed, 2))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(speed, 1))));
            printf("LEFT Rotate\n");
            }
        }
        else{
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(calculate_speed(mapValue(0, 1))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(calculate_speed(mapValue(0, 1))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(calculate_speed(mapValue(0, 1))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(calculate_speed(mapValue(0, 1))));
        }
    }
    
}

int mapValue(int value, int direction)
{
    int minValue = 50;
    int value1 = value * 0.4;
    if (direction == 1){
        int duty_cycle = minValue + value1;
        return duty_cycle;
    }
    else if (direction == 2){
        int duty_cycle = minValue - value1;
        return duty_cycle;
    }
    else{
        //duty cycle for standing still
        return 50;
    }
}
int calculate_speed(int duty_cycle){
    int number = 40000000;
    int speedy = ((duty_cycle * (number / 10)) / 10);
    return speedy;
}

