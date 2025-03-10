
#include "interrupt_and_threads.h"
#include "math_calc.h"
#include "path_check.h"
#include "canbus.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "bt_controller.h"



//(FENRIS24) Definerer hvor stort plass hver tråd skal tildeles
#define PROCESS_THREAD_STACK_SIZE_JOY 4096
#define PROCESS_THREAD_STACK_SIZE_SENS 4096
#define PROCESS_THREAD_STACK_SIZE_INT 4096

//(FENRIS24) Definerer prioriteten til hver tråd
#define PROCESS_THREAD_PRIORITYJOY 3
#define PROCESS_THREAD_PRIORITYSENS 2
#define PROCESS_THREAD_PRIORITYINT 1

//(FENRIS24) Reserverer et område på "stacken" til hver tråd
K_THREAD_STACK_DEFINE(sensor_stack, PROCESS_THREAD_STACK_SIZE_SENS);
K_THREAD_STACK_DEFINE(joystick_stack, PROCESS_THREAD_STACK_SIZE_JOY);
K_THREAD_STACK_DEFINE(int_stack, PROCESS_THREAD_STACK_SIZE_INT);

//(FENRIS24) Variabel av datatype med 2 heltall (x og y) som brukes etter det er utført en stereografisk projeksjon på potmeter x og y verdi.
struct ProjectionResult x_y_temp;

//(FENRIS24) Global variabel som kontinuerlig skal oppdateres med sensor-verdi sendt over CANBUS fra sensor-kontroller
uint8_t sensorRead;

//(FENRIS24) 3 trådstruktur-variabler. En for hver tråd.
struct k_thread process_thread1;
struct k_thread process_thread2;
struct k_thread process_thread3;

// Variabel med datatype som kan holde rå joystick-data sendt over bluetooth
static struct joystick_data js_raw_data;

//(FENRIS24) Variabel med datatype som kan holde en kjøre-kommando
struct data_for_motorcontroller data;

//(FENRIS24) Lager thread_id variabler lik NULL. Hvis null så settes så kan en tråd startes. Det er bygd slik fordi noen tråder bare skal initialiseres èn gang.
k_tid_t js_thread_id = NULL;
k_tid_t sensor_thread_id = NULL;
static k_tid_t thread_id = NULL;

//(FENRIS24) Variabel brukt til å feilteste. Brukt til å teste om timet interrupt ble triggret ved hvert intervall. Hvis så ble counter++.
int counter = 0;

//(FENRIS24) Litt usikker på hva denne gjør
#define TIMER_INST_IDX 1

//(FENRIS24) Definerer hvor ofte interrupt skal inntreffe i millisekunder
#define TIME_TO_WAIT_MS 500UL

//(FENRIS24) Funksjonen som sammenligner ønsket kjøreretning med sensor-verdier avlest. Kjørekommando sendes videre over CANBUS etter sammenligning
void process_data(struct data_for_motorcontroller data, uint8_t sensorVals) {

        
        //(FENRIS24) Sammenligner ønsket kjørertning med sensorverdi, og oppdaterer kjørekommando med resultatene fra sammenligningen. 
        struct path_check_results motor_values = path_check(data.angle , data.speed, data.js, sensorRead);

        //(FENRIS24) Sender kjørekommando over CANBUS
        send_custom_data(get_can_dev(), (uint8_t)motor_values.js_num, (int16_t)motor_values.angle, (int16_t)motor_values.speed);

        //(FENRIS24) Etter tråden er kjørt, må tråd_id settes lik NULL igjen slik at den kan startes på nytt ved neste timet interrupt
        if (thread_id != NULL) {
            //(FENRIS24) Venter til tråden er ferdig, og fjerner den når jobben er gjort
            k_thread_join(thread_id, K_FOREVER);

            //(FENRIS24) Setter thread_id = NULL
            thread_id = NULL;
        }
}


int get_counter(){
    //(FENRIS24) Returnerer counter
    return counter;
}


