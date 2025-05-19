#include "vesc_uart.h" 
#include <stdio.h>
#include <string.h>
#include <zephyr/drivers/uart.h> // for å få tilgang til zephyr sin UART-driver og UART-funksjoner som uart_poll_out(), funksjonen sender en og en byte via UART
#include <zephyr/kernel.h> //for å få tilgang til zephyr sine funksjoner k_busy_wait() og k_msleep(), brukes til venting og pauser
#include <zephyr/device.h> //for å få tilgang til zephyr sine funksjoner DEVICE_DT_GET() og device_is_ready(), henter og sjekker status på enheter
#include <zephyr/devicetree.h> //for å få tilgang til DT_NODELABEL(), som brukes for å hente uart konfigurasjonen fra devicetreet

#define UART_NODE_LABEL DT_NODELABEL(uart1) //zephyr ser i devicetree og i min app.overlay fil etter en node som heter "uart1" og lagrer referansen i UART_NODELABEL

//HELE VESC_UART.C ER SKREVET AV (RAMEEZ SADIQ PROWLTECH)

//Lager en peker og kalt uart1_enhet, den skal holde på UART-enheten. Den starter som null og får verdi senere av DEVICE_DT_GET
//uart1_enhet er
static const struct device *uart1_enhet = NULL; 

//denne funksjonen tar inn en array med bytes, som er selve dataen som skal sendes, og lengden defineres, altså hvor hvor mange bytes dataen består av.
//Den skal returnere en CRC-verdi som er på 16 bit. CRC-verdien skal brukes til å sjekke at dataen i pakken ikke er skadet
//Lager en peker til array med bytes og bruker const for å sikre at innholdet ikke endres, og bruker size_t for å definere lengden på data
//lager et register kalt crc for bytene med startverdi 0, når bytene behandles bygges CRC-verdien opp.
static uint16_t crc16_beregning(const uint8_t *data, size_t len){

    //når alle bytes er behandlet returneres CRC som en kontrollsum.
    uint16_t crc = 0;
    

    //for løkke som går igjennom hver byte i data-array, starter på indeks 0 og fortsetter til alle bytes (len) er behandlet
    for (size_t i = 0; i < len; i++) {
    
        //data[i] brukes for å hente en byte fra data-arrayet og caster til 16-bit så den kan brukes med CRC-registeret
        // <<8 brukes for å flytte byten 8 biter til venstre og plasseres i øverste halvdel i CRC-registeret
        //bruker XOR for å blande byten inn i CRC-registeret. denne sammenlingningen gjør at CRC blir endret basert på ny data som blir lagt inn.
        crc ^= (uint16_t)data[i] << 8;
        
        //bruker en for løkke for å gå igjennom alle 8 bit i byten - CRC blir beregnet bit for bit. 
        for (int bit = 0; bit < 8; bit++) {

            //sjekker om den øverste biten(den mest signifikante biten, som er til venstre) i CRC-registeret er 1
            if (crc & 0x8000) {

                //Hvis den er 1, så betyr det at CRC-registeret inneholder et aktivt polynomledd, så skiftes CRC-registeret et steg til venstre og CRC-registeret blir XOR'a med polynomet 0x1021
                crc = (crc << 1) ^ 0x1021;

                //Hvis den er 0, blir registeret bare skiftet et steg til venstre og ingen XOR blir utført
            } else { 
                crc <<= 1;
            }
        }
    }
    //Returnerer hele CRC-registeret etter behandlingen av hele datasettet
    return crc;
}

    //Denne funksjonen starter opp UART1, henter UART1 enheten fra devicetree og sjekker at den er klar
    void uart_start(void){

        //Henter UART1 fra devicetreet og lagrer dens referanse i uart1_enhet
        uart1_enhet = DEVICE_DT_GET(UART_NODE_LABEL);

        //Sjekker om den er klar, hvis den ikke er klar printes det ut en beskjed om det, device_is_ready er en zephyr funksjon.
        if (!device_is_ready(uart1_enhet)) {
            printf("UART1 er ikke klar\n");

        //Hvis den er klar, printes det ut en melding om at UART1 har startet
        } else {

            //uart1_enhet peker til en struct device i Zephyr, name har navnet som har blitt satt som navnet på enheten i devicetree
            printf("UART1 har startet: %s\n", uart1_enhet->name);

        }
}    

