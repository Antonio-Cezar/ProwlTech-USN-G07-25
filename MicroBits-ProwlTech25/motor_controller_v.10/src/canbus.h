#ifndef CANBUS_H
#define CANBUS_H

#include <zephyr/drivers/can.h>  //(fenris2024)
#include <zephyr/kernel.h>       //for å få tilgang til zephyr sine funksjoner k_busy_wait() og k_msleep(), brukes til venting og pauser
#include <stdint.h>              // Standard faste heltallstyper

//can_receive functions(fenris2024)
#define RECEIVE_ID 0x0000001 // RECEIVE ID (fenris2024)
#define CAN_MSGQ_SIZE 10 // Message queue size (fenris2024)
//can_send functions
#define MSG_ID 0x0000002 // Message ID (fenris2024)

//(RS_PROWLTECH)
//definerer en struct som samler og holder på all data som mottas fra fjernkontrollen via CAN
struct can_ret_data {

    //hvor fort bilen skal bevege seg. 
    // 0.0 = full stopp
    // 1.0 = maks fart 
    float fart;      

     //hvilken retning bilen skal bevege seg i (0 til 2pi radianer) 
    // 0 = rett frem
    // pi/2 = mot venstre
    // pi =  bakover
    // 3pi/2 =  mot høyre
    float vinkel;    

    //Den rotasjonen vi ønsker rundt egen akse
    // -1.0 = roter mot venstre
    // 0.0 = ingen rotasjon
    // 1.0 = roter mot høyre
    float rotasjon;

    //ekstra finjustering for sving fra en egen styrespak, for at bilen skal svinge som en ekte bil.
    float sving_js;
//(RS_PROWLTECH - SLUTT)
}; 

void canBegin(const struct device *can_dev); // Function for initializing and starting the CAN device(Fenris2024)

void canState(const struct device *can_dev); // Function for checking the CAN state(fenris2024)

//can_dev functions
const struct device *const get_can_dev(void); // Function for getting the CAN device(fenris2024)

extern struct k_msgq can_msgq; // Message queue structure(fenris2024)

int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data); // Function for setting up the can filter, taking in the CAN device, callback function and cb data(fenris2024)

void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data); // Function for checking CAN frames on the bus(fenris2024)

void process_can_data(void); // Function for processing the receievd CAN data (fenris)


void send_string(const struct device *can_dev, const char *str); // Function for sending a string over the bus(fenris2024)

//for å returnere når CAN mottok en melding sist. 
int64_t get_last_can_rx_time(void);  

const char* can_state_to_str(enum can_state state); // Function that converts the CAN states to string(fenris2024)

#endif // CANBUS_H
