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

void send_sensor_distances(const struct device *can_dev, uint8_t *distances) {
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));

    frame.flags = CAN_FRAME_IDE;
    frame.id = MSG_ID;
    frame.dlc = 4;  // Vi sender 4 bytes

    memcpy(frame.data, distances, 4);  // Kopierer 4 bytes inn i CAN-rammen

    if (can_send(can_dev, &frame, K_SECONDS(10), NULL, NULL) != 0) {
        printk("Failed to send distance frame\n");
    } else {
        printk("Sent distances: %d %d %d %d\n", distances[0], distances[1], distances[2], distances[3]);
    }
}


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

    if (setup_can_filter(can_dev, can_rx_callback, NULL) < 0) { // Check to see if the CAN filter is set up or not.
        printk("Failed to add CAN filter\n");
        return;
    }

    hc_sr04_init(&sensors[0], "GPIO_0", 4, 3); //Forward sensor bit 000(0), Trigger on pin P2(P0.04), Echo on pin P1(P0.03)
    hc_sr04_init(&sensors[1], "GPIO_0", 10, 9); //Right Sensor bit 00(0)0, Trigger on pin P8(P0.10), Echo on pin P9(P0.09)
    hc_sr04_init(&sensors[2], "GPIO_0", 31, 28); //Left Sensor bit 0(0)00, Trigger on pin P3(P0.31), Echo on pin P4(P0.28)
    hc_sr04_init(&sensors[3], "GPIO_0", 11, 30); //Rear Sensor bit (0)000, Trigger on pin P7(P0.11), Echo on pin P10(P0.30)

    while (1) {
        if (can_get_state(can_dev, &state, &err_cnt) != 0) {
            printk("Failed to get CAN state\n");
        }
    
        double distances[4];
        uint8_t sensor_distances[4];  // Avrundede avstander i byteformat
    
        for (int i = 0; i < 4; i++) {
            distances[i] = hc_sr04_read_distance(&sensors[i]);
            sensor_distances[i] = (uint8_t)distances[i];  // Konverter til heltall 0–255
    
            printk("Sensor %d Distance: %d cm\n", i + 1, sensor_distances[i]);
        }
    
        send_sensor_distances(can_dev, sensor_distances);
    
        k_msleep(500);
    }
}