//funksjon for å sende RPM-verdi
void send_rpm(int rpm) {

    //sjekker at UART har startet og hvis ikke så printes beskjed om det og funksjonen avsluttes
    if (!uart1_enhet) {
        printf("UART har ikke startet\n");
        return;
    }


    //Bygger innholdet som skal puttes i rammen, består av kommando og data
    // lager en array på 5 bytes for informasjonen som skal sendes. 1 byte for kommando og 4 bytes for selve dataen som er RPM-verdi
    //VESC firmware definerer rpm-verdi som int32_t
    uint8_t kommando_og_data[1 + sizeof(int32_t)];

    // her setter vi hva den første byten er, det er kommandoen som vi gir til VESC-enheten, at den skal endre RPM-verdien til det vi ønsker.
    kommando_og_data[0] = COMM_SET_RPM;

    //Dette gjør rpm verdien fra little-endian til big-endian, som betyr at rekkefølgen på bytene i tallet/verdien vi sender snus
    //Dette gjør at første tallet blir til den mest betydningsfulle fordi det VESC-enheten leser tallene i big-endian formatet.
    int32_t rpm_big_endian = __builtin_bswap32(rpm);

    //bruker memory copy funksjonen for å kopiere antall bytes fra en plass i minnet til en annen plass
    //[0] ble brukt for å putte kommando , bruker [1] for å putte selve rpm verdien i de siste 4 bytene.
    //lager to pekere sånn at memcpy funksjonen kopierer 4 byte fra rpm_big_endian og putter i kommando_og_data arrayet.
    memcpy(&kommando_og_data[1], &rpm_big_endian, sizeof(rpm_big_endian));

    // her lager jeg en variabel som heter crc den bruker funksjonen for CRC-beregning, returnerer en 16bit verdi. denne verdien lagres da i CRC variabelen
    //verdien er kontrollsummen for å se om dataen i rammen er skadet.
    uint16_t crc = crc16_beregning(kommando_og_data, sizeof(kommando_og_data));

    //Bygger rammen til hele datapakken så den følger VESC-protokollen, 12 bytes.
    //I rammen er det: 
    //1 startbyte
    //1 byte for lengden
    //antall bytes på selve innholdet(kommando og rpm) 
    //2 bytes for CRC
    //1 sluttbyte
    uint8_t ramme[2 + sizeof(kommando_og_data) + 2 + 1];

    //lager en variabel for å ha kontroll hvor i rammen(arrayet) vi skal legge inn neste byte
    size_t indeks = 0;

    //første byte for rammen som er startbyte (starten på rammen)
    ramme[indeks++] = VESC_STX;

    //Andre byte i pakken for å angi hvor mange byte det er i innholdet (lengden)
    ramme[indeks++] = (uint8_t)sizeof(kommando_og_data);

    //kopierer all data fra kommando_og_data og putter i ramme[indeks] og øker etterpå indeksen for å peke videre så hele innholdet kommer med
    memcpy(&ramme[indeks], kommando_og_data, sizeof(kommando_og_data)); indeks += sizeof(kommando_og_data);

    //setter inn den øverste byten av CRC-sjekksummen
    ramme[indeks++] = (uint8_t)(crc >> 8);

    //legger inn den nederste(minste) byten av CRC-sjekksummen
    ramme[indeks++] = (uint8_t)(crc & 0xFF);

    //legger til sluttbyte for å indikere slutten på rammen
    ramme[indeks++] = VESC_ETX;


    //bruker en for løkke for å sende 1 og 1 byte over til VESC-enheten
    //uart_poll_out sender en byte og venter til linjen er klar før neste blir sendt
    for (size_t i = 0; i < indeks; i++){
    uart_poll_out(uart1_enhet, ramme[i]);

    //har en liten pause mellom hver byte så VESC ikke mister data under overføringen
    k_busy_wait(100); //100 mikrosekunder mellom bytes
    }

    //printer ut hvor mange RPM som har blitt sendt
    printf("RPM-pakke sendt: %d RPM\n", rpm);
}

