
#include <stdio.h>
#include <stdbool.h>
#include "path_check.h"

struct path_check_results path_check(int angle, int speed, bool js_num, uint8_t sensorValues){
    uint8_t testBits;
    struct path_check_results forward_results;
    forward_results.angle = angle;
    forward_results.js_num = js_num;
    if (js_num == true){
        
        if (speed > 10){
        // Determine motor control based on angle
            if (angle >= 68 && angle < 113) {
            // Forward motion
            // Adjust speed based on joystick radius
                printf("FORWARD\n");
                testBits = (uint8_t)1;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if(angle >= 113 && angle < 158){
            // Diagonal Right-forward motion
                printf("Right-Forward diagonal");
                testBits = (uint8_t)3;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if (angle >= 158 && angle < 203) {
            // Right strafe motion
                printf("RIGHT STRAFE\n");
                testBits = (uint8_t)2;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if (angle >= 203 && angle < 248){
            //Diagonal right-backward motion
                printf("Right-Backward diagonal");
                testBits = (uint8_t)10;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if (angle >= 248 && angle < 292) {
            // Backward motion
            // Implement backward motor control
                printf("BACKWARDS\n");
                testBits = (uint8_t)8;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if (angle >= 292 && angle < 337) {
            //Diagonal left-backward motion
                printf("Left-Backward diagonal");
                testBits = (uint8_t)12;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if ((angle >= 337 && angle < 360) || (angle > 0 && angle < 22)){
            // Left strafe motion
            // Implement left strafe motor control
                printf("LEFT STRAFE\n");
                testBits = (uint8_t)4;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if (angle >= 22 && angle < 68){
            // diagonal left-forward motion
                printf("Left-Forward diagonal");
                testBits = (uint8_t)5;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }
            }
        }
    
    }

    else if(js_num == false){
        if (speed > 10){
            if ((angle <= 90 && angle <= 0) || (angle >= 270 && angle <= 360)){
            //Right rotate?
                printf("RIGHT Rotate\n");
                testBits = (uint8_t)15;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = (int)(speed/2);
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }
            } 
        
        else if (angle > 90 && angle <= 270){
            //Left rotate?
            
            printf("LEFT Rotate\n");
            testBits = (uint8_t)15;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = (int)(speed/2);
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }
            }
        
        }
    }
    return forward_results;
}