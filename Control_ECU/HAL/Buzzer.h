/* ==================================================================
 * Module: Buzzer Driver
 *
 * File Name: Buzzer.h
 *
 * Description: Header File for Buzzer driver.
 *
 * Author: Bilal Mohamed El-Qaqei
 *
 * Date: 20/06/2023
 * ====================================================================*/
#ifndef BUZZER_H_
#define BUZZER_H_
#include "../MCAL/std_types.h"

/************************************************
 * 				Definitions
 ***********************************************/
#define  BUZZER_PORT		PORTD_ID
#define	 BUZZER_PIN			PIN3_ID

/************************************************
 * 				Prototypes
 ***********************************************/
void BUZZER_init(void);
void BUZZER_on(void);
void BUZZER_off(void);

#endif /* BUZZER_H_ */
