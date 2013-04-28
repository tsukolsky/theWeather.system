/*******************************************************************************\
| theWeather.system_tiny.cpp
| Author: Todd Sukolsky
| Collaborator: Michael Gurr
| Initial Build: 12/28/12
| Last Revised: 4/24/2013
| Created by Todd Sukolsky
|================================================================================
| Description:	This header file contains symbol declarations describing ports and
|				pins for access to the on-board i/o devices and interface connector
|				pins on the ATMEGA168A on the SolarBoard
|--------------------------------------------------------------------------------
| Revisions: 7/31--Created
|			4/24: Added code for PCINT3 to be enabled, rising edge causes a hit.
|				  Added ADC initializations. Thought we could communicate SPI to the
|				  ATmega324PA, but that was false. It needs to be done on an I2C bus, joy.
|================================================================================
| *NOTES:
\*******************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdtypes.h"
#include "ATtiny84A.h"
#include "tinyTWI.h"

#define FOSC 8000000					//using internal 8MHz crystal with no clock divide
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD - 1	//declares baud rate

#define __startADCconversion() {flagWaitingForADC=fTrue; ADCSRA |= (1 << ADSC);} 
/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

BOOL flagWaitingForADC=fFalse;

/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */

void DeviceInit (void);
void AppInit (unsigned int ubrr); //initializes UART, LEDs, other things
void Wait_ms(volatile WORD delay);

/* ------------------------------------------------------------ */
/*				Interrupt Service Routines						*/
/* ------------------------------------------------------------ */

ISR(PCINT0_vect){
	cli();
	if (PINA & (1 << PCINT3)){
		//Take reading and load data.
		//__startADCconversion();
	}
	sei();
}

ISR(ADC_vect){
	flagWaitingForADC=fFalse;
}
/*****************************************************************************************************************/

int main(void)
{
	DeviceInit();
	AppInit(MYUBRR);
	BYTE sendByte=0x05, receivedByte=0x00;
	// main program loop
	while (fTrue) {
		SendToMega(sendByte);

	}  //end while fTrue
} // end main()

/**********************************************************************************************************************************/
void DeviceInit(void)
{
	// Default all i/o ports to input with pull-ups enabled	
	DDRA = 0;
	DDRB = 0;
	PORTA = 0xFF;
	PORTB = 0xFF;
}
/**********************************************************************************************************************************/
void AppInit(unsigned int ubrr)
{
	//Initialize I2C
	TWI_init_slave();
	
	//Initialize ADC
	ADMUX = 0x00;							//Uses reference 3.3V, ADC0
	ADCSRA = (1 << ADIE)|(1 << ADPS2)|(1 << ADPS0);		//enable global interrupt, clk divide of 32
	
	//Initialize PCINT3
	GIMSK = (1 << PCIE0);		//enable interrupts from PCINT[0:7]
	PCMSK0 = (1 << PCINT3);		//enable pin change interrupt on PCINT3
	
	sei(); //enable interrupts
}

/**********************************************************************************************************************************/		
void Wait_ms(volatile WORD delay)
{	
	volatile WORD i;

	while(delay > 0){
		for( i = 0; i < 400; i++);
		delay -= 1;
	}
}

/************************************************************************/
