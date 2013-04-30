/*******************************************************************************\
| thermostat.h
| Author: Todd Sukolsky
| Collaborator: Michael Gurr
| Initial Build: 4/26/13
| Last Revised: 4/27/13
| Copyright of Todd Sukolsky and Michael Gurr
|================================================================================
| Description: Class "thermostat".
|--------------------------------------------------------------------------------
| Revisions:
|		4/26: Initial build.
|		4/27: Added addTheDay, printTheWeek, resetDay and resetWeek for functionality
|			  beyond current readings. Now able to save a weeks worth of data.
|		4.28: Fixed an issue with the new day->newWeek timing. Now on a new week it takes a reading
|			  just to make sure there is something valid in the data variables.
|================================================================================
| *NOTES:
\*******************************************************************************/

void SaveDay();		//function defined in EEPROM.
void Print0(char string[]);
double GetTempTherm();
double GetHumidity();
	
class thermostat{
	public:
		thermostat();
		void reset();
		void resetDay();
		void resetWeek();
		void addDataPoint(double temp, double humidity);
		double getHigh();
		double getLow();
		double getAveT();
		double getAveH();
		void saveData();
		void PrintWeek();
		void addTheDay();
		void takeReadings();
		
	private:
		double high,low,averageT, averageH;			//high temp, low temp, averagetemp, average humidity: all for the current day.
		double weekHigh,weekLow,weekAverageT,weekAverageH;
		WORD numDayReadings;
		BYTE howManyDays;
};

thermostat::thermostat(){
	reset();
}

void thermostat::reset(){
	resetDay();
	resetWeek();
}

void thermostat::resetDay(){
	high=0.0;
	low=1000.0;
	averageT=0.0;
	averageH=0.0;
	numDayReadings=0;	
}

void thermostat::resetWeek(){
	howManyDays=0;
	weekHigh=0;
	weekLow=0;
	weekAverageH=0;
	weekAverageT=0;	
}

void thermostat::takeReadings(){
	addDataPoint(GetTempTherm(),GetHumidity());	
}

void thermostat::addTheDay(){
	//Add this day to the average of things
	takeReadings();
	howManyDays++;
	weekHigh=(weekHigh*(howManyDays-1) + high)/howManyDays;
	weekLow=(weekLow*(howManyDays-1)+low)/howManyDays;
	weekAverageH=(weekAverageH*(howManyDays-1)+averageH)/howManyDays;
	weekAverageT=(weekAverageT*(howManyDays-1)+averageT)/howManyDays;
	resetDay();
}

void thermostat::PrintWeek(){
	//Set the time, take a reading
	if (howManyDays==0){
		weekAverageT=averageT;
		weekAverageH=averageH;
		weekHigh=high;
		weekLow=low;
	}
	char weekHighStr[6],weekLowStr[6],weekAveHStr[6],weekAveTStr[6];
	dtostrf(weekHigh,0,2,weekHighStr);
	dtostrf(weekLow,0,2,weekLowStr);
	dtostrf(weekAverageH,0,2,weekAveHStr);
	dtostrf(weekAverageT,0,2,weekAveTStr);
	Print0("WH");
	Print0(weekHighStr);
	Print0("/WL");
	Print0(weekLowStr);
	Print0("/WT");
	Print0(weekAveTStr);
	Print0("/Wh");
	Print0(weekAveHStr);
	Print0("XXX");
	resetWeek();
}

void thermostat::addDataPoint(double temp, double humidity){
	numDayReadings++;
	averageT=(averageT*(numDayReadings-1)+temp)/numDayReadings;
	averageH=(averageH*(numDayReadings-1)+humidity)/numDayReadings;
	
	if (temp>high){
		high=temp;
	}
	//Seperate if statements just in case it is a high and a low.
	if (temp<low){
		low=temp;
	}
}

void thermostat::saveData(){
	SaveDay();			//Calls save day in 
	reset();			//reset thermostat.
}
double thermostat::getHigh(){
	return high;
}

double thermostat::getLow(){
	return low;
}

double thermostat::getAveT(){
	return averageT;
}

double thermostat::getAveH(){
	return averageH;
}