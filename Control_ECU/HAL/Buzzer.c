/* ==================================================================
 * Module: Buzzer Driver
 *
 * File Name: Buzzer.c
 *
 * Description: Source File for Buzzer driver.
 *
 * Author: Bilal Mohamed El-Qaqei
 *
 * Date: 20/06/2023
 * ====================================================================*/

#include "../MCAL/gpio.h"
#include "Buzzer.h"

/*
 * Function Name: BUZZER_init
 * Description: Setup Buzzer_pin as output pin  and turn off the buzzer
 */
void BUZZER_init(void)
{
	//Setup the direction for the buzzer pin as output pin through the GPIO driver
	GPIO_setupPinDirection(BUZZER_PORT,BUZZER_PIN, PIN_OUTPUT);

	//Turn off the buzzer through the GPIO.
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);
}

/*
 * Function Name: BUZZER_On()
 * Description :  enable the Buzzer through the GPIO
 */
void BUZZER_on(void)
{
	GPIO_writePin(BUZZER_PORT , BUZZER_PIN , LOGIC_HIGH);
}

/*
 * Function Name: BUZZER_Off()
 * Description:  disable the Buzzer through the GPIO.
 */
void BUZZER_off(void)
{
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);
}
