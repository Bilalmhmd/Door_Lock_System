/******************************************************************************
 *
 * Module: APPlication
 *
 * File Name: main.c
 *
 * Description: Application of Control_ECU
 *
 * Author: Bilal Mohamed El-Qaqei
 *
 *******************************************************************************/
#include "../HAL/Buzzer.h"
#include "../HAL/External_EEPROM.h"
#include "../HAL/DC_MOTOR.h"
#include "../MCAL/timer1.h"
#include "../MCAL/TWI.h"
#include "../MCAL/UART.h"
#include <util/delay.h>
#include <avr/io.h>

/*************************************************************************
 * 						Definitions                                      *
 *************************************************************************/
#define PASS_SIZE					 5
#define PASS_IS_TRUE				0x10
#define PASS_IS_FALSE				0x01
#define MC2_READY     				0xFF
#define MC2_READY_TO_GET_USER_PASS 	0xee
#define USER_PASS_IS_TRUE 			0x17
#define USER_PASS_IS_FALSE			0x16
#define MC2_WILL_SEND_CHECK		    0x20
#define CHECK_USER_PASS    		    0x55

/*************************************************************************
 * 						Function Prototypes                              *
 *************************************************************************/
void activeBuzzzer(void);
void checkOnPass(uint8 arr1 [], uint8 arr2[] ,uint8 arr3[] );
void myOwnDelay(uint16 sec);
void timerTicks(void);
void openDoor(void);
void systemOperation(void);
void writeEEPROM(uint8 arr4[]);
void checkOnEEPROMpass(uint8 array1[] , uint8 array2[]);
void recievePass(uint8 arr[]);
void systemRun(uint8 userpass[] , uint8 newpass[] ,uint8 samepass[]);

/*************************************************************************
 * 						Flags                                            *
 *************************************************************************/
boolean isPassTrue = FALSE;

boolean isUserPassTrue = FALSE;

/* to know which operation will be run -- open door or change password */
uint8 SystemOperation;
/* To count number of failed*/
uint8 failedAttemp=0;


/*************************************************************************
 * 						Global Variable used in call back function
 *************************************************************************/
static volatile uint8 g_tick=0;

/* ==================================================================================== *
 * 					                 * NOTES *                                          *
 * 1. Using Switch instead of if give me some of speed and decrease CPU load            *
 * 2. Enter button -- '='                                                               *
 * =====================================================================================*/

/*
 * Function Name : main()
 * Description   :implement Control ECU Code
 */
int main(void)
{
	/*Enable i-bit to use interrupt in TIMER1*/
	SREG |=(1<<7);
	/*save USER Password in global array*/
	uint8 userPass[PASS_SIZE];
	/* Save The Passwords in array*/
	uint8 newpass [PASS_SIZE], samePass[PASS_SIZE];
	/***********************************************
	 * 				UART Configurations            *
	 * 1.Even Parity                               *
	 * 2.stop bit --onebit                         *
	 * 3.8 bits Data                               *
	 * 4.Baudrate= 9600                            *
	 ***********************************************/
	UART_ConfigType configurations = { Eight_bits, Even_Parity , One_bit , 9600 };
	UART_init( &configurations);

	/*******************************************************
	 *             I2C Configuration                       *
	 * 1.use 400k bit rate -- TWB=2                        *
	 * 2.Choice address number (4)                         *
	 *******************************************************/
	TWI_ConfigType twi_configurations={0x02,0x04};
	TWI_init(&twi_configurations);

	/* DcMotor initialization*/
	DcMotor_Init();
	/* Buzzer initialization*/
	BUZZER_init();

	/* Send to MC1 that MC2 become Ready  to recieve orders*/
	while(UART_recieveByte() != MC2_READY);
	while(1)
	{
		/*Check if Pass and same pass are taken from HMI_ECU*/
		checkOnPass(newpass ,samePass ,userPass);
		/*Start runing the system*/
		systemRun(userPass,newpass,samePass);
	}


}

/************************************************************************
 * Function Name : recievePass()                                        *
 * Description   : recieve password from HMI_ECU and save it in array   *
 ************************************************************************/
