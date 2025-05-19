#include "motorstyring.h"
#include "pwm.h" //for å kunne bruke pwm_write så motorene forstår signalene vi sender
#include <math.h> //for å få tilgang til mattefunksjoner
#ifndef M_PI //for å definere pi så vi kan bruke til vinkelberegning (er ikke definert i math.h)
#define M_PI 3.14159265358979323846
#endif
#include <stdio.h> // for å bruke printf og skrive ut
#include <stdlib.h> // for tilgang til minnehåndtering
#include <stdbool.h>
#define MOTOR_ID_FRONT_LEFT 0
#define MOTOR_ID_FRONT_RIGHT 1
#define MOTOR_ID_REAR_LEFT 2
#define MOTOR_ID_REAR_RIGHT 3

//laget av ras (husk å kommentere)
//Global flagg for nødstopp - under arbeid
static bool nødstopp = false;

void sett_nødstopp(bool aktivert) {
    nødstopp = aktivert;

    if (aktivert) {
        printf("nødstopp aktivert! alle motor stoppes.\n");
        int stopp_pwm = speed_to_ds(mapValue(0, 1));
        for (int i = 0; i < 4; ++i) {
            pwm_write(i, stopp_pwm);
        }

    } else {
        printf("Nødstopp deaktivert. Motorer aktive igjen.\n");
    }

}


bool er_nødstopp_aktivert(){
    return nødstopp;
}


//ny versjon for mapValue og pwm-verktøy

int mapValue (int prosent, int retning){
    int min = 50;
    int diff = prosent * 0.4f; // 0-100 - > 0-40

    if (retning == 1) return min + diff; //fremover
    if (retning == 2) return min - diff; // bakover
    return 50; // default setting
}

int speed_to_ds(int duty_cycle) {
    int periode = 40000000; // 40ms = 25 hz
    return ((duty_cycle * (periode / 10)) /10); //(duty cycle / 100) * periode

}

//funksjon som beregn motorverdier fra fart, vinkel og rotasjon. regner ut hvor mye hver av de 4 motorene skal kjøre.
//basert på fart(hvor fort vi vil kjøre fra 0 til 1), 
//vinkel(hvilken retning vi vil kjøre. for eks 0 er fremover og pi/2 er til venstre),
//rotasjon(rotasjon av hele bilen, positiv=høyre og negativ=venstre)

 struct MotorVerdier kalkulerMotorVerdier(float fart, float vinkel, float rotasjon) {
    float x = fart * cosf(vinkel); //her regner vi fart og vinkel om til x kordinater
    float y = fart * sinf(vinkel); //her regner vi fart og vinkel om til y kordinater

    float justert_rotasjon = rotasjon * fart; //gir dynamisk rotasjon

    float front_left = y + x + justert_rotasjon; //regner ut hvor mye hver motor må bidra for at hele bilen skal bevege seg riktig. 
    float front_right = y - x - justert_rotasjon; //mecanum hjulene kombinerer for å få dette til.
    float rear_left =  y - x + justert_rotasjon;
    float rear_right = y + x - justert_rotasjon;

    printf("beregnet motorverdier: front_left = %.2f, front_right = %.2f, rear_left = %.2f, rear_right = %.2f\n", front_left, front_right, rear_left, rear_right); //skriver ut det som har blitt beregnet, viktig info når man feilsøker.
   
    //hvis en av verdiene er større enn 1.0 deler vi alle på den største verdien, så de skaleres ned til mellom -1 og 1. dette er kalt normalisering.
    float max_val = fmaxf(fmaxf(fabsf(front_left), fabsf(front_right)), fmaxf(fabsf(rear_left), fabsf(rear_right)));

    if (max_val > 1.0f) {
        printf("Normaliserer motorverdier. Max verdi før: %.2f\n", max_val);
        front_left /= max_val;
        front_right /= max_val;
        rear_left /= max_val;
        rear_right /= max_val;
    }

    //sjekker kraft - feilsøkinger
    float gjennomsnitt = (fabsf(front_left) + fabsf(front_right) + fabs(rear_left) + fabs(rear_right)) / 4.0f;

    if (fabs(front_left) > gjennomsnitt * 1.5f)
        printf("Advarsel: front_left motor bruker mer kraft enn snittet\n");
    
    if (fabs(front_right) > gjennomsnitt * 1.5f)
        printf("Advarsel: front_right motor bruker mer kraft enn snittet\n");
    
    if (fabs(rear_left) > gjennomsnitt * 1.5f)
        printf("Advarsel: rear_left motor bruker mer kraft enn snittet\n");

    if (fabs(rear_right) > gjennomsnitt * 1.5f)
        printf("Advarsel: rear_right motor bruker mer kraft enn snittet\n");
    
    if (fabs(front_left) < 0.05f && gjennomsnitt > 0.2f)
        printf("Advarsel: front_left motor bidrar lite mens de andre jobber\n");
    
    if (fabs(front_right) < 0.05f && gjennomsnitt > 0.2f)
        printf("Advarsel: front_right motor bidrar lite mens de andre jobber\n");

    if (fabs(rear_left) < 0.05f && gjennomsnitt > 0.2f)
        printf("Advarsel: rear_left motor bidrar lite mens de andre jobber\n");
    
    if (fabs(rear_right) < 0.05f && gjennomsnitt > 0.2f)
        printf("Advarsel: rear_right motor bidrar lite mens de andre jobber\n");


    struct MotorVerdier motorVerdier = {front_left, front_right, rear_left, rear_right}; // struct med 4 motorverdiene og returnerer den.
    return motorVerdier;

}


 static float gjeldende_fart = 0.0f;
 static float gjeldende_vinkel = 0.0f;
 static float gjeldende_rotasjon = 0.0f;
 static const float endring_per_steg = 0.05f;

