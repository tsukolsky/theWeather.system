/*******************************************************************************\
| theWeather.system_mega.cpp
| Author: Todd Sukolsky
| Collaborator: Michael Gurr
| Initial Build: 1/3/2013
| Last Revised: 4/27/13
| Copyright of Todd Sukolsky and Michael Gurr
|================================================================================
| Description: This is the main program for the ATmega324PA, project theWeather.system.
|		This is responsible, in Rev A, for taking temperatuer readings from a thermistor,
|		humidity readings, and two SPI by Wire temperature readings from PCB chips. For
|		more information see Proposal in the Files folder. 
|--------------------------------------------------------------------------------
| Revisions:
|	1/3: Initial build
|	1/5: Added interrupts and timer functionality.
|	1/6-4/23: Progressed code to the point it is now at pull. Not sure why I didn't comment
|			  things, however I am now.
|	4/24: Changed functionality to a less speratic model. Now this guy will take readings, print,
|		  go to sleep, and wake up after so many timer overflows. This is ideal. Now comes the time
|		  to work on SPI with the ATtiny and I2C with the RTC.
|	4/25: Added longer timeout to function, added UART receive technique and flags to get  certain things.
|		  Added polling and flag set for query requests.
|	4/26: Changed the project into a CPP to allow for classes. Added thermostat, clock, eeprom functionality.
|		  Now the system will not do anything until an interrupt occurs, at which point it will set a flag to
|		  receive from the Pi and then go get the message. Clock is implemented to interface with the RTC
|		  chip on board, can now save humidities. clock saves the day's statistics when at midnight.
|	4/27: Worked with more UART commands. now recognizes "WEEK.", "time.", and can receive a time with format HHMMSS.
|		  On a new day, the week averages are added too. If there is no week data yet, pulls current data for the week.
|		  Makes sure the time is set to 7AM when that happens as well since that's when the script on the Pi triggers this. 	
|	4/28: Slight tweak to PrintWeek protocol  
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
#include <avr/eeprom.h>
#include "stdtypes.h"
#include "ATmega324PA.h"
#include "thermostat.h"
#include "clock.h"
#include "myUart.h"
#include "eepromGurrDaddy.h"
#include "myTWI.h"

/**************Baud defines******************/
#define FOSC 8000000					//using internal 8Mhz crystal, no clock divide
//#define FOSC 14747560					//using external 14.74756 MHz crystal with no clock divide (X2)
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD - 1			//declares baud rate

/**************Sensor Values****************/
#define HUMIDITY_OFFSET		155
#define HUMIDITY_CONVERTER	4.919		//(ADC result -155)/THIS = Humidity percentage * 100, 6.86084
#define THERM_OFFSET		269			//OFFSET to 0 in our linear progression
#define THERM_CONVERTER		9.805		//COnversion factor for ADC to celsius, was 8.875 for 0->40 curve
#define ON_CHIP_CONVERTER	.03125		//.03125 degress Celsius per 1 on ADT and TI temp

/**************Debug LED Value**************/
#define DEBUG_NUMBER 0xF0				//8'b11111000

#define TIMEOUT		60*5				//5 second timeout with UART receptions.
#define SLEEP_TIME  60*5				
#define PULSE_TIME	31
//8MHz/1024=7820/(256)~30=one second.	
/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

WORD sleepCnt=0;									//originally used to map how long the device sleeps, can now be used for downtime?
BOOL flagGoToSleep, flagAllStats,flagReceivePi, flagSendWeek;

clock theClock;
thermostat theThermostat;

/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */

void DeviceInit (void);
void AppInit (unsigned int ubrr); //initializes UART, LEDs, other things
void Wait_ms(WORD delay);
void Wait_sec(WORD delay);
void ResetDebug();
double GetTempADT();
double GetTempTherm();
double GetHumidity();
double GetTempTI(unsigned int ubrr1);
void InitTimer2();

/* ------------------------------------------------------------ */
/*				Interrupt Service Routines						*/
/* ------------------------------------------------------------ */

ISR(INT2_vect){
	cli();
	prtDebug |= (1 << bnD1);	//should toggle amber LED on. 
	Wait_ms(200);
	flagReceivePi=fTrue;
	sei();
}

//used for sleeping.
ISR(TIMER2_OVF_vect){
	cli();
	static WORD timerTwoCount=0,receivePiTimer=0;
	

	if (timerTwoCount++>=PULSE_TIME) {
		theClock.addSecond(1);  
		timerTwoCount = 0;
		pinDebug = (1 << bnD0);
	}		//toggles green, left LED. Should be on half second.
	
	//Receive Timeout	
	if (flagReceivePi && receivePiTimer<TIMEOUT){receivePiTimer++;}								//no timteout yet
	else if (flagReceivePi && receivePiTimer>=TIMEOUT){receivePiTimer=0;flagReceivePi=fFalse;}	//timeout
	else {receivePiTimer=0;}																	//make sure this statys at 0 and is reset if a successful receive happens		
	sei();
}


