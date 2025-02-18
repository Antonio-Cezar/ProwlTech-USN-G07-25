#include "pwm.h"
#include "motorkontroller.h"
#include <stdio.h>
#include <stdbool.h>


// Hele dette biblioteket er laget av Erik-Andre Hegna.
int control_motors(int angle, int speed, bool joystick_nr)
{
    // Funksjon som styrer motorene basert på joystickdata som er sendt fra kommunikasjonskontroller.
    // Den tar inn joystickens vinkel i forhold til senter, avstand mellom senter og joystick som fart, og hvilken joystick som brukes.
    
    if (joystick_nr == true){
        //Joystick_nr er en bool som indikerer om brukeren ønsker laterale eller rotasjons-bevegelser.
        //Hvis "true", velges laterale bevegelser
        
        if (speed > 10){
        //Et krav om at joystick må være minst 10% unna origo for å kjøre
        //Denne er viktig med tanke på joystickens dødssone, og skal gjøre det umulig å kjøre hvis joystick står i senter.
        
        //Videre sjekkes det kontinuerlig etterhvilken vinkel joystickens x og y punkt ligger i. 
        // Det er denne testen som velger de ulike motor-koordineringene.
        //Hver motor-koordinering er definert av hvilken vinkel punktet har på en sirkel. Så "angle" ligger alltid mellom 0 og 360.
        if (angle >= 68 && angle < 113) {
            // Vinkel for kjøring: fremover (90-22 < angle < 90+23)
            // Denne vinkelen betyr at joysticken er rettet fremover, og farts-kontrolleren får signal om å kjøre alle 4 motorene fremover.
            // Pwm_write er en selv-bygd funksjon fra pwm.h biblioteket. Funksjonen trenger å vite hvilken motor_pin signalet skal genereres på,
            // og hvilken verdi signalet skal ha. Perioden er definert et annet sted i koden, så verdien til signalet må være skalert i forhold
            // til perioden definert. Valgt motor-koordinering printes også til seriell monitor.
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(speed, 1))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
            printf("FORWARD\n");

        } else if(angle >= 113 && angle < 158){
            // Vinkel for kjøring: diagonal høyre-fremover (135-22 < angle < 135+23)
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(0, 1))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
            printf("Right-Forward diagonal");

        } else if (angle >= 158 && angle < 203) {
            // Vinkel for kjøring: streifing mot høyre (180-22 < angle < 180+23)
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(speed, 2))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
            printf("RIGHT STRAFE\n");

        } else if (angle >= 203 && angle < 248){
            // Vinkel for kjøring diagonal høyre-bakover (225-22 < angle < 225+23)
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(0, 2))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(speed, 2))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(0, 2))));
            printf("Right-Backward diagonal");

        } else if (angle >= 248 && angle < 292) {
            // Vinkel for kjøring bakover (270-22 < angle < 270+23)
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(speed, 2))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
            printf("BACKWARDS\n");

        } else if (angle >= 292 && angle < 337) {
            // Vinkel for kjøring diagonalt venstre-bakover (315-22 < angle < 315+23)
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(0, 2))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(0, 2))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
            printf("Left-Backward diagonal");

        } else if ((angle >= 337 && angle < 360) || (angle > 0 && angle < 22)){
            // Vinkel for streifing mot venstre ((360-22 < angle < 360) && (0 < angle < 0+23))
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(speed, 1))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
            printf("LEFT STRAFE\n");

        } else if (angle >= 22 && angle < 68){
            // Vinkel for kjøring diagonalt venstre-fremover (45-22 < angle 45+23)
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(speed, 1))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
            printf("Left-Forward diagonal");

        }

    }
    
    else {
        // Dette er else-funksjon for farten. Hvis farten er under 10% settes alle motorene til
        // hastighet lik 0.
        pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
        pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(0, 1))));
        pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
        pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
        printf("Standing still\n");
    }

    
    return 0;
    }

    // Dette er en else if-funksjon som velges hvis kontrolleren ønsker at bilen skal rotere.
    // Her har brukeren av kontrolleren trykket inn joysticken for å endre fra laterale
    // bevegelser til rotasjonsbevegelser.
    // Kontrolleren alternerer mellom true og false for hvert trykk på joysticken.
    else if(joystick_nr == false){
        //Ny test for å bekrefte at joystick ikke står i senter for å kunne kjøre.
        if (speed > 10){
            //Ny test for å se hvilken vinkel joysticken har. Her er område for hver motor-koordiering
            // økt fra 45 grader til 180 grader. Det er bare høyre og venstre rotasjon som kan
            // velges når joystick_nr er "false". 
            if ((angle <= 90 && angle >= 0) || (angle >= 270 && angle <= 360)){
                //Vinkel for rotering rundt bilens akse mot høyre. 1 og 4 kvadrant på enhetssirkelen/joysticken.
                pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
                pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(speed, 2))));
                pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
                pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
                printf("RIGHT Rotate\n");
            } 
            else if (angle > 90 && angle < 270){
                //Vinkel for rotering rundt bilens akse mot venstre. 2 og 3 kvadrant på enhetssirkelen/joysticken.
                pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
                pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(speed, 1))));
                pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(speed, 2))));
                pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(speed, 1))));
                printf("LEFT Rotate\n");
                }
            else {
                // Hvis vinkelene av en eller annen grunn faller utenfor intervallet 0 til 360 skal bilen stoppe.
                pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
                pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(0, 1))));
                pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
                pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
                printf("Standing still\n");
                }
        }
        else {
            // Hvis hastigheten (lengde fra origo på js) på rotasjon er mindre enn 10% skal bilen stoppe.
            // Denne funksjonen stopper også rotasjonen når system-bruker flytter js tilbake til dødsone område.
            pwm_write(MOTOR_FRONT_LEFT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
            pwm_write(MOTOR_FRONT_RIGHT,(uint32_t)(speed_to_ds(mapValue(0, 1))));
            pwm_write(MOTOR_REAR_LEFT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
            pwm_write(MOTOR_REAR_RIGHT, (uint32_t)(speed_to_ds(mapValue(0, 1))));
            printf("Standing still\n");
        }
        return 0;
    }
    

    
    return 0;
}

