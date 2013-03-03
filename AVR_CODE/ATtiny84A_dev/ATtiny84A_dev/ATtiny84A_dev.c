
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdtypes.h"
#include "atTiny84A.h"

#define DEAD
#define DEBUG

#define FOSC 8000000					//using internal 8MHz crystal with no clock divide
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD - 1	//declares baud rate

#define __toggleLED() pinLED |= (1 << bnLED) //toggles current LED value
/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */


/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */

void DeviceInit (void);
void AppInit (unsigned int ubrr); //initializes UART, LEDs, other things
void Wait_ms(WORD delay);
void PutUart0Ch(char ch);
void Print0(char string[]);

/* ------------------------------------------------------------ */
/*				Interrupt Service Routines						*/
/* ------------------------------------------------------------ */


/*****************************************************************************************************************/
int main(void)
{
	DeviceInit();
	AppInit(MYUBRR);
	
	// main program loop
	while (fTrue) {
		sei();
		Wait_ms(1000);
		prtLED &= ~(1 << bnLED);
		Wait_ms(1000);
		prtLED |= (1 << bnLED);
		cli();										  //disable interrupts	
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
#ifdef DEAD
void AppInit(unsigned int ubrr)
{
	//initialize stuff for UART
	
	//Initialize LED outputs
	ddrLED |= (1 << bnLED); //set power LED to output
	prtLED |= (1 << bnLED);	//turn it on initially
	sei(); //enable interrupts
}

/**********************************************************************************************************************************/																												*/
void Wait_ms(WORD delay)
{	
	WORD i;

	while(delay > 0){
		for( i = 0; i < 400; i++){
			asm volatile("nop");
		}
		delay -= 1;
	}
}
#endif

/************************************************************************/
