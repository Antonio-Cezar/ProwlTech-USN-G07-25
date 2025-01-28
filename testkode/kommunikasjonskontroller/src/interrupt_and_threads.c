
#include "interrupt_and_threads.h"
#include "math_calc.h"
#include "path_check.h"
#include "canbus.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "bt_controller.h"




#define PROCESS_THREAD_STACK_SIZE 4096
#define PROCESS_THREAD_PRIORITY 5

K_THREAD_STACK_DEFINE(process_thread_stack, PROCESS_THREAD_STACK_SIZE);
struct ProjectionResult x_y_temp;
uint8_t sensorRead;
struct k_thread process_thread;
static struct joystick_data js_raw_data;
struct data_for_motorcontroller data;



#define TIMER_INST_IDX 1
#define TIME_TO_WAIT_MS 1000UL
void process_data(struct data_for_motorcontroller data, uint8_t sensorVals){
        
        printk("Inne i dataprosesserings-funksjonen\n");
        struct path_check_results motor_values = path_check(data.angle , data.speed, data.js, sensorVals);
        send_custom_data(get_can_dev(), (uint8_t)motor_values.js_num, (int16_t)motor_values.angle, (int16_t)motor_values.speed);

}

static void timer_handler(nrf_timer_event_t event_type, void *p_context)
{
    if(event_type == NRF_TIMER_EVENT_COMPARE0)
    {
        
        //char * p_msg = p_context;
        // Create a new Zephyr thread to run process_data function
        k_tid_t thread_id = k_thread_create(&process_thread, process_thread_stack,
                                            PROCESS_THREAD_STACK_SIZE,
                                            (k_thread_entry_t)process_data, NULL,
                                            NULL, NULL, PROCESS_THREAD_PRIORITY, 0,
                                            K_NO_WAIT);
        if (thread_id != NULL) {
            printk("Thread created successfully\n");
        } else {
            printk("Failed to create thread\n");
        }
        printk("Timer interrupt!\n");        
    }
}

void continous_js_thread(){
        while (1){
                
                js_raw_data = get_js_data();
                process_joystick_data();
                x_y_temp = stereographic_projection_2D(js_raw_data.x , js_raw_data.y);
                data.angle = calculate_angle_degrees(x_y_temp.x, x_y_temp.y);
                data.speed = scale_to_percent(calculate_radius(x_y_temp.x, x_y_temp.y), 512);
                data.js = js_raw_data.joystickId;
                printk("Angle: %d, speed: %d\n", data.angle, data.speed);
                k_msleep(100);
        }
}

void continous_sensor_thread(){
        while(1){
                sensorRead = get_sensor_byte();
                printk("Sensorverdi: %d\n", (int)sensorRead);

                k_msleep(100);
        }
}

void interruptThreadBegin(){
        printk("Hello Timer with Interrupts! %s\n", CONFIG_BOARD);
	
    //timer setup
	#if defined(__ZEPHYR__)
    	IRQ_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TIMER_INST_GET(TIMER_INST_IDX)), IRQ_PRIO_LOWEST,
                NRFX_TIMER_INST_HANDLER_GET(TIMER_INST_IDX), 0, 0);
	#endif
	
	nrfx_timer_t timer_inst = NRFX_TIMER_INSTANCE(TIMER_INST_IDX);
 	uint32_t base_frequency = NRF_TIMER_BASE_FREQUENCY_GET(timer_inst.p_reg);
        nrfx_timer_config_t config = NRFX_TIMER_DEFAULT_CONFIG(base_frequency);
        config.bit_width = NRF_TIMER_BIT_WIDTH_32;
        config.p_context = "My context";

        nrfx_err_t status = nrfx_timer_init(&timer_inst, &config, timer_handler);
        NRFX_ASSERT(status == NRFX_SUCCESS);

        nrfx_timer_clear(&timer_inst);

    /* Creating variable desired_ticks to store the output of nrfx_timer_ms_to_ticks function */
        uint32_t desired_ticks = nrfx_timer_ms_to_ticks(&timer_inst, TIME_TO_WAIT_MS);
    
    /*
     * Setting the timer channel NRF_TIMER_CC_CHANNEL0 in the extended compare mode to stop the timer and
     * trigger an interrupt if internal counter register is equal to desired_ticks.
     */
        nrfx_timer_extended_compare(&timer_inst, NRF_TIMER_CC_CHANNEL0, desired_ticks,
                                NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

        nrfx_timer_enable(&timer_inst);
}

void start_js_thread(){
    printf("Joystick-thread started!");
    k_tid_t js_thread_id = k_thread_create(&process_thread, process_thread_stack,
                                           PROCESS_THREAD_STACK_SIZE,
                                           continous_js_thread, NULL, NULL,
                                           NULL, PROCESS_THREAD_PRIORITY, 0,
                                           K_NO_WAIT);
    if (js_thread_id != NULL) {
        printk("Joystick thread created successfully\n");
    } else {
        printk("Failed to create Joystick thread\n");
    }
}

void start_sensor_thread(){
    printf("Sensor-thread started!");
    k_tid_t sensor_thread_id = k_thread_create(&process_thread, process_thread_stack,
                                               PROCESS_THREAD_STACK_SIZE,
                                               continous_sensor_thread, NULL, NULL,
                                               NULL, PROCESS_THREAD_PRIORITY, 0,
                                               K_NO_WAIT);
    if (sensor_thread_id != NULL) {
        printk("Sensor thread created successfully\n");
    } else {
        printk("Failed to create Sensor thread\n");
    }
}




