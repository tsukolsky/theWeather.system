/*******************************************************************************\
| eepromGurrDaddy.h
| Author: Michael Gurr
| Collaborator: Todd Sukolsky
| Initial Build: 4/26/13
| Last Revised: 4/26/13
| Copyright of Todd Sukolsky and Michael Gurr
|================================================================================
| Description: Document contains methods for EEPROM movements of temperature readings
|		and such data. 
|--------------------------------------------------------------------------------
| Revisions:
|	4/26: Initial Build
|================================================================================
| *NOTES: (1) Delete works down to 0, can't delete from 2->30, doesn't work that way.
|		  (2) 1024bytes/20bytes~51=>40 days max is storage of this.
\*******************************************************************************/

using namespace std;

extern clock theClock;
extern thermostat theThermostat;

//Parameters
#define INITIAL_OFFSET		4
#define BLOCK_SIZE			20
#define MAX_DAYS			40
//Byte Offsets
#define DAY					0
#define MONTH				2
#define LOW					4
#define HIGH				8
#define AVGT				12
#define AVGH				16

//Total number of days stored
BYTE EEMEM eeTotalDays=0;
BYTE EEMEM eeOldestDay=0;

void MoveDown(BYTE whichDay){
	//Get what day we are moving and all of it's data into temp variables
	WORD offset=INITIAL_OFFSET+(whichDay)*BLOCK_SIZE;
	WORD dayT,monthT;
	dayT=eeprom_read_word((WORD *)(offset+DAY));
	monthT=eeprom_read_word((WORD *)(offset+MONTH));
	float lowT=eeprom_read_float((float*)(offset+LOW));
	float highT=eeprom_read_float((float*)(offset+HIGH));
	float avgT=eeprom_read_float((float*)(offset+AVGT));
	float avgH=eeprom_read_float((float *)(offset+AVGH));
	
	//Put all that data into the spot below this one. Offset is one block down
	offset-=BLOCK_SIZE;
	eeprom_update_word((WORD *)(offset+DAY),dayT);
	eeprom_update_word((WORD *)(offset+MONTH),monthT);
	eeprom_update_float((float*)(offset+LOW),lowT);
	eeprom_update_float((float*)(offset+HIGH),highT);
	eeprom_update_float((float*)(offset+AVGT),avgT);
	eeprom_update_float((float*)(offset+AVGH),avgH);
}

void SaveDay(){
	//This is called when a new day occurs. This is done BEFORE the Day is incremented, so we can use the current data.
	static BOOL allFull=fFalse;
	BYTE daysStored,oldestDay;
	WORD offset;
	
	//If we are all full, don't waste a read on the TotalDays, go right to OldestDay protocol
	if (!allFull){daysStored=eeprom_read_byte(&eeTotalDays);}
		
	//Get offset for where we are goign to store data.
	if (daysStored >= MAX_DAYS || allFull){
		if (!allFull){allFull=fTrue;}
		oldestDay=eeprom_read_byte(&eeOldestDay);
		//Offset is wherever the oldestDay is, which is the block size*absolute + initial.
		offset = INITIAL_OFFSET+(oldestDay)*BLOCK_SIZE;
		//increment oldest day to next location.
		oldestDay++;
		eeprom_update_byte(&eeOldestDay,oldestDay);
	} else {
		//Offset is how many days are stored. If 0 are stored, should be lowest location. if one is stored, should be the next location (block size up).
		offset = INITIAL_OFFSET+(daysStored)*BLOCK_SIZE;
		daysStored++;
		//Update how many days are stored.
		eeprom_update_byte(&eeTotalDays,daysStored);	
	}

	//Write the data to EEPROM
	eeprom_update_word((WORD *)(offset+DAY),(float)theClock.getDay());
	eeprom_update_word((WORD *)(offset+MONTH),(float)theClock.getMonth());
	eeprom_update_float((float *)(offset+LOW),(float)theThermostat.getLow());
	eeprom_update_float((float *)(offset+HIGH),(float)theThermostat.getHigh());
	eeprom_update_float((float *)(offset+AVGT),(float)theThermostat.getAveT());
	eeprom_update_float((float *)(offset+AVGH),(float)theThermostat.getAveH());
}

void DeleteDay(BYTE whichDay){
	BYTE daysStored=eeprom_read_byte(&eeTotalDays);
	for(int i=whichDay+1; i<=daysStored; i++){
		MoveDown(i);
	}
	
	daysStored--;
	eeprom_update_byte(&eeTotalDays,daysStored);
}