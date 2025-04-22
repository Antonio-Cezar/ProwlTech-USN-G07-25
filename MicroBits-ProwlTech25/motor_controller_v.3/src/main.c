#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "motorstyring.h" //lagt til av RAS(PROWLTECH25)
#include "canbus.h"
#include "vesc_uart.h"
// Commented and coded by OA and EAH


 int main(void) {
    printf("Starter UART...\n");
    //const struct device *const can_dev = get_can_dev(); // Getting the CAN device connected to the system
    //canBegin(can_dev); // Function for startring up and initializing the CAN device
    //printf("CAN er klar. \n");

    uart_init(); //initialiserer uart

    k_msleep(10);


    const struct device *const can_dev = get_can_dev();
    if (!can_dev) {
        printf("error: fant ikke can device\n");
        return -1;
    }
    canBegin(can_dev); // Function for startring up and initializing the CAN device
    printf("CAN er klar. \n");


    while (1) {
        //canState(can_dev);// Function for checking the CAN state
        process_can_data(); // Using the process_can_data function
        //if (er_nødstopp_aktivert()) { 
            //printf("nødstopp aktivert\n"); // RAS og linja over
        //}

        //send_string(can_dev, "Hello"); // Using the send_string function to send the string "Hello"
        k_msleep(50); // Sleep for 50ms, letting the callback handle messages
    }
}