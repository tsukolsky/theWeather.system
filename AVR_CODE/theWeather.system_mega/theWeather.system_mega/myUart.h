/*******************************************************************************\
| myUart.h
| Author: Todd Sukolsky
| Collaborator: Michael Gurr
| Initial Build: 4/26/2013
| Last Revised: 4/27/2013
| Copyright of Todd Sukolsky and Michael Gurr
|================================================================================
| Description: This header file contains declarations for UART transmission/reception
|		from the UART0 ports on the ATmega324PA.
|--------------------------------------------------------------------------------
| Revisions:
|		4/26: Initial build.
|		4/27: Added recognized commands time., T<time>, WEEK. to uart receive that
|			  in turn responds back to the server.
|================================================================================
| *NOTES:
\*******************************************************************************/

extern BOOL flagReceivePi,flagAllStats,flagSendWeek;
extern clock theClock;
void SaveDay();

/*******************************/
/*			Forward Decs.	   */
/*******************************/
void PutUart0Ch(char ch);
void Print0(char string[]);

/**********************************************************************************************************************************/
void PutUart0Ch(char ch)
{
	while (! (UCSR0A & (1 << UDRE0)));
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
/**********************************************************************************************************************************/
void ReceivePi(){
	char recChar, recString[10];
	BYTE strLoc=0,state=0;
	BOOL noDelimiter=fTrue;
	
	while (flagReceivePi){
		switch (state) {
			case 0:{
				//Send ACK
				Print0("ACK.");
				state=1;
				break;
			}//end case 0
			case 1:{
				//Receive the string coming back
				while (noDelimiter && flagReceivePi){
					while (!(UCSR0A & (1 << RXC0)) && flagReceivePi);//wiat for a character to come in on uart
					if (!flagReceivePi){break;state=3;}//had a timeout
					recChar=UDR0;
					recString[strLoc++]=recChar;
					if (recChar=='.'||recChar=='\0'){recString[strLoc]='\0';noDelimiter=fFalse; state=2;}
					else if (strLoc>=10){state=4;noDelimiter=fFalse;}
					else;
				}//end while noDelimiter and receiving 
				break;
			}//end case 1
			case 2:{
				state=3;
				if (!strncmp(recString,"STATS.",6)){flagAllStats=fTrue;}
				else if (!strncmp(recString,"Hi.",3)){Print0("Hello Raspberry Pi!");}
				else if (!strncmp(recString,"save.",5)){Print0("Saving..."); SaveDay();}
				else if (!strncmp(recString,"WEEK.",6)){flagSendWeek=fTrue;}
				else if (!strncmp(recString,"time.",5)){theClock.printTime();}
				else if (recString[0]='T'){state=5;}
				else {state=4;}
				break;
			}//end case 2
			case 3:{
				//Graceful exit
				int i=0;
				for (i=0;i<strLoc;i++){recString[strLoc]=NULL;}	//clear string memory
				flagReceivePi=fFalse;
				break;
			}//end case 3
			case 4:{
				//Bad ACK or ask string
				Print0("Unknown query.");
				state=3;
				break;
			}//end case 4
			case 5:{
				char tempString[3];
				tempString[0]=recString[1];
				tempString[1]=recString[2];
				tempString[2]='\0';
				WORD hour=atoi(tempString);
				tempString[0]=recString[3];
				tempString[1]=recString[4];
				WORD minute=atoi(tempString);
				tempString[0]=recString[5];
				tempString[1]=recString[6];				
				WORD second=atoi(tempString);
				theClock.setTime(hour,minute,second);
				theClock.printTime();
				state=3;
				break;
			}//end case 5
			default:{flagReceivePi=fFalse;break;}			
		}//end switch
	}//end while ReceivePi	
}//end function