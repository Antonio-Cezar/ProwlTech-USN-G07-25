#include "motorstyring.h"
#include "vesc_uart.h"
#include <stdio.h>
#include <math.h> //for å få tilgang til mattefunksjoner som cos(),sin(),fmaxf() og fabsf()
#include <zephyr/kernel.h> //for å få tilgang til zephyr sine funksjoner k_busy_wait() og k_msleep(), brukes til venting og pauser

//MOTORSTYRING.C  ER KODET AV RAMEEZ AHMED SADIQ (PROWLTECH2025)

#define NUM_MOTORS 4 //definerer antall motorer på bilen
#define MAX_RPM 20000  // definerer at motorene kan maks rotere 20 000 rpm

//Funksjon som regner ut hvor mye hver motor skal bidra med for å oppnå ønsket fart, vinkel, rotasjon og sving
struct MotorVerdier kalkulerMotorVerdier(

    //hvor fort bilen skal bevege seg. 
    // 0.0 = full stopp
    // 1.0 = maks fart 
    float fart,

    //hvilken retning bilen skal bevege seg i (0 til 2pi radianer) 
    // 0 = rett frem
    // pi/2 = mot venstre
    // pi =  bakover
    // 3pi/2 =  mot høyre
    float vinkel,

    //Den rotasjonen vi ønsker rundt egen akse
    // -1.0 = roter mot venstre
    // 0.0 = ingen rotasjon
    // 1.0 = roter mot høyre
    float rotasjon,

    //ekstra finjustering for sving fra en egen styrespak, for at bilen skal svinge som en ekte bil.
    float sving_js){ 

    //Hvis bilen står stille og RB/LB trykkes så vil bilen rotere med en fast hastighet.
    //trykkes en annen kjøreknapp så stoppes rotasjonen.
    if (fart == 0.0f && rotasjon != 0.0f) {

        //bruker 0.3f fordi maks rpm er satt til 20tusen, ved å bruke 0.3 * 20tusen
        //så blir rotasjonsfarta 6tusen rpm, fast og kontrollert.
        float rotering;
        if (rotasjon > 0) {
            rotering = 0.3f;
        } else {
            rotering = -0.3f;
        } 

        return (struct MotorVerdier) {

            //foran venstre (går bakover, negativ retning)
            -rotering, 

            // foran høyre (går fremover, positiv retning)
            rotering, 

            //bak venstre (går bakover, negativ retning)
            -rotering,
            
            //bak høyre (går fremover, positiv retning)
            rotering

            
        };
    }

    
    //regner ut hvor mye bevegelse som skal være i X-retning (sideveis)
    //Omgjør fart og vinkel til en x-verdi
    //sinf(vinkel) gir hvor mye som skal gå i X-retning
    float x = fart * sinf(vinkel);

    //regner ut hvor mye bevegelse som skal være i Y-retning (fremover/bakover)
    //Omgjør fart og vinkel til en Y-verdi
    //cosf(vinkel) gir hvor mye som skal gå i Y-retning
    // X og Y tilsammen er en bevegelsesvektor som
    float y = fart * cosf(vinkel);

    //for å legge til ekstra sidebevegelse (sving) fra joystick for svinging
    //sving_js ganges med 0.5f for å unngå for brå/kraftig svingeffekt.
    x += sving_js * 0.5f;

    //for å regne ut hvor mye bidrag hvert hjul må tilføre for å kunne bevege fremover, bakover eller sideveis
    //verdiene y og x er fremdrift og sidebevegelse

    //foran venstre (kombinerer retning fremover og høyre bevegelse)
    float foran_venstre = y + x;

    //foran høyre hjul (kombinerer retning fremover og venstre bevegelse)
    float foran_høyre = y - x;

    //bak_venstre (kombinerer retning bakover og venstre bevegelse)
    float bak_venstre = y - x;

    //bak høyre (kombinerer retning bakover og høyre bevegelse)
    float bak_høyre = y + x;
    

    //Etter at det har blitt beregnet hvor mye hvert hjul må bidra med er det viktig å normalisere verdiene
    //normaliserer verdiene for å unngå at noen av verdiene overstiger -0.1 eller 0.1
    //overstiger de dette kan det føre til uforventet bevegelser

    //finner den største absoluttverdien av motorverdiene.
    //bruker funksjonen fmaxf som kan sammenligne to verdier om gangen
    //bruker fmaxf på de to hjulene foran for å finne den største verdien av de to hjulene foran
    //bruker fmaxf på de to hjulene bak for å finne den største verdien av de to hjulene bak
    //bruker deretter fmaxf på begge fmaxf verdiene for å finne hvilken verdi 
    //som er størst av den største foran og største bak
    //den største verdien av de fire legges deretter i "maxverdi"
    float maksverdi = fmaxf(
        fmaxf(fabsf(foran_venstre), fabsf(foran_høyre)), fmaxf(fabsf(bak_venstre), fabsf(bak_høyre)));

    //hvis maksverdi er større enn 1.0f må vi skalere og dette gjøres ved å dele alle 
    //hjulverdiene på maksverdien vi fant. 
    if (maksverdi > 1.0f) {
        foran_venstre /= maksverdi;
        foran_høyre /= maksverdi;
        bak_venstre /= maksverdi;
        bak_høyre /= maksverdi;
    }

    //returnerer de beregnede verdiene vi fikk etter normalisering som en struct
    //og har sikret at ingen av hjulverdiene overstiger 1.0.
    return (struct MotorVerdier) {
        foran_venstre,
        foran_høyre,
        bak_venstre,
        bak_høyre
    };
}