/*****************************************************************************************************************/
int main(void)
{	
	DeviceInit();
	AppInit(MYUBRR);
	ResetDebug();	//clear LED's
	InitTimer2();
	TWI_init_master();		//make us the master of our own destiny
	sei();
	Wait_sec(2);
	prtDebug|=(1 << bnD4);
	// main program loop
	while (fTrue) {	
		
		//IF receiving from the Pi
		if (flagReceivePi){
			//Print0("Receiving.");
			ReceivePi();
			prtDebug &= ~(1 << bnD1);
		}
		Wait_sec(1);
		
		//Print the week, does implicit things
		if (flagSendWeek){
			theThermostat.takeReadings();
			theThermostat.PrintWeek();
			theClock.setTime(7,0,0);
			flagSendWeek=fFalse;
		}
		
		//If we were asked for stats, send them back.
		if (flagAllStats){
			cli();
			//Print0("Taking readings.");
			prtDebug |= (1 << bnD2);
			char printString[50];
			//Get all the data readings
			double adtTemp=GetTempADT();
			double tiTemp=GetTempTI(MYUBRR);
			double thermTemp=GetTempTherm();
			double humidity=GetHumidity();
			//Add the therm temp to the thermostat class long with the humidity.
			theThermostat.addDataPoint(thermTemp,humidity);
			//Declare strings to convert doubles into
			char adtStr[8],tiStr[8],thermStr[8],humidityStr[8];
			dtostrf(adtTemp,1,2,adtStr);
			dtostrf(thermTemp,1,2,thermStr);
			dtostrf(tiTemp,1,2,tiStr);
			dtostrf(humidity,1,2,humidityStr);
			//Make the output string
			strcpy(printString,"AD");
			strcat(printString,adtStr);
			strcat(printString,"/TI");
			strcat(printString,tiStr);
			strcat(printString,"/TH");
			strcat(printString,thermStr);
			strcat(printString,"/HU");
			strcat(printString,humidityStr);
			//Drop debug indicator light, print the string, exit while clearing the string in memory
			prtDebug &= ~(1 << bnD2);
			Print0(printString);
			Print0("XXX");
			flagAllStats=fFalse;
			int i=0;
			for (i=0;i<50;i++){printString[i]=NULL;}
			sei();
		}		
		
		//Power save. Should sleep for ~10 seconds
		//Print0(" Going to sleep... ");
		prtDebug &= ~(1 << bnD4);
		flagGoToSleep=fTrue;
		sleepCnt=0;
		SMCR = (1 << SM1)|(1 << SM0);
		SMCR |= (1 << SE);
		while (!flagReceivePi){
			asm volatile("SLEEP");
			sleepCnt++;
		}		
		SMCR = 0;
		flagGoToSleep = fFalse;
		prtDebug|=(1 << bnD4);
		Wait_ms(10);
		//Reset sleep register on wakeup
		//Print0(" Waking up... ");
		
	}  //end while fTrue
} // end main()

/**********************************************************************************************************************************/
void DeviceInit(void)
{
	// Default all i/o ports to input with pull-ups disabled 	
	DDRA = 0;
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;

	PORTA = 0x00;
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
}
/**********************************************************************************************************************************/
void AppInit(unsigned int ubrr)
{
	//initialize stuff for UART
	UBRR0L = ubrr;   											//set low byte of baud rate
	UBRR0H = (ubrr >> 8);										//set high byte of baud rate
	//enable tx using normal clock
	UCSR0B = (1 << TXEN0)|(1 << RXEN0);
	UCSR0C = (1 << UCSZ01)|(1 << UCSZ00);							//8 data bits, no parity

	//Initialize Debug LEDS. Check by strobing up and down
	ddrDebug |= (1 << bnD0)|(1 << bnD1)|(1 << bnD2)|(1 << bnD3)|(1 << bnD4);	//Set as outputs
	for (int i = 4; i < 8; i++) {	//Strobe from right to left
		prtDebug |= (1 << i);
		Wait_ms(200);
		ResetDebug();
	}
	for (int j = 7; j >= 4; j--){	//Strobe from left to right
		prtDebug |= (1 << j);
		Wait_ms(200);
		ResetDebug();
	}		
	
	//Initialize SPI for AD Temperature Sensor
	ddrSpi |= (1 << bnMosi)|(1 << bnSck)|(1 << bnSS);				//SS, Sck, MOSI all outputs
	prtSpi |= (1 << bnSS)|(1 << bnSck);								//pull Sck and SS high
	prtSpi &= ~(1 << bnMiso);										//no pull up on Miso, its an input, keep it low
	prtSpi &= ~(1 << bnMosi);										//pull dwon on Mosi, should always be low
	SPCR0 |= (1 << MSTR0)|(1 << SPE0)|(1 << SPR00)|(1 << SPR10);	//enables spi, master mode to talk with AD and fck/128

/*	//RTC initialization
	ddrRTC |= (1 << bnMFP)|(1 << bnSDA)|(1 << bnSCL);
	prtDebug |= (1 << bnMFP)|(1 << bnSDA)|(1 << bnSCL);
*/
	//Initialize power outputs
	PRR0 |= (1 << PRTWI)|(1 << PRTIM2)|(1 << PRTIM1)|(1 << PRTIM0)|(1 << PRUSART1)|(1 << PRADC)|(1 << PRSPI); //Turn EVERYTHING off initially

	//set up interrupts for waking device up through INT2
	EICRA |= (1 << ISC21)|(1 << ISC20);		//rising edge of INT2 triggers asynchronous interrupt
	EIMSK  = (1 << INT2);					//enables interrupts on INT2 as long as global interrupt is set.
	
	//Initialize Booleans
	flagGoToSleep=fFalse;
	flagAllStats=fFalse;
	flagReceivePi=fFalse;
	flagSendWeek=fFalse;
}

