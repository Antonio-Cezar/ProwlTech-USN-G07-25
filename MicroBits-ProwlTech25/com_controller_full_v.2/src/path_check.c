
#include <stdio.h>
#include <stdbool.h>
#include "path_check.h"

struct path_check_results path_check(int angle, int speed, bool js_num, uint8_t sensorValues){
    //(FENRIS24) Lager en variabel som kan settes lik sensorene i retningen som skal testen. Se Masketabell for sensorer (tabell 9 i rapport)
    uint8_t testBits;

    //(FENRIS24) Variabelen som manipuleres og returneres fra funksjonen
    struct path_check_results forward_results;
    forward_results.angle = angle;
    forward_results.js_num = js_num;

    //(FENRIS24) Hvis sann er laterale bevegelser valgt
    if (js_num == true){
        
        //(FENRIS24) For å kunne kjøre skal joystick være minst 10% ut fra origo. 10% hastighet
        if (speed > 10){
        //(FENRIS24) Funksjon som tester om joysticken er satt i en fremover-kjørings posisjon
            if ((angle >= 337 && angle <= 360) || (angle >= 0 && angle < 23)) {
            
                printf("FORWARD\n");
                //(FENRIS24) Her må det bitsvis AND'es med 0001 (se tabell), så variabelen settes lik sensoren som sanser fremover
                testBits = (uint8_t)1;

                //(FENRIS24) Funksjon som tester om bitvis operasjonen returnerer en verdi større enn 0. Hvis dette skjer betyr det at sensoren sanser et objekt i retningen, og kjørekommandoen må blokkeres.
                if ((int)(sensorValues & testBits) > 0){
                    //(FENRIS24) fart settes lik null hvis sensor er blokkert
                    forward_results.speed = 0;
                    
                }
                else{
                    //(FENRIS24) fart settes lik den opprinnelige farten hvis sensor IKKE er blokkert
                    forward_results.speed = speed;
                    
                }

            } else if(angle >= 23 && angle < 67){
                //(FENRIS24) Diagonal kjøring fremover mot høyre
                printf("Right-Forward diagonal");
                testBits = (uint8_t)3;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if (angle >= 67 && angle < 112) {
                //(FENRIS24) Streifing mot høre
                printf("RIGHT STRAFE\n");
                testBits = (uint8_t)2;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if (angle >= 112 && angle < 157){
                //(FENRIS24) Diagonal kjøring bakover mot høyre
                printf("Right-Backward diagonal");
                testBits = (uint8_t)10;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if (angle >= 157 && angle < 202) {
                //(FENRIS24) Bakover kjøring
                printf("BACKWARDS\n");
                testBits = (uint8_t)8;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if (angle >= 202 && angle < 247) {
            //(FENRIS24) Diagonal bakover til venstre kjøring.
                printf("Left-Backward diagonal");
                testBits = (uint8_t)12;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if (angle >= 247 && angle < 292){
            //(FENRIS24) Vesntre streifing
                printf("LEFT STRAFE\n");
                testBits = (uint8_t)4;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = 0;
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }

            } else if (angle >= 292 && angle < 337){
            //(FENRIS24) diagonal kjøring fremover mot venstre
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
        else {
            //(FENRIS24) Hvis en vinkel utenfor 0-360 av en eller annen grunn sendes til funksjonen, skal bilen stå stille.
            printf("Standing still, no motion\n");
            forward_results.speed = 0;
            }
    
    }

    //(FENRIS24) Funksjon for å se om det er ønsket å rotere bilen
    else if(js_num == false){

        if (speed > 10){
            if (angle >= 20 && angle < 160){
            //(FENRIS24) Roter rundt egen akse mot høyre
                printf("RIGHT Rotate\n");
                testBits = (uint8_t)15;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = (int)(speed/2);
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }
            } 
        
        else if (angle > 200 && angle < 340){
            //(FENRIS24) Rotasjon rundt egen akse mot venstre
            
            printf("LEFT Rotate\n");
            testBits = (uint8_t)15;
                if ((int)(sensorValues & testBits) > 0){
                    forward_results.speed = (int)(speed/2);
                    
                }
                else{
                    forward_results.speed = speed;
                    
                }
                
            }
            else {
            printf("No rotation\n");
            forward_results.speed = 0;
            }
        
        }
        //(FENRIS24) Hvis joysticken er i vinkel mellom rotasjonsintervallene eller av en eller annen grunn gir verdier utenfor 0-360 grader.
        else {
            printf("Standing still, no rotation\n");
            forward_results.speed = 0;
            }
    }
    //(FENRIS24) returnerer kjørekommandoen etter test
    return forward_results;
}