//denne funksjonen tar inn 2 parameter 
//en VESC-enhet_id som parameter,så den vet hvilken spesifikk VESC-enhet den skal sende til
//og den tar inn en RPM verdi som parameter, det er denne verdien som skal sendes til VESC-enheten
void send_rpm_videre(uint8_t VESC_enhet_id, int32_t rpm) {

    //Sjekker at UART har startet og hvis ikke så sender den ikke data
    if (!uart1_enhet) {
        return;
    }

    //siden C bruker little endian(minst signifikante) først, men VESC-protocol krever big endian(mest signifikante) først
    //kompilatoren har en innebygget funksjon som gjør denne endringen, derfor brukes __builtin_bswap32
    int32_t rpm_big_endian = __builtin_bswap32(rpm);

    //lager et array som heter kommando_og_data som inneholder VESC-data vi skal sende på totalt 7 bytes
    //første byte for COMM_FORWARD_CAN
    //andre byte for VESC_enhet_id
    //tredje byte for COM_SET_RPM
    //4 bytes for rpm-verdien
    uint8_t kommando_og_data[1 + 1 + 1 + sizeof(rpm_big_endian)];

    //plass brukes for å fortelle hvor i arrayet vi er, og starter på plass 0
    size_t plass = 0; 
    
    //legger COM_FORWARD_CAN i plass 0 (første byte) og øker plass med 1 (flytter til neste plass på array)
    kommando_og_data[plass++] = COMM_FORWARD_CAN;

    //legger VESC_enhet_id i plass 1 (andre byte) og øker plass med 1 (flytter til neste plass på array)
    kommando_og_data[plass++] = VESC_enhet_id;

    //legger COM_SET_RPM i plass 2 (tredje byte) og øker plass med 1 (flytter til neste plass på array)
    kommando_og_data[plass++] = COMM_SET_RPM;

    //bruker memcpy funksjonen til å kopiere bytes fra et sted til et annet
    //oppgir først hvor det skal kopieres til, kommando_og_data arrayet og den plassen vi har kommet til, "&" brukes å angi adressen
    // oppgir deretter hvor vi skal kopiere fra som er rpm_big_endian og "&" brukes for å hente adressen til første byten
    // oppgir deretter hvor mange bytes som skal kopieres og da brukes sizeof(rpm_big_endian)
    memcpy(&kommando_og_data[plass], &rpm_big_endian, sizeof(rpm_big_endian));

    //nå som 4 bytes til har blitt lagt inn må vi flytte plassen på arrayet til slutten så ikke noe blir overskrevet hvis det senere skal legges til flere bytes.
    //bruker sizeof(rpm_big_endian) her også fordi det i teorien betyr å flytte plassen 4 bortover siden den er på 4 bytes.
    plass += sizeof(rpm_big_endian);

    //bruker funksjonen crc16_beregning for å beregne en crc-sjekksum 
    //for å sikre at kommandoen og dataen som sendes ikke er skadet eller endret underveis
    //bruker sizeof så den bruker lengden til kommando_og_data (7 bytes)
    uint16_t crc = crc16_beregning(kommando_og_data, sizeof(kommando_og_data));

    //lager et array for datapakken som skal videresendes til VESC-enhet
    //pakken er på totalt 12 bytes
    //1 for startbyte
    //1 for lengden
    //7 for innholdet(COMM_FORWARD_CAN),(VESC_enhet_id),(COM_SET_RPM)(rpm-verdi)
    //2 for crc
    //1 for sluttbyte
    uint8_t ramme[ 2 + sizeof(kommando_og_data) + 2 + 1];

    //setter startposisjon i rammen på indeks 0 så innlegging av bytes starter her
    size_t indeks = 0;

    //flytter posisjon med en og legger inn startbyte
    ramme[indeks++] = VESC_STX;

    //legger inn 1 byte for lengden på selve innholdet (teller ikke med startbyte,sluttbyte,crc)
    ramme[indeks++] = (uint8_t)sizeof(kommando_og_data);

    //kopierer det som er i kommando_og_data, antall bytes som må kopieres, og legger det inn i ramme
    memcpy(&ramme[indeks], kommando_og_data, sizeof(kommando_og_data)); 

    //flytter posisjon i array til slutten, utifra lengden på kommando_og_data (7 bytes)
    indeks += sizeof(kommando_og_data);

    //flytter posisjon med 1 så det vi la inn ikke blir overskrevet og legger inn CRC-register (mest signifikante bit først)
    ramme[indeks++] = (uint8_t)(crc >> 8);

    //flytter posisjon og legger inn i CRC-register (den minste signifikate bit)
    ramme[indeks++] = (uint8_t)(crc & 0xFF);

    //flytter posisjon og legger inn sluttbyte for å indikere slutten på rammen
    ramme[indeks++] = VESC_ETX;

    //bruker for løkke for å iterere over alle bytes
    for (size_t i = 0; i < indeks; i++) {

        //bruker denne funksjonen til å sende en byte om gangen via UART
        uart_poll_out(uart1_enhet, ramme[i]);

        //pause på 300 mikrosekunder mellom sending av hver byte så VESC rekker å behandle byten
        k_busy_wait(300);
    }






}

