/*******************************************************************************\
| main.c
| Author: Todd Sukolsky
| ID: U50387016
| Initial Build: 12/20/2012
| Last Revised: 12/26/12
| Copyright of Todd Sukolsky
|================================================================================
| Description: Code for the Atmega168A-ND PDIP chip used for breadboards. Used 
|			   for testing purposes.
|--------------------------------------------------------------------------------
| Revisions: 12/20-23: Initial build, project was to test functionality of 168 on 
|					   Bread Board. First toggled LED, then counted LEDS from 0-15
|					   in binary, then made a function that pulses back and forth.
|			 12/26:		Removed old LED toggle code, working with ADC converter 
|						for stagnant room temp. Should be using 3.3V comparator.
|						-Got readings and temporary conversion, NTC so as it gets warmer, resistance goes UP
|						 , therefore it needs to be on positive rail OR inversion factor. Need lookup table
|						   in EEPROM probably.
|
|================================================================================
| *NOTES:
\*******************************************************************************/

#include <inttypes.h>
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdtypes.h"
#include "atmega168A.h"

#define DEAD
//#define DEBUG
//Baud defines
#define FOSC 8000000					//using internal 8MHz crystal with no clock divide
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD - 1	//declares baud rate

//Temperature defines, need to be changed
#define CONVERTER_INT 11
#define CONVERTER_DEC1 6
#define CONVERTER_DEC2 4

//Wrong
#define __startADCconversion() ADCSRA |= (1 << ADSC);  //starts ADC conversion
/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

WORD adc_reading;
BOOL flag_adc_done = fFalse;

/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */

void DeviceInit (void);
void AppInit (unsigned int ubrr); //initializes UART, LEDs, other things
void Wait_ms(WORD delay);
void PutUart0Ch(char ch);
void Print0(char string[]);
void convertTEMP(WORD rawTemp);

/* ------------------------------------------------------------ */
/*				Interrupt Service Routines						*/
/* ------------------------------------------------------------ */

SIGNAL (SIG_ADC) {
		adc_reading = ADCL;
		adc_reading |= (ADCH << 8);
		flag_adc_done = fTrue;
}

/*****************************************************************************************************************/
int main(void)
{	
	//unsigned int delay;
	DeviceInit();
	AppInit(MYUBRR);
	
	// main program loop
	while (fTrue) {		
		prtLED |= (1 << bnLED3);
		__startADCconversion();
		while(!flag_adc_done) {asm volatile("nop");}
		prtLED &= ~(1 << bnLED3);
		cli();
		#ifdef DEBUG
			char adc_output[6];
			utoa(adc_reading,adc_output,10);
			adc_output[5] = ' ';
			adc_output[6] = '\0';
			Print0("ADC Conversion=");
			Print0(adc_output);
		#endif
		convertTEMP(adc_reading);
		Wait_ms(5000);
		adc_reading = 0;
		flag_adc_done = fFalse;
		sei();
	}  //end while fTrue
} // end main()


/**********************************************************************************************************************************/
void DeviceInit(void)
{
	// Default all i/o ports to input with pull-ups enabled	
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;

	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;
}
/**********************************************************************************************************************************/
#ifdef DEAD
void AppInit(unsigned int ubrr)
{
	//initialize stuff for UART
	UBRR0L = ubrr;   											//set low byte of baud rate
	UBRR0H = (ubrr >> 8);										//set high byte of baud rate
	//enable tx using normal clock
	UCSR0B = (1 << TXEN0)|(1 << RXEN0);
	UCSR0C = (1 << UCSZ01)|(1 << UCSZ00);							//8 data bits, no parity
	
	//Init ADC temp conversion
	ADMUX = 0; //set it to 0 initially
	ADMUX |= (1 << REFS0); //select AVCC as source
	ADCSRA |= (1 << ADEN)|(1 << ADIE)|(1 << ADPS2);
	
	//INit LEDS
	ddrLED = (1 << bnLED3);
	sei();
}