void recievePass(uint8 arr[])
{
	uint8 i;
	for(i=0 ; i< PASS_SIZE ;i++)
	{
		arr[i]=UART_recieveByte();
	}
	/* recieve pass untill enter*/
	while(UART_recieveByte() != '=');
}

/**************************************************************************************************
 * Function Name:checkOnNewPass()                                                                 *
 * Description:                                                                                   *
 * 	Recieve and Check if The two pass are the same or not , if are matched write it in EEPROM     *
 **************************************************************************************************/
void checkOnPass(uint8 arr1 [], uint8 arr2[] ,uint8 arr3[] )
{
	uint8 i;
	recievePass(arr1);		/* recieve pass*/
	recievePass(arr2);		/* recieve confim pass*/

	/* Compare if two pass are matched or not*/
	for(i=0;i<PASS_SIZE;i++)
	{
		if(arr1[i] == arr2[i])
		{
			isPassTrue=TRUE;
		}
		else
		{
			isPassTrue=FALSE;
			break;
		}
	}

	/* Agreement between two ECUs*/
	UART_sendByte(MC2_WILL_SEND_CHECK);

	/* Send The result for HMI_ECU*/
	switch(isPassTrue)
	{
	case TRUE:
		UART_sendByte(PASS_IS_TRUE);	/* Send Result*/
		isPassTrue=FALSE;				/* Clear Flag*/
		writeEEPROM(arr1);				/* Write pass in EEPROM */
		systemOperation();				/*Recieve user option */

		/* Agreement between two ECUs*/
		while(UART_recieveByte() != MC2_READY_TO_GET_USER_PASS);
		/* Recieve User Pass*/
		recievePass(arr3);
		/* Check if equal the pass which write in EEPROM*/
		checkOnEEPROMpass(arr3 ,arr1);
		break;

		/* Case False*/
	case FALSE:
		UART_sendByte(PASS_IS_FALSE);	/*Send Result to HMI_ECU*/
		checkOnPass(arr1,arr2,arr3);	/* Re-call the function to do the same step*/
		break;
	}
}

/******************************************************
 * Function Name : checkOnEEPROMpass()                *
 * Description   : check on password in EEPROM        *
 ******************************************************/
void checkOnEEPROMpass(uint8 array1[] , uint8 array2[])
{
	uint8 i;
	/* First read pass which stored in EEPROM*/
	for(i=0 ; i<PASS_SIZE ;i++)
	{
		EEPROM_readByte((0x0C32)+i,array1+i);
	}
	/* Compare f two pass are the same or not , And Change Flag */
	for(i=0 ;i<PASS_SIZE ;i++)
	{
		if(array1[i]== array2[i])
		{
			isUserPassTrue=TRUE;		/* make flag true*/
		}
		else
		{
			isUserPassTrue=FALSE;		/* Make Flag False*/
			break;
		}
	}
}

/*************************************************************
 * Function Name: systemRun()                                *
 * Description  : Start running of the system                *
 *************************************************************/
void systemRun(uint8 userpass[] , uint8 newpass[] ,uint8 samepass[])
{
	/* To know if flag is true or false*/
	checkOnEEPROMpass(userpass ,newpass);
	/* Agreement between two ECUs*/
	while(UART_recieveByte ()!= CHECK_USER_PASS);
	/*Check on Flag of user password*/
	switch(isUserPassTrue)
	{
	case TRUE:

	{
		UART_sendByte(USER_PASS_IS_TRUE);	/* Send to HMI that the user pass is true*/
		failedAttemp=0;						/* Make failed Attemps =0*/
		/* Check on User choice*/
		switch(SystemOperation)
		{
		case '+' :
			openDoor();			/* turn on dcmotor , unlocking door*/
			/* return to know user option , Re-turn to step take operation from user*/
			systemOperation();
			/* Agreement between two ECUs*/
			while(UART_recieveByte() != MC2_READY_TO_GET_USER_PASS);
			/* take user pass*/
			recievePass(userpass);
			/* start running system again*/
			systemRun(userpass,newpass,samepass);
			break;
		case '-' :
			/* recieve change pass , And start Runnig system again*/
			checkOnPass(newpass,samepass,userpass);

			systemRun(userpass,newpass,samepass);
			break;
		}
	}
	break;
	/* Case False user pass*/
	case FALSE :
		UART_sendByte(USER_PASS_IS_FALSE);		/* Send Result to HMI_ECU*/
		/* increment failed attemps*/
		failedAttemp++;
		/* Check if failed attemps reach to three attemps continous*/
		switch(failedAttemp)
		{
		case 3:
			failedAttemp=0;		/* clear failed attemps*/
			/* Turn on Buzzer for 60 minute*/
			activeBuzzzer();
			/* Re-turb again to know which new operation that user need*/
			systemOperation();
			/* Agreement between two ECUs*/
			while(UART_recieveByte() != MC2_READY_TO_GET_USER_PASS);
			/* take user pass*/
			recievePass(userpass);
			/* start running system again*/
			systemRun(userpass,newpass,samepass);
			break;
		}
		/* Agreement between two ECUs*/
		while(UART_recieveByte() != MC2_READY_TO_GET_USER_PASS);
		/* take user pass*/
		recievePass(userpass);
		/* start running system again*/
		systemRun(userpass,newpass,samepass);
		break;
	}
}

