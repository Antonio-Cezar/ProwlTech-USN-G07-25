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
#include "pwm.h"
#include "motorkontroller.h"
#include "motorstyring.h" //lagt til av RAS(PROWLTECH25)
#include "canbus.h"
// Commented and coded by OA and EAH


void main(void) {
    printf("Starter CAN...\n");
    const struct device *const can_dev = get_can_dev(); // Getting the CAN device connected to the system
    canBegin(can_dev); // Function for startring up and initializing the CAN device
    printf("CAN er klar. \n");

    int ret2 = pwm_begin();
    if(ret2 < 0){
        printf("\nError initializing PWM. Error code = %d\n", ret2);
        while(1);
    }

    //tester motorstyring direkte
    //float test_fart = 0.8f;
    //float test_vinkel = M_PI / 4;
    //float test_rotasjon = 0.3f;
    //kontroller_motorene(test_fart, test_vinkel, test_rotasjon);


    printf("PWM er klar.\n");

    float test = 1.23f;
    printf("test float: %.2f\n", test);

    k_msleep(10);

    while (1) {
        printf("In the while loop\n");
        canState(can_dev);// Function for checking the CAN state
        process_can_data(); // Using the process_can_data function
        send_string(can_dev, "Hello"); // Using the send_string function to send the string "Hello"
        k_msleep(50); // Sleep for 50ms, letting the callback handle messages
    }
}