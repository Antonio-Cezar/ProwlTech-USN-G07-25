#ifndef VESC_UART_H //passer på at fila bare blir inkludert en gang for tryggere kompilering
#define VESC_UART_H 

//HELE VESC_UART.H ER SKREVET AV RAMEEZ SADIQ PROWLTECH 2025

//For å få tilgang til faste heltallstyper
#include <stdint.h> 

//Start-byte som forteller VESC-firmware hvor pakken starter
#define VESC_STX 0x02

//Slutt-byte som forteller VESC-firmware hvor pakken slutter
#define VESC_ETX 0x03

//kommando-ID hentet fra den offisielle VESC-firmwaren, må være riktig ID-nummer for at VESC-enheten skal tolke kommandoen riktig
#define COMM_SET_RPM    8 //Forteller VESC-enheten om hvilken hastighet motor skal ha
#define COMM_FORWARD_CAN    34 //Forteller VESC-enheten om å videresende kommando til en annen enhet via CAN

//Initialiserer UART1 så den er klar for kommunikasjon med VESC
void uart_start(void);

 //Sender en RPM-verdi til master VESC-enhet via UART
void send_rpm(int32_t rpm);

//Master-VESC sender en RPM-verdi til spesifikk VESC-enhet via CAN
void send_rpm_videre(uint8_t node_id, int32_t rpm); 

#endif // VESC_UART_H