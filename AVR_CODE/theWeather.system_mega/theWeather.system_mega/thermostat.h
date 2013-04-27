/*******************************************************************************\
| thermostat.h
| Author: Todd Sukolsky
| Collaborator: Michael Gurr
| Initial Build: 4/26/13
| Last Revised: 4/26/13
| Copyright of Todd Sukolsky and Michael Gurr
|================================================================================
| Description: Class "thermostat".
|--------------------------------------------------------------------------------
| Revisions:
|		4/26: Initial build.
|================================================================================
| *NOTES:
\*******************************************************************************/

void SaveDay();		//function defined in EEPROM.

class thermostat{
	public:
		thermostat();
		void reset();
		void addDataPoint(double temp, double humidity);
		double getHigh();
		double getLow();
		double getAveT();
		double getAveH();
		void saveData();
		
	private:
		double high,low,averageT, averageH;			//high temp, low temp, averagetemp, average humidity: all for the current day.
		WORD numReadings;
};

thermostat::thermostat(){
	high=0.0;
	low=0.0;
	averageT=0.0;
	averageH=0.0;
	numReadings=0;
}

void thermostat::reset(){
	high=0.0;
	low=0.0;
	averageT=0.0;
	averageH=0.0;
	numReadings=0;
}

void thermostat::addDataPoint(double temp, double humidity){
	numReadings++;
	averageT=(averageT*(numReadings-1)+temp)/numReadings;
	averageH=(averageH*(numReadings-1)+humidity)/numReadings;
	
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