/*******************************************************************************\
| clock.h
| Author: Todd Sukolsky
| Collaborator:
| Initial Build: 4/26/13
| Last Revised: 4/26/13
| Copyright of Todd Sukolsky and Michael Gurr
|================================================================================
| Description: Class "clock".
|--------------------------------------------------------------------------------
| Revisions:
|		4/26: Initial build. Can't increment normal way, has to be done long way
|			  with temp variables. Pain in the butt. On new day the addDay function
|			  will call thermostat function to save the days information.
|================================================================================
| *NOTES:
\*******************************************************************************/
#include <string.h>
#include <math.h>
#include <stdlib.h>

using namespace std;

BYTE daysInMonths[12]={31,28,31,30,31,30,31,31,30,31,30,31};
extern thermostat theThermostat;
void Print0(char string[]);

class clock{
	public:
		clock();
		void addSecond(WORD seconds);
		BYTE getSecond();
		BYTE getMinute();
		BYTE getHour();
		BYTE getDay();
		BYTE getMonth();
		WORD getYear();
		
	private:
		volatile BYTE second,minute,hour;
		volatile BYTE day,month;
		volatile WORD year;
		void addMinute(WORD minutes);
		void addHour(WORD hours);
		void addDay(WORD days);
		void addMonth(WORD months);
		void addYear(WORD years);
};

clock::clock(){
	second=0;
	minute=0;
	hour=0;
	day=0;
	month=0;
	year=2013;
}

void clock::addSecond(WORD seconds){
	volatile int tempSecond=second+seconds;
	second=tempSecond%60;
	if (tempSecond/60>=1){
		addMinute(tempSecond/60);
	}
}

void clock::addMinute(WORD minutes){
	volatile int tempMinutes=minute+minutes;
	minute=tempMinutes%60;
	if (tempMinutes/60>=1){
		addHour(tempMinutes/60);
	}
}

void clock::addHour(WORD hours){
	volatile int tempHours=hour+hours;
	hour=tempHours%24;
	if (tempHours>=1){
		addDay(tempHours/24);
	}
}

void clock::addDay(WORD days){
	//new day, need to save the average, high and low into eeprom
	theThermostat.saveData();
	
	//Get the new day.
	volatile int tempDays=day+days;
	BYTE currentMonth=month;
	if (tempDays/daysInMonths[currentMonth]>=1){
		//See how many months we need to go forward.
		while (tempDays >= daysInMonths[currentMonth]){
			tempDays-=daysInMonths[currentMonth];
			currentMonth++;
		}
		if (tempDays>28 && currentMonth==2){
			tempDays-=daysInMonths[currentMonth];
			currentMonth++;
		}
	}
	days=tempDays;
	month=currentMonth;
}

void clock::addMonth(WORD months){
	volatile int tempMonths=month+months;
	month=tempMonths%12;
	if (tempMonths/12>=1){
		addYear(tempMonths/12);
	}
}

void clock::addYear(WORD years){
	volatile int tempYears=year+years;
	year=tempYears;
}

/**********************************/
/*			Get Functions		  */
/**********************************/
BYTE clock::getSecond(){
	return second;
}
BYTE clock::getMinute(){
	return minute;
}
BYTE clock::getHour(){
	return hour;
}
BYTE clock::getDay(){
	return (day+1);
}
BYTE clock::getMonth(){
	return (month+1);
}
WORD clock::getYear(){
	return year;
}