/**********************************************************************************************************************************/
void InitTimer2(){
	//Disable global interrupts
	cli();
	//Give timer2 power
	PRR0 &= ~(1 << PRTIM2);	
	Wait_ms(1);	
	//Set up registers	
	TCCR2B |= (1 << CS22)|(1 << CS21)|(1 << CS20);	//1024 prescaler
	while (ASSR & ((1 << TCR2BUB)|(1 << TCN2UB)));	//wait for it not to be busy
	TIFR2 = (1 << TOV2);							//Clear any interrupts pending for the timer
	TIMSK2 = (1 << TOIE2);							//Enable overflow on it
	//Re-enable global interrupts
	sei();
}
/**********************************************************************************************************************************/

double GetTempTI(unsigned int ubrr)
{
	WORD tempDataTI = 0;
	
	//Bring the USART1 back online, reset regitser
	PRR0 &= ~(1 << PRUSART1);	//give it power
	Wait_ms(10);				//Leveling time
	UBRR1 = 0;
	
	//Initialize USART1 pins
	ddrSpi1 |= (1 << bnSpiSck_1)|(1 << bnSpiCs_1);	//Initialize Sck and Cs as outputs
	prtSpi1 |= (1 << bnSpiSck_1)|(1 << bnSpiCs_1);	//Bring them high to initialize
	
	//INitialize functionality
	UCSR1C = (1 << UMSEL11)|(1 << UMSEL10)|(0 << UCPOL1)|(0 << 1);	//Set MSPI mode and SPI data mode 0, pg 202 of ds
	UCSR1B = (1 << RXEN1)|(1 << TXEN1);								//Enable the receiver and transmitter

	//Set Baud rate, 9600, same as SPI
	UBRR1L = ubrr;
	UBRR1H = (ubrr >> 8);
	
	//Wait for empty transmit buffer
	while (!(UCSR1A & (1 << UDRE1)));
	
	//Send CS low
	prtSpi1 &= ~(1 << bnSpiCs_1);
	
	//Write to the buffer, starting a transmission
	UDR1 = 0x00;	
	
	//Wait for it to be received, this is the first byte
	while (!(UCSR1A & (1 << RXC1)));
	tempDataTI = (UDR1 << 8);	//first byte of data
	
	//Write to buffer again, get second byte
	UDR1 = 0x00;
	
	//Wait for it to be received, second byte
	while (!(UCSR1A &(1 << RXC1)));
	tempDataTI |= UDR1;	
	
	//Bring SCK and CS back high
	prtSpi1 |= (1 << bnSpiCs_1)|(1 << bnSpiSck_1);
	
	//Reset all hardware, then discontinue power
	UCSR1C = 0;
	UCSR1B = 0;
	UCSR1A = 0;
	UBRR1L = 0;
	UBRR1H = 0;
	PRR0 |= (1 << PRUSART1);	//turns off power to USART1
	
	//Manipulate tempDataTI
	double dataTI = (tempDataTI/4)*ON_CHIP_CONVERTER*1.8 + 32;			//Div 4 shifts right 2 bits; Converts into celcius, then into Fahrenheit

	return dataTI;	
		
}