/************************************************************
 * Function Name :systemOperation()                         *
 * Description   : Know the operation from HMI_ECU          *
 ************************************************************/
void systemOperation(void)
{
	/* To save user options in Global variable to use it when starts the system*/
	SystemOperation=UART_recieveByte();
}

/**************************************************************
 * Function Name : writeEEPROM()                              *
 * Description   : Write in EEPROM in Assumption address      *
 **************************************************************/
void writeEEPROM(uint8 arr4[])
{
	uint8 k;
	/* Write pass in EEPROM*/
	for(k=0 ;k<PASS_SIZE ;k++)
	{
		EEPROM_writeByte((0x0C32)+k ,arr4[k]);
	}

}

/**************************************
 * Function Name :openDoor()          *
 * Description   : Turn on Dc-Motor   *
 **************************************/
void openDoor(void)
{
	/* Turn on DC motor in Maximum speed in CW*/
	DcMotor_Rotate(CW_DcMotor ,255);
	/* count 15 sec from timer1*/
	myOwnDelay(15);
	/* turn off DcMotor*/
	DcMotor_Rotate(STOP_DcMotor ,0);
	/* count 3 sec fro timer 1*/
	myOwnDelay(3);
	/* Turn on DC motor in Maximum speed in CCW*/
	DcMotor_Rotate(A_CW_DcMotor ,255);
	/* count 15 sec from timer1*/
	myOwnDelay(15);
	/* turn off DcMotor*/
	DcMotor_Rotate(STOP_DcMotor ,0);
}

/**********************************************
 * Function Name : activeBuzzzer()            *
 * Description   : Active buzzer for 1 minute *
 **********************************************/
void activeBuzzzer(void)
{
	/* Start turn on Buzzer*/
	BUZZER_on();
	/* Count 60 sec from timer1*/
	myOwnDelay(60);
	/* Turn off Buzzer again*/
	BUZZER_off();
}

/***********************************************************
 * Function Name : timerTicks()                            *
 * Description: Just increment num. of ticks in timer 1s   *
 ***********************************************************/
void timerTicks(void)
{
	g_tick++;
}

/*************************************************************************************
 * Function Name : myOwnDelay()                                                      *
 * Description   : initialize timer1 and count until to sec which is input from user *
 *************************************************************************************/
void myOwnDelay(uint16 sec)
{
	g_tick=0;

	/***********************************************
	 * 				TIMER1 Configurations          *
	 * 1.Initial value = 0                         *
	 * 2.compare value(in ctc mode) not used = 0   *
	 * 3.prescaler = 64                            *
	 * 4.mode of operation = Normal                *
	 ***********************************************/
	Timer1_ConfigType Timer1_Confige = { 0 ,0 , F_CPU_64 , NORMAL_Mode};
	Timer1_init(&Timer1_Confige);

	/* Used call back which call in ISR for time1*/
	Timer1_setCallBack(timerTicks);

	/* Delay */
	while(g_tick < (2*sec));

	/* Deinitialization of timer1*/
	Timer1_deInit();

}

