#include <zephyr/drivers/can.h>
#include "interrupt_and_threads.h"
#include "canbus.h"
#include "math_calc.h"
#include "path_check.h"
#include "canbus.h"
#include "bt_controller.h"


void main(void)
{
    const struct device *const uart_dev = get_uart_dev();
    if (!device_is_ready(uart_dev)) {
        printk("UART device is not ready\n");
        return;
    }

    configure_uart(uart_dev);
    canBegin();
    start_sensor_thread();
    start_js_thread();
    interruptThreadBegin();

    while(1) {
        k_msleep(100);  // Sleep for 100 milliseconds
    }

}
