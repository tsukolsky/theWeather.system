/*******************************************************************************\
| ATtiny84A.h
| Author: Todd Sukolsky
| ID: U50387016
| Initial Build: 7/31/2012
| Last Revised: 4/24/2013
| Created by Todd Sukolsky
|================================================================================
| Description:	This header file contains symbol declarations describing ports and
|				pins for access to the on-board i/o devices and interface connector	
|				pins on the ATMEGA168A on the SolarBoard	
|--------------------------------------------------------------------------------
| Revisions: 7/31--Created
|			4/24: Added SPI port declarations.
|================================================================================
| *NOTES:
\*******************************************************************************/


#ifndef THE_WEATHER_SYSTEM	
#define THE_WEATHER_SYSTEM

/* ---------------------------------------------------------------
				Interface Connector Declarations
   --------------------------------------------------------------- */
//Symbol definitions for SPI
#define prtSPI	PORTA
#define ddrSPI	DDRA
#define pinSPI	PINA
#define bnSCK	4
#define bnMISO	5
#define bnMOSI	6

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