static void timer_handler(nrf_timer_event_t event_type, void *p_context)
{
    //(FENRIS24) Funksjonen venter på at korrekt "event" skjer. Skjer ved timet interrupt
    if(event_type == NRF_TIMER_EVENT_COMPARE0)
    {
        if(thread_id == NULL){
            counter++;
        
        //(FENRIS24) Lager en ny variabel som er lik en tråd med parametere: trådvariabel av tråddatatype, hvilken stack den skal bruke, strørrelsen på stacken, 
        //(FENRIS24) hvilken funksjon som skal kjøre, og prioritet.
            k_tid_t thread_id = k_thread_create(&process_thread1, int_stack,
                                            K_THREAD_STACK_SIZEOF(int_stack),
                                            (k_thread_entry_t)process_data, NULL,
                                            NULL, NULL, PROCESS_THREAD_PRIORITYINT, 0,
                                            K_NO_WAIT);
        //(FENRIS24) Hvis tråd lages uten feil
            if (thread_id != NULL) {
                printk("Thread created successfully\n");
            } else {
                //(FENRIS24) Hvis thread_id ikke setter til den nye tråden vil den fortsatt være lik NULL. Dette betyr at en feil oppsto i konstruksjonen av tråden
                printk("Failed to create thread\n");
            }        
        }
    k_msleep(10);
    }
}




void continous_js_thread(){
        while (1){
                //(FENRIS24) Henter joystick-data fra bt_controller.c.
                js_raw_data = get_js_data();
                
                //(FENRIS24) Funksjon for å si til bt_controller at data er hentet
                process_joystick_data();

                //(FENRIS24) Lager en ny variabel av datatype som skal holde x og y verdi etter en stereografisk projeksjon. Variabelen settes lik funksjonen som utfører
                //(FENRIS24) projeksjonen og sender ny-utregnet x og y ut av den.
                x_y_temp = stereographic_projection_2D(js_raw_data.x , js_raw_data.y);

                //(FENRIS24) Setter global kjørekommando sin vinkel lik dataen som kommer ut av funksjonen som kalkulerer vinkel ut fra x- og y-verdi.
                data.angle = calculate_angle_degrees(x_y_temp.x, x_y_temp.y);

                //(FENRIS24) Setter global kjørekommando sin fart lik en skalert verdi av hva punkt x og y sin lengde fra origo er i planet. Den skaleres i forhold til maks verdi den kan ha (512).
                data.speed = scale_to_percent(calculate_radius(x_y_temp.x, x_y_temp.y), 512);

                //(FENRIS24) Setter global kjørekommando sin bool lik joystick dataen sin bool. Ingen endring eller manipulering av verdien er ikke nødvendig
                data.js = js_raw_data.joystickId;

                //(FENRIS24) Printer verdiene til skjerm. Mest brukt for testing av verdier og funksjoner. brukt til test.
                //(FENRIS24) printk("Angle: %d, speed: %d\n", data.angle, data.speed);

                //(FENRIS24) Systemet sover i 0.1 sekund før tråden kjøres igjen. Sove-perioden må kalibreres som et forhold til hvor ofte den tidsbaserte interrupten triggres.
                k_msleep(10);
        }
}

void continous_sensor_thread(){
        while(1){

                //(FENRIS24) Henter sensordata fra CANBUS.c
                sensorRead = get_sensor_byte();

                //(FENRIS24) Printer sensorverdi til skjerm. Brukt til test.
                //(FENRIS24) printk("Sensorverdi: %d\n", (int)sensorRead);

                //(FENRIS24) Systemet sover i 0.01 sekund før tråden kjøres igjen. Sove-perioden kalibreres som et forhold av tidsbasert interrupt.
                k_msleep(10);
        }
}

