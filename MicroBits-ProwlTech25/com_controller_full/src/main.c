#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>
#include "interrupt_and_threads.h"
#include "canbus.h"
#include "math_calc.h"
#include "path_check.h"
#include "canbus.h"
#include "bt_controller.h"


int main(void) {
    //configure_uart(uart_dev);
    btBegin();
    canBegin();
    start_sensor_thread();
    start_js_thread();
    interruptThreadBegin();

    while (1) {
        k_msleep(500);  // Sleep for 100 milliseconds
    }
    return 0; // Add return statement
}