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

//Symbol definintions for GPIO led
#define prtLED PORTB
#define ddrLED DDRB
#define pinLED PINB
#define bnLED  0

//Symbol definitions for waiting counter
#define BIT0 0
#define BIT1 1
#define BIT2 2
#define BIT3 3
#define BIT4 4
#define BIT5 5
#define BIT6 6
#define BIT7 7


#endif /* THE_WEATHER_SYSTEM */

/***************************************************************/