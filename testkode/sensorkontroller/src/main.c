#include "canbus.h"
#include "sensors.h"



int main(void) {
    const struct device *const can_dev = get_can_dev();
    hc_sr04_t sensors[4];
    if (!can_dev) {
        printk("Cannot find CAN device!\n");
        return;
    }
    /*else{
        printf("Found CAN device!\n");
    }
    */

    

    enum can_state state;
    struct can_bus_err_cnt err_cnt;

    int ret = can_start(can_dev);
    if (ret != 0) {
        printf("Failed to start CAN controller: Error %d\n", ret);
        return;
    } /*else {
        printf("CAN controller started successfully\n");
    }*/

    if (setup_can_filter(can_dev, can_rx_callback, NULL) < 0) {
        printk("Failed to add CAN filter\n");
        return;
    }

    hc_sr04_init(&sensors[0], "GPIO_0", 4, 3); //Forward sensor bit 000(0), Trigger on pin P2(P0.04), Echo on pin P1(P0.03)
    hc_sr04_init(&sensors[1], "GPIO_0", 10, 9); //Right Sensor bit 00(0)0, Trigger on pin P8(P0.10), Echo on pin P9(P0.09)
    hc_sr04_init(&sensors[2], "GPIO_0", 31, 28); //Left Sensor bit 0(0)00, Trigger on pin P3(P0.31), Echo on pin P4(P0.28)
    hc_sr04_init(&sensors[3], "GPIO_0", 11, 30); //Rear Sensor bit (0)000, Trigger on pin P7(P0.11), Echo on pin P10(P0.30)

    while (1) {
        uint8_t sensor_byte = 0;
        if (can_get_state(can_dev, &state, &err_cnt) != 0) {
            printk("Failed to get CAN state\n");
        } else {
        printf("CAN State: %s\n", can_state_to_str(state));
        printf("TX Errors: %d, RX Errors: %d\n", err_cnt.tx_err_cnt, err_cnt.rx_err_cnt);
        }

        double distances[4];
        distances[0] = hc_sr04_read_distance(&sensors[0]);
        distances[1] = hc_sr04_read_distance(&sensors[1]);
        distances[2] = hc_sr04_read_distance(&sensors[2]);
        distances[3] = hc_sr04_read_distance(&sensors[3]);

        for (int i = 0; i < 4; i++) {
            printk("Sensor %d Distance: %d cm\n", i + 1, (int)distances[i]);
            if ((int)distances[i] < 15) {
                sensor_byte |= (1 << i); // Set the bit corresponding to the sensor if blocked
            }
        }

        printk("Sensor Status: 0x%02X\n", sensor_byte);
        send_byte(can_dev, sensor_byte);
        k_sleep(K_SECONDS(1));
    }
}