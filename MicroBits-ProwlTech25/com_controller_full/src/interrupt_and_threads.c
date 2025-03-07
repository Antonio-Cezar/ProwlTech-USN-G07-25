#include "interrupt_and_threads.h"
#include "math_calc.h"
#include "path_check.h"
#include "canbus.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "bt_controller.h"
//(PROWLTECH25) Commented and coded by CA & RS 

//(FENRSI24) Definerer hvor stort plass hver tråd skal tildeles
#define PROCESS_THREAD_STACK_SIZE_JOY  4096
#define PROCESS_THREAD_STACK_SIZE_SENS 4096
#define PROCESS_THREAD_STACK_SIZE_INT  4096

//(FENRSI24) Definerer prioriteten til hver tråd
#define PROCESS_THREAD_PRIORITY_JOY  3
#define PROCESS_THREAD_PRIORITY_SENS 2
#define PROCESS_THREAD_PRIORITY_INT  1

//(FENRSI24) Reserverer et område på stacken til hver tråd
K_THREAD_STACK_DEFINE(sensor_stack, PROCESS_THREAD_STACK_SIZE_SENS);
K_THREAD_STACK_DEFINE(joystick_stack, PROCESS_THREAD_STACK_SIZE_JOY);
K_THREAD_STACK_DEFINE(int_stack, PROCESS_THREAD_STACK_SIZE_INT);

//(FENRSI24) Variabel for mellomlagring av joystick-koordinater etter stereografisk projeksjon
struct ProjectionResult x_y_temp;

//(FENRSI24) Global variabel for sensorverdi mottatt over CANBUS
uint8_t sensorRead;

//(FENRSI24) Trådstruktur-variabler for hver tråd
struct k_thread process_thread1;
struct k_thread process_thread2;
struct k_thread process_thread3;

//(FENRSI24) Variabel for rå joystick-data mottatt over Bluetooth
static struct joystick_data js_raw_data;

//(FENRSI24) Variabel for kjørekommando
struct data_for_motorcontroller data;

//(FENRSI24) Tråd-ID variabler (NULL = ikke startet ennå)
k_tid_t js_thread_id = NULL;
k_tid_t sensor_thread_id = NULL;
static k_tid_t thread_id = NULL;

//(FENRSI24) Variabel for feilsøking – teller hvor mange ganger interrupt er trigget
static int counter = 0;

//(FENRSI24) Timer-konfigurasjon
#define TIMER_INST_IDX 1
#define TIME_TO_WAIT_MS 500UL

//(FENRSI24) Behandler data og sender kommando til motorcontroller
void process_data(struct data_for_motorcontroller data, uint8_t sensorVals) {
    struct path_check_results motor_values = path_check(data.angle, data.speed, data.js, sensorVals);
    
    //(FENRSI24) Sender kjørekommando over CANBUS
    send_custom_data(get_can_dev(), (uint8_t)motor_values.js_num, (int16_t)motor_values.angle, (int16_t)motor_values.speed);

    //(FENRSI24) Venter til tråden er ferdig og frigjør ressursene
    if (thread_id != NULL) {
        k_thread_join(thread_id, K_FOREVER);
        thread_id = NULL;
    }
}


//(FENRSI24) Returnerer interrupt-telleren for feilsøking
int get_counter() {
    return counter;
}

//(FENRSI24) Timer-handler som starter prosesseringstråd ved interrupt
static void timer_handler(nrf_timer_event_t event_type, void *p_context) {
    if (event_type == NRF_TIMER_EVENT_COMPARE0) {
        if (thread_id == NULL) {
            counter++;

            //(FENRSI24) Starter en ny tråd for prosessering
            thread_id = k_thread_create(&process_thread1, int_stack,
                                        K_THREAD_STACK_SIZEOF(int_stack),
                                        (k_thread_entry_t)process_data, NULL,
                                        NULL, NULL, PROCESS_THREAD_PRIORITY_INT, 0,
                                        K_NO_WAIT);

            if (thread_id != NULL) {
                printk("Thread created successfully\n");
            } else {
                printk("Failed to create thread\n");
            }
        }
    }
    k_msleep(10);
}

//(FENRSI24) Kontinuerlig tråd for innhenting av joystick-data
void continous_js_thread() {
    while (1) {
        js_raw_data = get_js_data();
        process_joystick_data();

        //(FENRSI24) Utfører stereografisk projeksjon av joystick-verdier
        x_y_temp = stereographic_projection_2D(js_raw_data.x, js_raw_data.y);

        //(FENRSI24) Oppdaterer kjørekommando
        data.angle = calculate_angle_degrees(x_y_temp.x, x_y_temp.y);
        data.speed = scale_to_percent(calculate_radius(x_y_temp.x, x_y_temp.y), 512);
        data.js = js_raw_data.joystickId;

        //(FENRSI24) Sover før ny iterasjon
        k_msleep(10);
    }
}

//(FENRSI24) Kontinuerlig tråd for innhenting av sensor-data
void continous_sensor_thread() {
    while (1) {
        sensorRead = get_sensor_byte();
        k_msleep(10);
    }
}

//(FENRSI24) Initialiserer og starter timerbasert interrupt
void interruptThreadBegin() {
    printk("Timed interrupt initialized! %s\n", CONFIG_BOARD);

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

    uint32_t desired_ticks = nrfx_timer_ms_to_ticks(&timer_inst, TIME_TO_WAIT_MS);
    nrfx_timer_extended_compare(&timer_inst, NRF_TIMER_CC_CHANNEL0, desired_ticks,
                                NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    nrfx_timer_enable(&timer_inst);
}

//(FENRSI24) Starter joystick-tråden
void start_js_thread() {
    if (js_thread_id == NULL) {
        js_thread_id = k_thread_create(&process_thread2, joystick_stack,
                                       K_THREAD_STACK_SIZEOF(joystick_stack),
                                       continous_js_thread, NULL, NULL, NULL,
                                       PROCESS_THREAD_PRIORITY_JOY, 0, K_NO_WAIT);

        if (js_thread_id != NULL) {
            printk("Joystick thread created successfully\n");
        } else {
            printk("Failed to create Joystick thread\n");
        }
    }
}

//(FENRSI24) Starter sensor-tråden
void start_sensor_thread() {
    if (sensor_thread_id == NULL) {
        sensor_thread_id = k_thread_create(&process_thread3, sensor_stack,
                                           K_THREAD_STACK_SIZEOF(sensor_stack),
                                           continous_sensor_thread, NULL, NULL, NULL,
                                           PROCESS_THREAD_PRIORITY_SENS, 0, K_NO_WAIT);

        if (sensor_thread_id != NULL) {
            printk("Sensor thread created successfully\n");
        } else {
            printk("Failed to create Sensor thread\n");
        }
    }
}