//Hovedfunksjonen som konverterer bevegelse til PWM-signal, denne blir kalt når motorene skal kjøre.


int kontroller_motorene(float ønsket_fart, float ønsket_vinkel, float ønsket_rotasjon) {
    printf("kontrollerer motorene. Fart: %.2f, Vinkel: %.2f, rotasjon: %.2f\n", ønsket_fart, ønsket_vinkel, ønsket_rotasjon); //printer ut hvilke kommandoer vi har fått

    if (nødstopp) {
        printf("Nødstopp aktiv - motorstyring stoppes.\n");
        return -1;
    }

    if (!isfinite(ønsket_fart) || !isfinite(ønsket_vinkel) || !isfinite(ønsket_rotasjon)) {
        printf("Feil: en eller flere inputverdier er uendelige (inf) eller NaN\n");
        return -1;
    }

    //sjekker om at vinklene som kommer inn ikke er rare. Vinkelen bør være mellom -pi og pi.
    if (ønsket_vinkel < -M_PI || ønsket_vinkel > M_PI) {
        printf("Vinkelen som kom inn er utenfor forventet område: %.2f rad\n", ønsket_vinkel);
    }

    if (ønsket_fart < 0.0f || ønsket_fart > 1.0f) {
        printf("Advarsel: Fart utenfor gyldig område (0.0-1.0): %.2f\n", ønsket_fart);
    }

    if (ønsket_rotasjon < -1.0f || ønsket_rotasjon > 1.0f) {
        printf("Advarsel: Rotasjon utenfor gyldig område (-1.0 - 1.0): %.2f\n", ønsket_rotasjon);

    }

    //Rotasjonssjekk - unngå overdominans av rotasjon

    if (fabs(ønsket_rotasjon) > ønsket_fart * 2.0f && ønsket_fart > 0.1f) {
        printf("Advarsel: Rotasjon dominerer over fart, det kan gi spin eller ubalanse\n");
    }

    // Jevn_sving: Oppdater fart, rotasjon og vinkel
    if (gjeldende_fart < ønsket_fart) {
        gjeldende_fart += endring_per_steg;
        if (gjeldende_fart > ønsket_fart) gjeldende_fart = ønsket_fart;
     } else if (gjeldende_fart > ønsket_fart) {
        gjeldende_fart -= endring_per_steg;
        if (gjeldende_fart < ønsket_fart) gjeldende_fart = ønsket_fart;
    }

    if (gjeldende_vinkel < ønsket_vinkel) {
        gjeldende_vinkel += endring_per_steg;
        if (gjeldende_vinkel > ønsket_vinkel) gjeldende_vinkel = ønsket_vinkel;
    } else if (gjeldende_vinkel > ønsket_vinkel) {
        gjeldende_vinkel -= endring_per_steg;
        if (gjeldende_vinkel < ønsket_vinkel) gjeldende_vinkel = ønsket_vinkel;
    }
    
    if (gjeldende_rotasjon < ønsket_rotasjon) {
        gjeldende_rotasjon += endring_per_steg;
        if (gjeldende_rotasjon > ønsket_rotasjon) gjeldende_rotasjon = ønsket_rotasjon;
    } else if ( gjeldende_rotasjon > ønsket_rotasjon) {
        gjeldende_rotasjon -= endring_per_steg;
        if (gjeldende_rotasjon < ønsket_rotasjon) gjeldende_rotasjon = ønsket_rotasjon;
    }

    //jevn stopp, når fart og rotasjon går mot null
    if (ønsket_fart < 0.01f && fabsf(ønsket_rotasjon) < 0.01f && (gjeldende_fart > 0.0f || fabsf(gjeldende_rotasjon) > 0.0f)) {
        printf("Ønsket fart og rotasjon går mot null, bremser jevnt\n");
        ønsket_fart = 0.0f;
        ønsket_rotasjon = 0.0f;
        gjeldende_vinkel = gjeldende_vinkel; //vinkelen beholdes under brems
    }

    // stopp motorene helt hvis alt er 0

    if (gjeldende_fart < 0.01f && fabsf(gjeldende_rotasjon) < 0.01f) { // Hvis det er ingen bevegelse eller rotasjon så skal motorene stoppes.
        printf("Motorene står stille. Fart: %.2f, rotasjon: %.2f\n", gjeldende_fart, gjeldende_rotasjon);
        int mapped = mapValue(0, 1);
        int pwm_duty = speed_to_ds(mapped);
        for (int i = 0; i < 4; ++i) {
            printf("Stopp: Motor %d, mapValue %d, PWM-verdi: %d\n", i, mapped, pwm_duty);
            pwm_write(i, pwm_duty);
            
        }
        printf("Står stille\n");
        return 0;
    }

    printf("motorene beveger seg jevnt. Fart: %.2f, vinkel: %.2f, rotasjon: %.2f\n", gjeldende_fart, gjeldende_vinkel, gjeldende_rotasjon);

    struct MotorVerdier motorVerdier = kalkulerMotorVerdier(gjeldende_fart, gjeldende_vinkel, gjeldende_rotasjon); //kalkulerer hva alle 4 motorene skal gjøre basert på fart,vinkel og rotasjon.
    float motorer[4] = { //legger verdiene i en array så samme gjøres for hver motor
        motorVerdier.front_left,
        motorVerdier.front_right,
        motorVerdier.rear_left,
        motorVerdier.rear_right

    };



    int pins[4] = {MOTOR_FRONT_LEFT, MOTOR_FRONT_RIGHT, MOTOR_REAR_LEFT, MOTOR_REAR_RIGHT}; //hver motor er koblet til en pin, definert i pwm.h

    

    for (int i = 0; i < 4; ++i) { //for løkke som går en gang per motor
        if (pins[i] < 0 || pins[i] > 10) {
            printf("Feil: ugyldig motorpin: %d for motor %d\n", pins[i], i);
            continue;
        }
    

        int retning = (motorer[i] >= 0.0f) ? 1 : 2; //hvis verdien er positiv er retningen fremover(1) og bakover hvis den er negativ(2).
        float fartsverdi = fabsf(motorer[i]) * 100.0f; //0-100% (gjør verdien om til prosent)

        int mapped = mapValue((int)fartsverdi, retning); //konverterer fart + retning til pwm duty-cycle. (hvor mye strøm motoren skal få)
        if (mapped < 0) {
            printf("Feil: mapvalue() returnerte negativ verdi: %d for motor %d\n", mapped, i);
        }

        int pwm_duty = speed_to_ds(mapped); //gjør duty-cycle om til tidsverdi i nanosekunder som pwm_write() kan bruke.
        if (pwm_duty < 0) {
            printf("Feil: speed_to_Ds() returnerte negativ verdi: %d for motor %d\n", pwm_duty, i);
        }

        printf("Motor: %d, Retning: %d, Fart: %.2f, mapValue: %d, PWM-verdi: %d\n", i, retning, fartsverdi, mapped, pwm_duty);
        pwm_write(pins[i], pwm_duty); //sender signalet til riktig motor
    }

    printf("Kjører fart: %.2f, vinkel: %.2f rad, rotasjon: %.2f\n", gjeldende_fart, gjeldende_vinkel, gjeldende_rotasjon);
    return 0;
}


//TODO under testing: -Prøve dynamisk endring_per_steg, -legg til toleranse på små forskjeller, -juster svingrespons
