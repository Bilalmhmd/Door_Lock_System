 /******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: DC_Motor.h
 *
 * Description: Header file for the DC Motor
 *
 * Author: Bilal Mohamed El-Qaqei
 *
 *******************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "../MCAL/std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PORT_INPUTS          PORTB_ID
#define PIN_INPUT_1          PIN1_ID
#define PIN_INPUT_2          PIN2_ID


/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum
{
	 STOP_DcMotor,A_CW_DcMotor,CW_DcMotor
}DcMotor_State;


/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description :
 * setup the direction for the two motor pins
 * Stop at the DC-Motor at the beginning
 */
void DcMotor_Init(void);


/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value.
 * Send the required duty cycle to the PWM driver based on the required speed value.
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* DC_MOTOR_H_ */
