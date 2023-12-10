 /******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: DC_Motor.c
 *
 * Description: Source file for the DC Motor
 *
 * Author: Bilal Mohamed El-Qaqei
 *
 *******************************************************************************/

#include "DC_Motor.h"
#include "../MCAL/gpio.h"
#include "../MCAL/std_types.h"
#include "../MCAL/PWM0.h"

/*
 * Description :
 * setup the direction for the two motor pins
 * Stop at the DC-Motor at the beginning
 */

void DcMotor_Init(void)
{
	//setup the direction for the two motor pins
	GPIO_setupPinDirection(PORT_INPUTS, PIN_INPUT_1, PIN_OUTPUT);
	GPIO_setupPinDirection(PORT_INPUTS, PIN_INPUT_2, PIN_OUTPUT);

	//Stop at the DC-Motor at the beginning
	GPIO_writePin(PORT_INPUTS, PIN_INPUT_1, LOGIC_LOW);
	GPIO_writePin(PORT_INPUTS, PIN_INPUT_2, LOGIC_LOW);
}

/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value.
 * Send the required duty cycle to the PWM driver based on the required speed value.
 */

void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{

	switch(state)
	{
	case STOP_DcMotor:
		/* Stop DC Motor */
		GPIO_writePin(PORT_INPUTS, PIN_INPUT_1, LOGIC_LOW);
		GPIO_writePin(PORT_INPUTS, PIN_INPUT_2, LOGIC_LOW);
		break;
	case CW_DcMotor:
		/* Clockwise Rotation */
		GPIO_writePin(PORT_INPUTS, PIN_INPUT_1, LOGIC_LOW);
		GPIO_writePin(PORT_INPUTS, PIN_INPUT_2, LOGIC_HIGH);
		break;
	case A_CW_DcMotor:
		/* Anti Clockwise Rotation */
		GPIO_writePin(PORT_INPUTS, PIN_INPUT_1, LOGIC_HIGH);
		GPIO_writePin(PORT_INPUTS, PIN_INPUT_2, LOGIC_LOW);
		break;
	}
	/* Send the required duty cycle to the PWM driver based on the required speed value */
	PWM_Timer0_Start(speed);
}