void interruptThreadBegin(){
        //(FENRIS24) Sender melding til seriell monitor om at timed-interrupt initialiseres og navn på det konfigurerte brettet.
        printk("Timed interrupt initialized! %s\n", CONFIG_BOARD);
	
    //(FENRIS24) Oppsettet til timeren:
	#if defined(__ZEPHYR__)
        //(FENRIS24) IRQ = interrupt request. Her kobles opp en timet interrupt request, med interrupt vektor av lavest prioritet.
    	IRQ_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TIMER_INST_GET(TIMER_INST_IDX)), IRQ_PRIO_LOWEST,
                NRFX_TIMER_INST_HANDLER_GET(TIMER_INST_IDX), 0, 0);
	#endif
	
        //(FENRIS24) Henter timerinstansen
	    nrfx_timer_t timer_inst = NRFX_TIMER_INSTANCE(TIMER_INST_IDX);

        //(FENRIS24) Henter basefrekvensen til timerinstansen
 	    uint32_t base_frequency = NRF_TIMER_BASE_FREQUENCY_GET(timer_inst.p_reg);

        //(FENRIS24) Konfigurerer standardkonfigurasjon for timer
        nrfx_timer_config_t config = NRFX_TIMER_DEFAULT_CONFIG(base_frequency);
        config.bit_width = NRF_TIMER_BIT_WIDTH_32; // Bitbredde lik 32 bits
        config.p_context = "My context";

        //(FENRIS24) Inititialiserer timer med konfigurasjon og angir funksjon som skal kjøres ved interrupt
        nrfx_err_t status = nrfx_timer_init(&timer_inst, &config, timer_handler);
            if (status != NRFX_SUCCESS) {
            printk("Timer initialization failed: %d\n", status);
        }

        //(FENRIS24) Sjekk om initaliseringen var vellykket
        NRFX_ASSERT(status == NRFX_SUCCESS);

        //(FENRIS24) Funksjon som nullstiller timeren
        nrfx_timer_clear(&timer_inst);

        //(FENRIS24)
        uint32_t desired_ticks = nrfx_timer_ms_to_ticks(&timer_inst, TIME_TO_WAIT_MS);
    
    
     
        //(FENRIS24) Funksjon som setter opp en timerkanal. Kanalen sammenligner ønsket "ticks" med internt tallregister.
        //(FENRIS24) Når tallregistrert er likt antall ticks vil systemet interruptes.
        nrfx_timer_extended_compare(&timer_inst, NRF_TIMER_CC_CHANNEL0, desired_ticks,
                                NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

        //(FENRIS24)Funksjon som aktiverer timeren
        nrfx_timer_enable(&timer_inst);
}

void start_js_thread(){

    //(FENRIS24) Printer til skjerm at initialisering av tråden har startet. Brukt til testing
    //(FENRIS24) printf("Joystick-thread started!");

    if(js_thread_id == NULL){

        //(FENRIS24) Lager en ny variabel som er lik en tråd med parametere: trådvariabel av tråddatatype, hvilken stack den skal bruke, strørrelsen på stacken, 
        //(FENRIS24) hvilken funksjon som skal kjøre, og prioritet.
        k_tid_t js_thread_id = k_thread_create(&process_thread2, joystick_stack,
                                           K_THREAD_STACK_SIZEOF(joystick_stack),
                                           continous_js_thread, NULL, NULL,
                                           NULL, PROCESS_THREAD_PRIORITYJOY, 0,
                                           K_NO_WAIT);
    
        //(FENRIS24) Hvis tråden ikke er lik NULL, altså at tråd_id har blitt satt lik tråden, printes en suksessmelding til seriell monitor.
        if (js_thread_id != NULL) {
            printk("Joystick thread created successfully\n");
        } else {
            //(FENRIS24) Hvis tråden er lik NULL, altså at tråd_id ikke har blitt satt lik tråden, printes en feilmelding til seriell monitor.
            printk("Failed to create Joystick thread\n");
        }
    }
}

void start_sensor_thread(){

    //(FENRIS24) Når initialiseringen skjer printes det til skjerm. Brukt til testing
    //(FENRIS24) printf("Sensor-thread started!");

    if (sensor_thread_id == NULL){

        //(FENRIS24) Lager en ny variabel som er lik en tråd med parametere: trådvariabel av tråddatatype, hvilken stack den skal bruke, strørrelsen på stacken, 
        //(FENRIS24) hvilken funksjon som skal kjøre, og prioritet.
        k_tid_t sensor_thread_id = k_thread_create(&process_thread3, sensor_stack,
                                               K_THREAD_STACK_SIZEOF(sensor_stack),
                                               continous_sensor_thread, NULL, NULL,
                                               NULL, PROCESS_THREAD_PRIORITYSENS, 0,
                                               K_NO_WAIT);
        //(FENRIS24) Hvis tråden ikke er lik NULL, altså at tråd_id har blitt satt lik tråden, printes en suksessmelding til seriell monitor.
        if (sensor_thread_id != NULL) {
            printk("Sensor thread created successfully\n");
        } else {
            //(FENRIS24) Hvis tråden er lik NULL, altså at tråd_id ikke har blitt satt lik tråden, printes en feilmelding til seriell monitor.
            printk("Failed to create Sensor thread\n");
        }
    }
}




