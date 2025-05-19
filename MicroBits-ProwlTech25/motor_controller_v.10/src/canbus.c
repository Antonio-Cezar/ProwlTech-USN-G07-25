#include "canbus.h"
#include "motorstyring.h"

#include <zephyr/sys/byteorder.h>  // sys_le16_to_cpu()
#include <zephyr/kernel.h>         // K_MSGQ_DEFINE, k_uptime_get()
#include <zephyr/drivers/can.h>    
#include <stdio.h>
#include <string.h>
#include <math.h>

//DET SOM ER KODET AV RAMEEZ AHMED SADIQ (PROWLTECH2025) ER KOMMENTERT MED (RS_PROWLTECH)

//can_dev functions(fenris2024)
const struct device *const get_can_dev(void){ // Fuction responsible for finding and accessing the CAN device(fenris2024)
    return DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus)); // Returns the CAN device by looking for it in the devicetree/ overlay file under "chosen"(fenris2024)
} 

//can_receive functions(fenris2024)
K_MSGQ_DEFINE(can_msgq, sizeof(struct can_ret_data), CAN_MSGQ_SIZE, 4); // Message queue function for putting the joystick values received from the com controller node in(fenris2024)

//variabel for å lagre tidspunktet den siste CAN-meldingen ble mottatt som var gyldig
//den blir oppdatert i can_rx_callback
static int64_t last_can_rx_time = 0;

//returnerer verdien som er lagret i last_can_rx_time
int64_t get_last_can_rx_time(void) {
    return last_can_rx_time;
}

//SKREVET AV FENRIS(2024)
void canBegin(const struct device *can_dev){ // Function for startring up and initializing the CAN device(fenris2024)
    //const struct device *const can_dev = get_can_dev(); // Getting the CAN device connected to the system(fenris2024)
    
    if (!can_dev) { // Test that checks if the CAN device is found. If not it will print the error message and return(fenris2024)
        printk("Cannot find CAN device!\n"); //(fenris2024)
        return; //(fenris2024)
    }

    int ret = can_start(can_dev); // Starting up the CAN device (fenris2024)
    if (ret != 0) { // Check to see if the CAN device has started, if not it will print the error message and return (fenris2024)
        printf("Failed to start CAN controller: Error %d\n", ret); //(fenris2024)
        return; //(fenris2024)
    }
    if (setup_can_filter(can_dev, can_rx_callback, NULL) < 0) { // Check to see if the CAN filter is set up or not (fenris2024)
        printk("Failed to add CAN filter\n"); //(fenris2024)
        return; //(fenris2024)
    }

    
}
//skrevet av Fenris2024
int setup_can_filter(const struct device *dev, can_rx_callback_t rx_cb, void *cb_data){ // Function for setting up the can filter, taking in the CAN device, callback function and cb data(fenris2024)
    struct can_filter filter = { // CAN filter for the joystick values(fenris2024)
        .flags = CAN_FILTER_DATA | CAN_FILTER_IDE, // Setting the flags for the filter, where the filter matches the data frames, and that the filter matches CAN frames with extended ID (29-bit)(fenris2024)
	    .id = RECEIVE_ID, // Filter ID set to match the sender ID of the joystick values, defined in the header file(fenris2024)
	    .mask = CAN_EXT_ID_MASK // Filter bit mask for an extended 29-bit CAN ID(fenris2024)
    };
    return can_add_rx_filter(dev, rx_cb, cb_data, &filter); // Returning the rx filter(fenris2024)
}