/**********************************************************************************************************************************/																											
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
/**********************************************************************************************************************************/

void PutUart0Ch(char ch)
{
	while (! (UCSR0A & (1 << UDRE0)) ) { asm volatile("nop"); }
	UDR0 = ch;
}
/**********************************************************************************************************************************/
void Print0(char string[])
{	
	BYTE i;
	i = 0;

	while (string[i]) {
		PutUart0Ch(string[i]);  //send byte		
		i += 1;
	}
}

#endif

/************************************************************************/

void convertTEMP(WORD rawTemp){

	WORD wTemp;
	BYTE len2 = 0;
	
	//Initialize all the strings to 0 to ensure good data
	char liveTempString[6];											//voltage buffer
	char bufferString[5];											//buffer
	char stringEnds[11];
	strcpy(stringEnds," degrees. ");									//default endings
	
	//Convert ADC reading
	wTemp = ((CONVERTER_INT) * rawTemp);									//Convert ADC reading into actual voltage
	wTemp = ((rawTemp/10)*CONVERTER_DEC1) + ((rawTemp/100)*CONVERTER_DEC2) + wTemp;
	utoa((unsigned) wTemp,liveTempString,10);						//Places wTemp into a string
	
	//Move voltage into buffer string
	bufferString[0] = liveTempString[0];
	bufferString[1] = liveTempString[1];
	bufferString[2]	= '.';
	bufferString[3] = liveTempString[2];
	
	Print0(bufferString);
	Print0(stringEnds);
}


/************************************************************************\
						Old AVR Code
\************************************************************************/
/*
void LEDarray(unsigned int ledSequence){
	prtLED = 0;	//clear whatever was on the port with LED1,2,3
	prtLED0 &= ~(1 << bnLED0);	//clear whatever was showing on LED0
	unsigned int led0, led1, led2, led3, led4;
	led4 = 16; led3 = 8; led2 = 4; led1 = 2; led0 = 1;
if (led4&ledSequence) {prtLED |= (1 << bnLED4);}
	if (led3&ledSequence) {prtLED |= (1 << bnLED3);}
	if (led2&ledSequence) {prtLED |= (1 << bnLED2);}
	if (led1&ledSequence) {prtLED |= (1 << bnLED1);}
	if (led0&ledSequence) {prtLED0 |= (1 << bnLED0);}
}
*/


/*In appInit	
	ddrLED |= (1 << bnLED1)|(1 << bnLED2)|(1 << bnLED3)|(1 << bnLED4);	//state that it's an output
	prtLED = 0; //should set all to pull up
	ddrLED0 |= (1 << bnLED0);	//set PB0 as output
	prtLED0 &= ~(1 << bnLED0);	//no pull up on LED4
	sei(); //enable interrupts
	*/

/*		//Show all 5 bit numbers
		for (int i = 0; i < 32; i++){
			LEDarray(i);
			Wait_ms(500);
		}
		//Streak from .5 Hz to fast, then back down, then wait
		delay = 150;
		up = fFalse;
		down = fFalse;
		while (!down || !up) {
			for (int j = 1; j <9 ; j++) {
				if (j == 1) {LEDarray(1);}
				if (j == 2 || j == 8) {LEDarray(2);}
				if (j == 3 || j == 7) {LEDarray(4);}
				if (j == 4 || j == 6) {LEDarray(8);}
				if (j == 5) {LEDarray(16);}
				Wait_ms(delay);
			}
			if (delay > 25 && !down) {delay -= 25;}
			else if (delay < 25) {delay += 10; down = fTrue;}
			else if (delay >= 25 && down && delay <= 150) {delay += 25;}
			else if (delay > 150) {up = fTrue;}
			else {delay -= 10;}
		}		
		cli();										  //disable interrupts	
		Print0("Hello Beagle Bone! ");
		*/