/*******************************************************************************\
| atmega324PA.h
| Author: Todd Sukolsky
| ID: U50387016
| Initial Build: 1/3/2013
| Last Revised: 1/3/13
| Copyright of Todd Sukolsky
|================================================================================
| Description:
|--------------------------------------------------------------------------------
| Revisions:
|================================================================================
| *NOTES:
\*******************************************************************************/


#ifndef THE_WEATHER_SYSTEM	
#define THE_WEATHER_SYSTEM

/* ---------------------------------------------------------------
				Interface Connector Declarations
   --------------------------------------------------------------- */

//Symbol definintions for GPIO leds

#define prtDebug	PORTC
#define ddrDebug	DDRC
#define pinDebug	PINC
#define bnD0		3
#define bnD1		4
#define bnD2		5
#define bnD3		6
#define bnD4		7

// Symbol definitions for the I2C signals
#define prtSDA		PORTC
#define prtSCL		PORTC
#define ddrSDA		DDRC
#define ddrSCL		DDRC
#define pinSDA		PINC
#define pinSCL		PINC
#define bnSDA		1
#define bnSCL		0

// Symbol definitions for access to the UART
#define prtRXD		PORTD
#define prtTXD		PORTD
#define ddrRXD		DDRD
#define ddrTXD		DDRD
#define pinRXD		PIND
#define pinTXD		PIND
#define bnRXD		0
#define bnTXD		1

// Symbol definition for the master pin change interrupt into the microcontroller, on PB2 (INT2)
#define prtINT2		PORTB
#define ddrINT2		DDRB
#define pinINT2		PINB
#define bnINT2		2

// Symbol definitions for access to the SPI (0) connector
#define	prtSpi		PORTB
#define	pinSpi		PINB
#define	ddrSpi		DDRB
#define	bnSS		4
#define	bnMosi	5
#define	bnMiso	6
#define	bnSck	7

//Symbol definitons for acces to SPI (1) connector for TI_ temp sensor, USART mode
#define prtSpi1		PORTD
#define ddrSpi1		DDRD
#define pinSpi1		PIND
#define bnSpiCs_1	5
#define bnSpiSck_1	4	 
#define bnSpiMiso_1	2	

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

//Symbol definitions for RTC Signals
#define prtRTC PORTC
#define ddrRTC DDRC
#define pinRTC PINC
#define bnMFP  2		//signal going into RTC

//Symbol definitions for thermistor and humidity sensors
#define prtSensor	PORTA
#define ddrSensor	DDRA
#define pinSensor	PINA
#define bnHumidity	1	//these won't actually be used, the ADC channels are selected on APP init, good to know though.
#define bnTherm		2

#endif /* THE_WEATHER_SYSTEM */

/***************************************************************/