// Callback for hver mottatt CAN‐frame
void can_rx_callback(const struct device *dev,
                     struct can_frame *frame,
                     void *user_data) {
    // Sjekk at ID og data‐lengde er korrekt (4×2 bytes = 8)
    if (frame->id == RECEIVE_ID && frame->dlc == 8) {
        // Dekode fire int16_t verdier, små‐endian til CPU‐endian
        int16_t f_i = sys_le16_to_cpu(*(int16_t *)&frame->data[0]);
        int16_t v_i = sys_le16_to_cpu(*(int16_t *)&frame->data[2]);
        int16_t r_i = sys_le16_to_cpu(*(int16_t *)&frame->data[4]);
        int16_t s_i = sys_le16_to_cpu(*(int16_t *)&frame->data[6]);

        // Pakk om til flytall (skaler fra 0–100 til 0.0–1.0)
        struct can_ret_data data = {
            .fart     = f_i  / 100.0f,
            .vinkel   = v_i  / 100.0f,
            .rotasjon = r_i  / 100.0f,
            .sving_js = s_i  / 100.0f,
        };

        last_can_rx_time = k_uptime_get();  // ← registrer tid
        // Legg meldingen i kø, håndteres i hovedløkken
        k_msgq_put(&can_msgq, &data, K_NO_WAIT);
    }
}

//can_send functions //skrevet av fenris(2024)

/* Function to convert CAN state to readable string */ //(fenris2024)
const char* can_state_to_str(enum can_state state) { //(fenris2024)
    switch (state) { //(fenris2024)
        case CAN_STATE_ERROR_ACTIVE: //(fenris2024)
            return "ERROR ACTIVE"; //(fenris2024)
        case CAN_STATE_ERROR_WARNING: //(fenris2024)
            return "ERROR WARNING"; //(fenris2024)
        case CAN_STATE_ERROR_PASSIVE: //(fenris2024)
            return "ERROR PASSIVE"; //(fenris2024)
        case CAN_STATE_BUS_OFF: //(fenris2024)
            return "BUS OFF"; //(fenris2024)
        case CAN_STATE_STOPPED: //(fenris2024)
            return "STOPPED"; //(fenris2024)
        default: //(fenris2024)
            return "UNKNOWN"; //(fenris2024)
    }
}

//skrevet av fenris2024
void send_string(const struct device *can_dev, const char *str){ // Function for sending a string over the bus(fenris2024)
    struct can_frame frame; // Using a CAN frame struct called frame(fenris2024)
    memset(&frame, 0, sizeof(frame)); // Zeros out the frame to ensure that all bits starts in a knwown state(fenris2024)

    frame.flags = CAN_FRAME_IDE; // Use extended identifier(fenris2024)
    frame.id = MSG_ID; // Using the message ID for the frame that is defined in the header file(fenris2024)
    frame.dlc = strlen(str);  // Set the DLC to the string length(fenris2024)
    memcpy(frame.data, str, frame.dlc);  // Copies the string to the CAN frame data field(fenris2024)

    if (can_send(can_dev, &frame, K_SECONDS(10), NULL, NULL) != 0) { // If test that sends the frame on the bus, and prints out a warning if it failes(fenris2024)
            printk("Failed to send frame\n"); //(fenris2024)
        } /*else {
            printk("Frame sent\n");
        }*/ //(fenris2024)
}

//(RS_PROWLTECH)
// funksjonen sjekker om det finnes noen nye CAN-meldinger i meldingskøen
//Hvis meldingen finnes blir den pakket ut og sendt videre til motorstyringsystemet
void process_can_data(void) { 

    //oppretter en variabel av typen can_ret_data, som kan holde på dataen som hentes ut av meldingskøen
    //struct skal samle dataene fart, vinkel, rotasjon og sving_js
    struct can_ret_data data;

    // Prøver å hente neste CAN-melding uten å blokkere og bruker K_NO_WAIT for å ikke vente, bare sjekke om noe er klart
    if (k_msgq_get(&can_msgq, &data, K_NO_WAIT) == 0) {

        // Skriver ut dataen som er mottatt, verdiene vises med to desimaltegn
        printf("[Fra PI til Microbit] fart=%.2f, vinkel=%.2f, rot=%.2f, sving_js=%.2f\n",
               data.fart, data.vinkel,
               data.rotasjon, data.sving_js);

        // De mottatte verdiene sendes videre til motorstyring for beregning
        kontroller_motorene(
            data.fart,
            data.vinkel,
            data.rotasjon,
            data.sving_js
        );
    }
}

