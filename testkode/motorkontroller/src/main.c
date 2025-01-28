#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <stdio.h>
#include "pwm.h"
#include "motorkontroller.h"
#include "canbus.h"


int main(void)
{
        struct can_ret_data drive_command;
        can_begin();
        pwm_begin();
        while (true)
        {
                // if (can_get_state(can_dev, &state, &err_cnt) != 0) {
                //         printk("Failed to get CAN state\n");
                // } else {
                //         //Finn løsning her etterhvert
                //         //printf("CAN State: %s\n", can_state_to_str(get_can_state()));
                //         //printf("TX Errors: %d, RX Errors: %d\n", err_cnt.rx_err_cnt, err_cnt.rx_err_cnt);
                // }
                drive_command = get_drive_command();
                control_motors(drive_command.js_deg_int_data, drive_command.js_spd_int_data, drive_command.js_bool_data);
                k_msleep(100);
        }
}
