/*
 * PWM0.h
 *
 *  Created on: Oct 29, 2023
 *      Author: BILAL
 */

#ifndef PWM0_H_
#define PWM0_H_

#include "std_types.h"

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description:
 * Generate a PWM signal with frequency is required
 * Timer0 will be used with pre-scaler F_CPU/8
 * F_PWM=(F_CPU)/(256*N) = (10^6)/(256*prescaler) = required
 * Duty Cycle can be changed by updating the value
 * in The Compare Register
 */
void PWM_Timer0_Start(uint8 duty_cycle);

/* Description:
 * Control the DC Motor direction using L293D H-bridge.
 * Control The DC Motor Speed using PWM from MC. connect the
 * PWM PIN of Timer0 OC0(PB3) to the enable of the Motor.
 * duty cycle 100% --> Maximum Speed
 * duty cycle 50%  --> Half Maximum Speed
 * duty cycle 0%   --> Motor Stop
 */


#endif /* PWM0_H_ */
