#ifndef __INTERRUPT_AND_THREADS_H
#define __INTERRUPT_AND_THREADS_H

#include <stdio.h>
#include <stdbool.h>
#include <zephyr/sys/printk.h>
#include <nrfx_timer.h>

struct js_data_raw {
    int pot_x; 
    int pot_y; 
    bool js;
};
struct data_for_motorcontroller {
    int angle; 
    int speed; 
    bool js;
};

void process_data(struct data_for_motorcontroller data, uint8_t sensorVals);
void continous_js_thread();
void continous_sensor_thread();
static void timer_handler(nrf_timer_event_t event_type, void *p_context);

void interruptThreadBegin();
void start_js_thread();
void start_sensor_thread();

#endif