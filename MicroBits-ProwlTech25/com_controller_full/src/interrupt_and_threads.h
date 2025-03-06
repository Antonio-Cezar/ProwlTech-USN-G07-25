#ifndef __INTERRUPT_AND_THREADS_H
#define __INTERRUPT_AND_THREADS_H

#include <stdio.h>
#include <stdbool.h>
#include <zephyr/sys/printk.h>
#include <nrfx_timer.h>

// Dette er en datatype brukt til å ta imot joystick-data fra den trådløse kontrolleren
// Den består av 2 heltall (potmeter verdi i x og y retning), og en bool (om joystick knapp er trykket inn)
struct js_data_raw {
    int pot_x; 
    int pot_y; 
    bool js;
};

// Dette er en datatype brukt for konvertert joystick-data, og er hva som vil bli kjørekommando for motor-kontroller.
// Den består av 2 heltall (vinkel til punktet x og y lager, og hypotenusen punktet lager fra origo) 
// og en bool om joystickknapp er trykket inn.
struct data_for_motorcontroller {
    int angle; 
    int speed; 
    bool js;
};

//funksjon brukt til feiltesting av tråder. Ble brukt til å se hvor mange ganger interrupten inntraff.
int get_counter();

// Dette er prosesseringsfunksjonen som kjøres i tråden som lages ved hver timet interrupt.
void process_data(struct data_for_motorcontroller data, uint8_t sensorVals);

// Dette er en tråd-funksjon som kontinuerlig kjøres, og dens oppgave er å konvertere joystick data til en kjørekommando, så lagre den globalt så process_data finner den.
void continous_js_thread();

// Dette er en tråd-funksjom som kontinuerlig kjøres, og dens oppgave er å hente sensor-dataen fra bt_controller bibliotek, så lagre den globalt så process_data finner den.
void continous_sensor_thread();

// Dette er funksjonen som kjøres når timet interrupt inntreffer. Når interrupt inntreffer endres event_type slik at ny funksjonalitet skjer --> lager tråd som kjører process_data
static void timer_handler(nrf_timer_event_t event_type, void *p_context);


// Dette er funksjonen som setter opp og initialiserer timet interrupt. Denne funksjonen er tatt fra veileders kode på github: https://github.com/aiunderstand/Real-TimeOS-with-Microbit-nRF52833/tree/main/Zephyr-C/Hardware_Timer_with_interrupt
void interruptThreadBegin();

// Dette er funksjonen som initialiserer og starter tråden som leser joystick-data. Tråden kjører continous_js_thread()
void start_js_thread();

// Dette er funksjonen som initialiserer og starter tråden som leser sensor-data. Tråden kjører continous_sensor_thread()
void start_sensor_thread();

#endif