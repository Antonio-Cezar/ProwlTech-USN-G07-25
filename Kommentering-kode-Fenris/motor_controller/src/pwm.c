#include <stdint.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h> 
 
#include <stdio.h>
#include <stdint.h>
#include "pwm.h"

// Globale variabler som kan settes lik PWM-oppsettet fra app.overlay filen.
// Det trengs 4 forskjellige enheter som kan holde informasjonen om hver kanal
// og GPIO oppsett
static const struct device *pwm0;
static const struct device *pwm1;
static const struct device *pwm2;
static const struct device *pwm3;

// Initialiserings-funksjon hvor globale enhetsvariabler settes lik informasjon nødvendig
// for å kunne generere signalet.
int pwm_begin()
{	
	//Setter global enhetsvariabel pwm0 lik konfigureringen som har label = PWM_0.
	pwm0=device_get_binding("PWM_0");

	// Hvis den ikke finner label med gitt navn, er variabelen lik NULL. Dette indikerer
	// at initialiserings-prosessen feilet.
	if (pwm0 == NULL)
	{
		//feilmelding printes til skjerm
		printf("Error acquiring PWM0 interface \n");
		return -1;
	}
    
	//Setter global enhetsvariabel pwm1 lik konfigureringen som har label = PWM_1.
    pwm1=device_get_binding("PWM_1");
	if (pwm1 == NULL)
	{
		printf("Error acquiring PWM1 interface \n");
		return -2;
	}

	//Setter global enhetsvariabel pwm2 lik konfigureringen som har label = PWM_2.
    pwm2=device_get_binding("PWM_2");
	if (pwm2 == NULL)
	{
		printf("Error acquiring PWM2 interface \n");
		return -3;
	}
	
	//Setter global enhetsvariabel pwm3 lik konfigureringen som har label = PWM_3.
    pwm3=device_get_binding("PWM_3");
	if (pwm3 == NULL)
	{
		printf("Error acquiring PWM3 interface \n");
		return -4;
	}
	return 0;
}


// pwm_write er en funksjon som er ment til å være enkel å bruke. I PWM.h har hver motor fått tildelt et heltall fra 0-3 som
// korrelerer med kanalen den skal bruke.
// Funksjonen er dermed ment til å se hvilket heltall/motor som sendes inn i funksjonen, og deretter velge hvilke kanal 
// konfigurasjon som skal velges.
// Variabelen "value" som sendes inn med funksjonskallet er duty-cycle verdien ønsket skalert i forhold til perioden på PWM-signalet.
void pwm_write(int motor_pin, uint32_t value)
{
    //Her velges fremre venstre motor
    if (motor_pin == 0){
		//pwm_set er en funksjon fra biblioteket i zephyr/drivers/pwm.h.
		//Det funksjonen trenger for å generere et PWM-signal er:
		// - Hvilken perifer, kanal og GPIO konfigurering som skal brukes
		// - Hvilken kanal som skal skal brukes
		// - Definere perioden til PWM-signalet (40 000 000 nano sekunder)
		// - Definere hvor mye av PWM-signalet som skal være høyt (duty cycle)
		// - Litt usikker på hva denne betyr, men har noe med signal typen å gjøre.
        pwm_set(pwm0, (uint32_t)motor_pin,PWM_PERIOD_NS,value,PWM_POLARITY_NORMAL);
    }
	//Her velges fremre høyre motor
    else if (motor_pin == 1){
        pwm_set(pwm1, (uint32_t)motor_pin,PWM_PERIOD_NS,value,PWM_POLARITY_NORMAL);
    }
	//Her velges bakre venstre motor
    else if (motor_pin == 2){
        pwm_set(pwm2, (uint32_t)motor_pin,PWM_PERIOD_NS,value,PWM_POLARITY_NORMAL);
    }
	//Her velges bakre høyre motor
    else if (motor_pin == 3){
        pwm_set(pwm3, (uint32_t)motor_pin,PWM_PERIOD_NS,value,PWM_POLARITY_NORMAL);
    }
}