// Dette er funksjonen som skalerer ønsket kjørehastighet som et forhold til perioden.
// Det er denne funksjonen som gjør farts-vektoren lesbar for pwm_write()-funksjonen.
// Tallet som kommer ut ifra denne funksjonen sier hvor lenge PWM signalet skal være høyt.
// Tallet som går inn i funksjonen er ønsket duty-cycle i prosent.
int speed_to_ds(int duty_cycle){
    // number er det samme som perioden i nano sekunder. 40 000 000 ns = 0.04 s
    // perioden er definert i PWM bibliotek, men hvis den skal endre må også number
    // i denne funksjonen endres til samme verdi.
    int number = 40000000; 
    
    // speedy er et tilfeldig navn som ble med i det ferdige produktet. 
    // Den regner ut hvor mange nano sekunder PWM signalet må være høyt
    // i forhold til perioden. Matematikken som brukes er vanlig prosent
    // regning. Verdien deles på totalt 100, men den deles på 10 2 ganger 
    // for å unngå avrundingsfeil og at tallet skal bli for stort for en INT verdi.
    // duty_cycle * number kan føre til udefinerbar stor verdi for INT,
    // og deling på 100 kan vi avrudingsfeil mtp at desimaltall ikke brukes.
    int speedy = ((duty_cycle * (number / 10)) / 10);

    //Antall nano sekunder PWM signalet skal være høyt returneres som et heltall.
    return speedy;
}

// Siden motorene skal kunne gå både bakover og fremover er det viktig å finne duty_cycle
// som passer til begge scenarioene. Siden vi allerede vet hvilken motor som skal gå bakover 
// eller fremover basert på vinkel, er funksjonen bygd opp slik at den tar inn ønsket hastighet (value)
// og et heltall (direction) som er enten 1 eller 2 som definerer hvilken vei det er ønsket at motoren skal gå.
// Funksjonen gjør det også mulig å bestemme hvilke duty-cycles som er mulig å lage vha skalering.
int mapValue(int value, int direction)
{
    //Et PWM-signal med duty_cycle 50 betyr at motoren står i ro. Denne verdien trengs i utregningen.
    int minValue = 50;

    // value1 er en skalert verdi av ønsket hastighet. Ved å skalere med 0.4 vil ønsket motorhastighet
    // fra 0 til 100 skaleres ned som et tall mellom 0 og 40. Ved å enten summere eller subtrahere midtverdi med skalert verdi
    // er det mulig å få verdier mellom 10 og 90. Verdien kan skaleres til et mindre tall, men det er viktig
    // at skalerings-verdien noensinne er større enn 0.5!
    int value1 = value * 0.4;

    //Hvis direction er 1 betyr det at det er ønsket å lage en duty_cycle verdi som vil kjøre motoren fremover.
    if (direction == 1){

        //Her summeres minValue med skalert hastighets-verdi
        int duty_cycle = minValue + value1;

        //Ny utregnet duty cycle returneres. Verdien gir motor-rotsjon fremover.
        return duty_cycle;
    }

    //Hvis direction er 2 betur det at det er ønsket å lage en duty_cycle verdi som vil kjøre motoren bakover.
    else if (direction == 2){

        //Her subtraheres minValue med skalert hastighets-verdi.
        int duty_cycle = minValue - value1;

        //NY utregnet duty cycle returneres. Verdien gir motor-rotsjon bakover.
        return duty_cycle;
    }

    // Denne else funksjonen er et ekstra sikkerhetslag hvis mapValue funksjonen
    // får uforutsett data inn.
    else{
        
        //Duty_cycle blir da lik 50, som betyr at motoren skal stå i ro.
        return 50;
    }
}

