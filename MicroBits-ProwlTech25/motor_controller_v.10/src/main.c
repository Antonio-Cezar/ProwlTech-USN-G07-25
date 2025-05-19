#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdio.h> //for tilgang til printf()
#include <stdint.h>  //for å bruke faste heltall
#include <stddef.h>  //tilgang til NULL og size_t
#include "vesc_uart.h" //tilgang til funksjoner for å sende RPM til vesc (RS_PROWLTECH)
#include "canbus.h" // tilgang til å hente joystick data fra Can-meldingskø (RS_PROWLTECH)
#include "motorstyring.h"  //beregner og sender motorverdier //(RS_PROWLTECH)

void main(void) {

    //starter UART og venter litt
    uart_start(); //(RS_PROWLTECH)
    k_msleep(100);

    //henter CAN-enhet fra devicetree
    const struct device *const can_dev = get_can_dev();
    if (!can_dev) {
        printf("Error: fant ikke CAN-enhet\n");
        return;
    }

    //starter CAN
    canBegin(can_dev);
    k_msleep(100);

    //Hovedløkka som håndterer innkommende data
    while (1) {
        
        //Henter data fra CAN og sender til motorstyring (RS_PROWLTECH)
        process_can_data();  

        //liten pause
        k_busy_wait(200);  
    }
}
