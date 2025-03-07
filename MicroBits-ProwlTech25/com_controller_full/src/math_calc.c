#include <math.h>
#include "math_calc.h"
//(PROWLTECH25) Commented and coded by CA & RS 

// (FENRIS24) Funksjonen tar inn rå potmeter verdi sendt fra kontroller (0 til 1023)
struct ProjectionResult stereographic_projection_2D(int x, int y){
    // (FENRIS24) Trekker fra 512 (1023/2) fra x og y. Må finne midtpunkt og ha muligheten til å ha negative akse-verdier for å senere bruke verdier i enhetssirkel.
    int unit_x = x - 512;
    int unit_y = y - 512;

    // (FENRIS24) Finner forholdet mellom x og y verdi
    double a = (double)y / (double)x;

    // (FENRIS24) Trigonometrisk formel for å finne x i en sirkel vha forholdet mellom x og y i en firkant
    double temp_x = 1 / sqrt(1 + (a * a));

    // (FENRIS24) Finner y-verdi ved å bruke forholdet mellom x og y i en firkant, og multipliserer med ny x-verdi
    double temp_y = a * temp_x;
    
    // (FENRIS24) lager en variabel som kan holde resultatene fra funksjonen
    struct ProjectionResult result;

    //(FENRIS24)  Oppdaterer variablen med resultatene utregnet i funksjonen
    result.x = (int)(temp_x * unit_x);
    result.y = (int)(temp_y * unit_y);

    // (FENRIS24) Funksjon som sjekker etter fortegnsfeil etter omforming av verdier
    if ((unit_y > 29 && result.y < 29) || (unit_y < -29 && result.y > 29)){
        result.y = (result.y * -1);
    }
    
    // (FENRIS24) Returnerer variablen
    return result;
}

// (FENRIS24) Denne funksjonen på få inn stereografisk projeksjonerte x og y verdier
int calculate_angle_degrees(int x, int y){
    // (FENRIS24) Lager variabler som lagrer x og y midlertidig
    int temp_x = x;
    int temp_y = y;

    // (FENRIS24) Funksjon for å definere dødsonen til joystick i x-retning
    if ((-40 < x) && (x < 41)){
        temp_x = 0;
    }
    //(FENRIS24)  Funksjon for å definere dødsone til joystick i y-retning
    if ((-28 < y) && (y < 29)){
        temp_y = 0;
    }

    // (FENRIS24) Sikkerhetslag i tilfelle joystick-verdiene kommer utenfor maks område i negativ og positiv retning
    if (x > 512){temp_x = 512;}
    if (x < -512){temp_x = -512;}
    if (y > 512){temp_y = 512;}
    if (y < -512){temp_y = -512;}

    // (FENRIS24) Lager variabel som skal holde vinkelen målt i radianer. Variabelen settes lik resultatet av en atan2 funksjon med x og y som parametere
    double angle_radians = atan2(temp_x, temp_y);

    //(FENRIS24)  Lager variabel som skal holde vinkel målt i grader. Variabelen settes lik radianer utregnet omgjort til vinkel
    double angle_degrees = ((angle_radians * 180) / 3.14159);

    //(FENRIS24) atan2 gir vinkel fra -180 til 180 grader. Denne funksjonen omgjør negativ vinkel til vinkler over 180 grader. Slik at 0-360 grader oppnåes.
    if (angle_degrees < 0){angle_degrees += 360.0;}

    // (FENRIS24)  returnerer utregnet vinkel
    return (int)angle_degrees;
}

// (FENRIS24) Funksjon som regner på punkt(x,y) sin lengde fra origo. Bruker vanlig pytagoras setning h = sqrt(k1*k1 + k2*k2)
int calculate_radius(int x, int y){
    // (FENRIS24) lager variabler midlertidig for å holde verdiene.
    int temp_x = x;
    int temp_y = y;

    // (FENRIS24) Sjekk for å definere dødsone fra joystick
    if ((-40 < x) && (x < 40)){
        temp_x = 0;
    }
    if ((-40 < y) && (y < 40)){
        temp_y = 0;
    }

    // (FENRIS24) lager variabel som holder resultatet utregnet etter pytagoras sin setning
    double radius = sqrt((temp_x * temp_x) + (temp_y * temp_y));

    // (FENRIS24) Returnerer punktets lengde fra origo
    return (int)radius;
}


int scale_to_percent(int number, int max_value){
    // (FENRIS24) Lager variabel som holder det skalerte tallet etter prosentutregning.
    double scaled_number = ((double)number / (double)max_value) * 100.0;

    // (FENRIS24)  Returnerer tall som beskriver hvor stort number er iforhold til max_value
    return (int)(scaled_number);
}
