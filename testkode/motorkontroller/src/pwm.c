#include <stdint.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h> 
 
#include <stdio.h>
#include <stdint.h>
#include "pwm.h"
static const struct device *pwm0;
static const struct device *pwm1;
static const struct device *pwm2;
static const struct device *pwm3;
int pwm_begin()
{
	// Configure the GPIO's 	
	pwm0=device_get_binding("PWM_0");
	if (pwm0 == NULL)
	{
		printf("Error acquiring PWM0 interface \n");
		return -1;
	}
    

    pwm1=device_get_binding("PWM_1");
	if (pwm1 == NULL)
	{
		printf("Error acquiring PWM1 interface \n");
		return -2;
	}

    pwm2=device_get_binding("PWM_2");
	if (pwm2 == NULL)
	{
		printf("Error acquiring PWM2 interface \n");
		return -3;
	}
	
    pwm3=device_get_binding("PWM_3");
	if (pwm3 == NULL)
	{
		printf("Error acquiring PWM3 interface \n");
		return -4;
	}
	return 0;
}
void pwm_write(int motor_pin, uint32_t value)
{
    // Set PWM signal for the specified motor pin
    //pwm_pin_set_usec(pwm, motor_pin, PWM_PERIOD_NS, value);
    if (motor_pin == 0){
        pwm_set(pwm0, (uint32_t)motor_pin,PWM_PERIOD_NS,value,PWM_POLARITY_NORMAL);
    }
    else if (motor_pin == 1){
        pwm_set(pwm1, (uint32_t)motor_pin,PWM_PERIOD_NS,value,PWM_POLARITY_NORMAL);
    }
    else if (motor_pin == 2){
        pwm_set(pwm2, (uint32_t)motor_pin,PWM_PERIOD_NS,value,PWM_POLARITY_NORMAL);
    }
    else if (motor_pin == 3){
        pwm_set(pwm3, (uint32_t)motor_pin,PWM_PERIOD_NS,value,PWM_POLARITY_NORMAL);
    }
}