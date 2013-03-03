/*******************************************************************************\
| atmega168A.h
| Author: Mark Taylor
| ID: U50387016
| Initial Build: 7/31/2012
| Last Revised: 7/31/2012
| Created by Mark Taylor: Permitted for use by Todd Sukolsky
|================================================================================
| Description:	This header file contains symbol declarations describing ports and
|				pins for access to the on-board i/o devices and interface connector	
|				pins on the ATMEGA168A on the SolarBoard	
|--------------------------------------------------------------------------------
| Revisions: 7/31--Created
|================================================================================
| *NOTES:
\*******************************************************************************/


#ifndef THE_WEATHER_SYSTEM	
#define THE_WEATHER_SYSTEM

/* ---------------------------------------------------------------
				Interface Connector Declarations
   --------------------------------------------------------------- */

//Symbol definintions for GPIO leds

#define prtLED		PORTC
#define ddrLED		DDRC
#define pinLED		PINC
#define prtLED0		PORTB
#define ddrLED0		DDRB
#define pinLED0		PINB
#define bnLED0		0
#define bnLED4		3
#define bnLED3		2
#define bnLED2		1
#define bnLED1		0

// Symbol definitions for the I2C signals
#define prtSDA		PORTC
#define prtSCL		PORTC
#define ddrSDA		DDRC
#define ddrSCL		DDRC
#define pinSDA		PINC
#define pinSCL		PINC
#define bnSDA		4
#define bnSCL		5

// Symbol definitions for access to the UART
#define prtRXD		PORTD
#define prtTXD		PORTD
#define ddrRXD		DDRD
#define ddrTXD		DDRD
#define pinRXD		PIND
#define pinTXD		PIND
#define bnRXD		0
#define bnTXD		1

// Symbol definition for the master pin change interrupt into the microcontroller
#define prtINT0		PORTD
#define ddrINT0		DDRD
#define pinINT0		PIND
#define bnINT0		2

// Symbol definitions for access to the SPI connector
#define	prtSpi	 PORTB
#define	pinSpi	 PINB
#define	ddrSpi	 DDRB

#define	bnSpiSS		 2
#define	bnSpiMosi	 3
#define	bnSpiMiso	 4
#define	bnSpiSck	 5

//Symbol definitions for waiting counter
#define BIT0 0
#define BIT1 1
#define BIT2 2
#define BIT3 3
#define BIT4 4
#define BIT5 5
#define BIT6 6
#define BIT7 7

//Symbol definitions for ADC
#define REFS1 7
#define REFS0 6
#define ADLAR 5
#define MUX3  3
#define MUX2  2
#define MUX1  1
#define MUX0  0

#define ADEN  7
#define ADSC  6
#define ADATE 5
#define ADIF  4
#define ADIE  3
#define ADPS2 2
#define ADPS1 1
#define ADPS0 0

#define ACME  6
#define ADTS2 2
#define ADTS1 1
#define ADTS0 0

#endif /* THE_WEATHER_SYSTEM */

/***************************************************************/