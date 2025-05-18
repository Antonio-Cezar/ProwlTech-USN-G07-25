#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys_clock.h>
#include <zephyr/timing/timing.h>
#include <stdbool.h>
#include <string.h>
#include "hc_sr04.h"
#include "canbus.h"
// Commented and coded by OA

hc_sr04_t sensors[4]; // Structure for defining how many sensors to use
enum can_state state; // Defines the state of the CAN controller
struct can_bus_err_cnt err_cnt; // CAN controller error counter

int main(void) {
    const struct device *const can_dev = get_can_dev(); // Getting the CAN device connected to the system
   
    if (!can_dev) { // Test that checks is the CAN device is found. If not it will print the error message and return
        printk("Cannot find CAN device!\n");
        return;
    }
    /*else{
        printf("Found CAN device!\n");
    }
    */    

    int ret = can_start(can_dev); // Starting up the CAN device
    if (ret != 0) { // Check to see if the CAN device has started, if not it will print the error message and return
        printf("Failed to start CAN controller: Error %d\n", ret);
        return;
    } 

    canBegin();

    if (setup_can_filter(can_dev, can_rx_callback, NULL) < 0) { // Check to see if the CAN filter is set up or not.
        printk("Failed to add CAN filter\n");
        return;
    }

    hc_sr04_init(&sensors[0], "GPIO_0", 4, 3); //Forward sensor bit 000(0), Trigger on pin P2(P0.04), Echo on pin P1(P0.03)
    hc_sr04_init(&sensors[1], "GPIO_0", 10, 9); //Right Sensor bit 00(0)0, Trigger on pin P8(P0.10), Echo on pin P9(P0.09)
    hc_sr04_init(&sensors[2], "GPIO_0", 31, 28); //Left Sensor bit 0(0)00, Trigger on pin P3(P0.31), Echo on pin P4(P0.28)
    hc_sr04_init(&sensors[3], "GPIO_0", 11, 30); //Rear Sensor bit (0)000, Trigger on pin P7(P0.11), Echo on pin P10(P0.30)

    while (1) {
        uint8_t sensor_byte = 0; // Sensor byte variable
        if (can_get_state(can_dev, &state, &err_cnt) != 0) { // If test to check the state of the CAN controller
            printk("Failed to get CAN state\n");
        } else {
        //printf("CAN State: %s\n", can_state_to_str(state));
        //printf("TX Errors: %d, RX Errors: %d\n", err_cnt.tx_err_cnt, err_cnt.rx_err_cnt);
        }

        double distances[4]; // Array with four distances. One for each sensor
        distances[0] = hc_sr04_read_distance(&sensors[0]); // Getting the distance for sensor 1
        distances[1] = hc_sr04_read_distance(&sensors[1]); // Getting the distance for sensor 2
        distances[2] = hc_sr04_read_distance(&sensors[2]); // Getting the distance for sensor 3
        distances[3] = hc_sr04_read_distance(&sensors[3]); // Getting the distance for sensor 4

        for (int i = 0; i < 4; i++) { // For loop that will run through the number of sensors used
            printk("Sensor %d Distance: %d cm\n", i + 1, (int)distances[i]); // Print funkction to display the sensor and its distance 
            if ((int)distances[i] < 15) { // If test that checks if the distance returned by the sensors is below the given value
                sensor_byte |= (1 << i); // Set the bit corresponding to the sensor if blocked
            }
        }

        printk("Sensor Status: 0x%02X\n", sensor_byte);
        send_byte(can_dev, sensor_byte); // Sending the sensor byte over the bus
        //printk("Byte sent");
        k_msleep(500); // Sleep for 500ms
    }
}