//lager en statisk variabel som husker verdien mellom hver gang funksjonen kalles og brukes til
//å gjøre overgangen mykere ved å gradvis nærme seg ønsket fart slik at 
//bilen ikke rykker, bråstopper eller bråstarter når joystick-verdier endres.
static float gjeldende_fart = 0.0f;

//samme prinsipp som nevnt over
static float gjeldende_vinkel = 0.0f;

//samme prinsipp som nevnt over
static float gjeldende_sving_js = 0.0f;

//for å angi hvor mye verdiene skal endres gradvis per funksjons kall
static const float endring = 0.2f;

//hovedfunksjonen som styrer motorene basert på joystick-verdier i form av "ønsket_"
int kontroller_motorene(float ønsket_fart, float ønsket_vinkel, float ønsket_rotasjon, float sving_js) {
    
    //skriver ut alle joystick-verdier i sanntid for å overvåke.
    printf("fart=%.2f, vinkel=%.2f, rotasjon=%.2f, sving=%.2f\n", ønsket_fart, ønsket_vinkel, ønsket_rotasjon, sving_js);

    
    //Her sjekkes det om gjeldende_fart er lavere enn ønsket_fart
    if (gjeldende_fart < ønsket_fart)

        //hvis gjeldende_fart er lavere enn ønsket_fart, økes farten gradvis med "endring", for å gi en mykere akselerasjon
        //farten økes med 0.2f og deretter blir fminf funksjonen for å sjekke hvem av gjeldende_fart+endring
        //og ønsket_fart er minst. gjeldende_fart lagrer verdien til den minste for å unngå at den overstiger 
        //ønsket_fart, i siste steg.
        gjeldende_fart = fminf(gjeldende_fart + endring, ønsket_fart);

        //sjekkes om gjeldende_fart er større enn ønsket_fart
    else if (gjeldende_fart > ønsket_fart)

        //bruker fmaxf funksjonen til å lagre den største farta av gjeldende_fart-endring og ønsket_fart
        //det gjøres hvis farta er høyere enn den joystick-verdien bruker sender så det gradvis
        //går ned mot ønsket fart og unngå å overstige ønsket_fart i siste steg
        gjeldende_fart = fmaxf(gjeldende_fart - endring, ønsket_fart);
    
    //sjekker om gjeldende_vinkel er lavere enn ønsket_vinkel
    //hvis den er det, økes vinkelen litt og litt med "endring" som er 0.2f for å gi en jevn overgang
    //dette gir en gradvis dreining i stede for brå endring i retning
     if (gjeldende_vinkel < ønsket_vinkel)
        gjeldende_vinkel += endring;
    
    //hvis gjeldende_vinkel er høyere enn ønsket_vinkel, reduseres den litt og litt
    //dette gjør at bilen roterer jevnt tilbake i motsatt retning
    else if (gjeldende_vinkel > ønsket_vinkel)
         gjeldende_vinkel -= endring;


    //sjekker om gjeldende_sving_js er lavere enn verdien fra joystick-sving
    //hvis den er det, så økes verdien gradvis for å få en myk og kontrollert sving
    //ungår at bilen plutselig rykker seg til siden
    if (gjeldende_sving_js < sving_js)
        gjeldende_sving_js += endring;


    //hvis gjeldende_sving_js er høyere enn sving_js blir den redusert gradvis
    //dette gir en jevn tilbakeføring når bruker slipper joystick spaken tilbake til midten
    else if (gjeldende_sving_js > sving_js)
        gjeldende_sving_js -= endring;  

    //hvis bilen står stille og bruker ønsker rotasjon og trykker eller holder rotasjonknapp inne
    if (gjeldende_fart == 0.0f && ønsket_rotasjon != 0.0f){

        //hvis ønsket_rotasjon er større enn 0 så roterer bilen rolig mot høyre
        //hvis ønsket_rotasjon er mindre enn 0 så roterer bilen rolig mot venstre
        //dette fordi negativ verdi gir rotasjon mot venstre og
        //positiv verdi gir  mot høyre
        float rotering;
        if (ønsket_rotasjon > 0) {
            rotering = 0.3f;
        } else {
            rotering = -0.3f;
        }

        //definerer hvordan hvert hjul skal rotere for å få rotasjon rundt sin egen akse
        //hvis verdi er 0.3 får vi hjulverdiene -0.3, 0.3, -0.3, 0.3 som gir rotasjon mot høyre
        //Hvis verdi er -0.3 får vi hjulverdiene 0.3, -0.3, 0.3, -0.3 som gir rotasjon mot venstre
        struct MotorVerdier rotasjon_motorverdier = {
            -rotering, //foran venstre
             rotering, //foran høyre
            -rotering, //bak venstre
             rotering  // bak høyre

        };
    
        //gjør motorbidrag om til faktisk RPM-verdier fra -1.0 til 1.0
        //Bruker definert max rpm for å skalere. at for eks hvis motorbidrag er 0.3, blir rpm verdien 6000 rpm
        //alle verdier typecastes til int32_t fordi vesc_protokollen bruker heltall for RPM
        //denne er kun for rotasjon og sjekkes først fordi den kjøres fra stillestående tilstand
        int32_t rpm[NUM_MOTORS] = {
            (int32_t)(rotasjon_motorverdier.foran_venstre * MAX_RPM),
            (int32_t)(rotasjon_motorverdier.foran_høyre * MAX_RPM),
            (int32_t)(rotasjon_motorverdier.bak_venstre * MAX_RPM),
            (int32_t)(rotasjon_motorverdier.bak_høyre * MAX_RPM),
        
        };
        
        //lager en array med ID for hver VESC-enhet. 
        //0 er master enheten som er koblet direkte til microbit via UART
        //1,2,3 er slave-enheter som styres via CAN-forwarding
        uint8_t vesc_id[NUM_MOTORS] = {0, 1, 2, 3};

        //for løkke som går igjennom alle motorene en etter en
        for (int i = 0; i < NUM_MOTORS; i++) {

            //hvis vesc_id[i] er 0 så sendes rpm med "send_rpm()"-funksjonen 
            //send-rpm() sender vesc-pakken som er ment for master-enheten
            //fordi 0 er master-enhet
            if(vesc_id[i] == 0) {
                send_rpm(rpm[i]);

            //hvis ID ikke er 0 så brukes send_rpm_videre funksjonen
            //fordi den sender vesc-pakken som er ment for slave-enhetene
            } else {
                send_rpm_videre(vesc_id[i], rpm[i]);
            }

            //liten pause mellom hver sending for å unngå at VESC-enhetene blir overbelastet
            k_msleep(1);

        }    
        //returnerer 0 så det blir bekreftet at funksjonen kjørte uten feil
        return 0;
    }

//Her kalles funksjonen kalkulerMotorVerdier() for å beregne hvor mye hver motor skal bidra
//gjeldende_fart og gjeldende_vinkel brukes for å styre retning og fart
//rotasjon blir satt til 0 fordi bilen ikke skal rotere rundt egen akse i denne delen
//gjeldende sving_js brukes for å legge til jevn sving fra joystick
struct MotorVerdier gjeldendeMotorVerdier = kalkulerMotorVerdier(
    gjeldende_fart,
    gjeldende_vinkel,
    0.0f,
    gjeldende_sving_js
);

//gjør motorbidrag om til faktisk RPM-verdier fra -1.0 til 1.0
//Bruker definert max rpm for å skalere. Hvis motorbidrag er 0.3, blir rpm verdien 6000 rpm
//alle verdier typecastes til int32_t fordi vesc_protokollen bruker heltall for RPM
//denne brukes under vanlig bevegelse basert på joystick-input
int32_t rpm_gjeldende[NUM_MOTORS] = {
        (int32_t)(gjeldendeMotorVerdier.foran_venstre * MAX_RPM),
        (int32_t)(gjeldendeMotorVerdier.foran_høyre * MAX_RPM),
        (int32_t)(gjeldendeMotorVerdier.bak_venstre * MAX_RPM),
        (int32_t)(gjeldendeMotorVerdier.bak_høyre * MAX_RPM),
        
};

//lager en array med ID for hver VESC-enhet. 
//0 er master enheten som er koblet direkte til microbit via UART
//1,2,3 er slave-enheter som styres via CAN-forwarding
uint8_t vesc_id_gjeldende[NUM_MOTORS] = {0, 1, 2, 3};

//for løkke som går igjennom alle motorene en etter en
for (int i = 0; i < NUM_MOTORS; i++) {

    //hvis vesc_id[i] er 0 så sendes rpm med "send_rpm()"-funksjonen 
    //send-rpm() sender vesc-pakken som er ment for master-enheten
    //fordi 0 er master-enhet
    if(vesc_id_gjeldende[i] == 0) {
        send_rpm(rpm_gjeldende[i]);
    
    //hvis ID ikke er 0 så brukes send_rpm_videre funksjonen
    //fordi den sender vesc-pakken som er ment for slave-enhetene
    } else {
        send_rpm_videre(vesc_id_gjeldende[i], rpm_gjeldende[i]);
        }

        //liten pause mellom hver sending for å unngå at VESC-enhetene blir overbelastet
        k_msleep(1);

    }   
    //returnerer 0 så det blir bekreftet at funksjonen kjørte uten feil 
    return 0;
}

