/******************************************************************************
 *
 * Module: Timer1
 *
 * File Name: timer1.c
 *
 * Description: Source file for the LCD driver
 *
 * Author: Bilal Mohamed El-Qaqei
 *
 *******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "common_macros.h"  /* To use the macros like SET_BIT */
#include "timer1.h"


/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;


/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description : Function to initialize the Timer1 driver
 * 	 Initialize Timer1 Registers
 */

void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	TCNT1 = Config_Ptr -> initial_value;   // Set Timer0 initial value

	/* Configure the timer control register
	 * Non PWM mode FOC1A =1 , FOC1B =1 (always)
	 * of TCCR1A Register
	 */
	TCCR1A |= (1<<FOC1A) | (1<<FOC1B);
	/*
	 * insert the required clock value in the first three bits (CS10, CS11 and CS12)
	 * of TCCR1B Register
	 */
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->prescaler);

	if(Config_Ptr -> mode == NORMAL_Mode)
	{
		/* 2. Normal Mode WGM10=0 & WGM11=0 & WGM12=0 & WGM13=0
		 * 3. Normal Mode COM1A0/COM1B0=0 & COM1A1/COM1B1=0
		 * TCCR0 register equal zero by default
		 */
		TIMSK = (1<< TOIE1); // Enable Timer1 Overflow Interrupt
	}
	else if(Config_Ptr -> mode == CTC_Mode)
	{
		/*CTC Mode WGM10=0 & WGM11=0 & WGM12=1 &  WGM13=0*/
		TCCR1B |= (1<< WGM12);

		TIMSK |= (1<<OCIE1A); // Enable Timer1 Compare Interrupt

		OCR1A = Config_Ptr -> compare_value; // Set Compare Value
	}
}

/*
 * Description: Function to disable the Timer0 to stop the Driver
 */
void Timer1_deInit(void)
{
	/* Clear All Timer1 Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	TIMSK = 0; // Disable Timer1 Interrupt
}

void Timer1_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}