/**********************************************************************************************************************************/
double  GetTempTherm()
{
	//Function variables
	WORD tempDataTherm = 0;
	double dataTherm = 0;
	
	//Turn ADC on
	PRR0 &= ~(1 << PRADC);
	ADMUX |= (1 << REFS0)|(1 << MUX1);	//internal 3v3 reference, ADC2
	ADCSRA |= (1 << ADEN)|(1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);		//Enables ADC, 128 clock prescaler
	DIDR0 = (1 << ADC7D)|(1 << ADC6D)|(1 << ADC5D)|(1 << ADC4D)|(1 << ADC3D)|(0 << ADC2D)|(1 << ADC1D)|(1 << ADC0D);	//disable all ADC except for ADC1
	Wait_ms(10);
	
	//Run conversion twice
	for (int i = 0; i < 2; i++){ ADCSRA |= (1 << ADSC); while (ADCSRA & (1 << ADSC));}	//do two conversions, throw first one out
	
	//Put conversion into buffer
	tempDataTherm = ADCL;
	tempDataTherm |= (ADCH << 8);
	
	//Disable ADC hardware/registers
	ADCSRA = 0;
	ADMUX = 0;
	DIDR0 |= (1 << ADC2D);	//disable ADC2D
	
	//Turn off power to ADC
	PRR0 |= (1 << PRADC);
	
	/**Operate to make real temperature in celcius, then Fahrenheit**/
	
	//Get offset of temperature from maximum ADC reading and offset to 0 for our linear progression
	tempDataTherm = 1023 - tempDataTherm - THERM_OFFSET;					//For 0->40 degree C	
	
	//Multiply by conversion factor
	dataTherm = tempDataTherm/THERM_CONVERTER;	//now in celsius
		
	//Convert to Fahrenheit; F = C*(9/5) * 32; 74.4 correlates to 23.555 Celsius
	dataTherm = dataTherm*1.8 + 32;	
	
	return dataTherm;

		
}

/**********************************************************************************************************************************/	
double GetHumidity()
{
	//Function variables
	WORD humidityResult;
	
	//Turn on power to ADC, turn on components
	PRR0 &= ~(1 << PRADC);				//give ADC power again
	Wait_ms(10);						
	ADMUX |= (1 << REFS0)|(1 << MUX0);  //Chooses AVCC as reference, channel ADC1
	ADCSRA |= (1 << ADEN)|(1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);	//Enables ADC, starts conversion, 128 clock prescaler
	DIDR0 = (1 << ADC6D)|(1 << ADC7D)|(1 << ADC5D)|(1 << ADC4D)|(1 << ADC3D)|(1 << ADC2D)|(0 << ADC1D)|(1 << ADC0D);	//disable all ADC except for ADC1
	Wait_ms(10);
	
	//Run conversion twice
	for (int i = 0; i < 2; i++){ ADCSRA |= (1 << ADSC); while (ADCSRA & (1 << ADSC));}	//do two conversions, throw first one out
	
	//Put ADC result into buffer
	humidityResult = ADCL;
	humidityResult |= (ADCH << 8);
	
	//Disable ADC hardware/registers
	ADCSRA = 0;
	ADMUX = 0;
	DIDR0 |= (1 << ADC1D);	//disable ADC1 in DIDR0
	
	//Turn off power to ADC
	PRR0 |= (1 << PRADC);

	//Get actual humidity
	double humidityResultDouble = (double)humidityResult;
	humidityResultDouble -= HUMIDITY_OFFSET;	//155 is 0 for a conversion, then divide the converting factor 
	humidityResultDouble /= HUMIDITY_CONVERTER;	//get actual humidity	
	
	return humidityResultDouble;
}
/**********************************************************************************************************************************/	

double GetTempADT()
{
	WORD rawDataAD = 0;
	
	//Turn SPI on
	PRR0 &= ~(1 << PRSPI);
	Wait_ms(10);
	
	//Slave select goes low to signal start of transmission
	prtSpi &= ~(1 << bnSS);	
	SPDR0 = 0x00;						//start a transmission
	while (!(SPSR0 & (1 << SPIF0)));	//get MSB byte
	rawDataAD = (SPDR0 << 8);				//should contain the data send over, store first byte in upper 8 bits
	SPDR0 = 0x00;						//start transmission of second byte
	while (!(SPSR0 & (1 << SPIF0)));	//get LSB byte byte
	rawDataAD |= SPDR0;					//get second byte, store in low order

	
	//Power off ADC, reset components
	PRR0 |= (1 << PRSPI);				//Turn SPI off	
	prtSpi |= (1 << bnSS)|(1 << bnSck);	//bring CS/SS, Sck high again
	Wait_ms(10);						//Give it time to settle
	
	//Convert number into Celsius, then Fahrenheit
	double dataAD = rawDataAD*ON_CHIP_CONVERTER*1.8 + 32;
	
	return dataAD;
}

/**********************************************************************************************************************************/	
void ResetDebug()
{
		WORD tempNumber = (prtDebug ^ DEBUG_NUMBER);	//gets numbers to clear,
		prtDebug &= tempNumber;
}
/**********************************************************************************************************************************/
void Wait_sec(WORD delay){
	WORD exitTime=((theClock.getSecond()+delay)%60);
	while (theClock.getSecond() != exitTime);
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
