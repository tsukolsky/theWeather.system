/*******************************************************************************\
| myUart.h
| Author: Todd Sukolsky
| ID: U50387016
| Initial Build: 4/26/2013
| Last Revised: 4/26/2013
| Copyright of Todd Sukolsky
|================================================================================
| Description: This header file contains declarations for UART transmission/reception
|		from the UART0 ports on the ATmega324PA.
|--------------------------------------------------------------------------------
| Revisions:
|		4/26: Initial build.
|================================================================================
| *NOTES:
\*******************************************************************************/

extern BOOL flagReceivePi,flagAllStats,flagTemp,flagHumidity;

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
					if (!flagReceivePi){break;}//had a timeout
					recChar=UDR0;
					recString[strLoc++]=recChar;
					if (recChar=='.'||recChar=='\0'){recString[strLoc]='\0';noDelimiter=fFalse; state=2;}
					else if (strLoc>=10){state=4;noDelimiter=fFalse;}
					else;
				}//end while noDelimiter and receiving 
				break;
			}//end case 1
			case 2:{
				if (!strncmp(recString,"STATS.",6)){flagAllStats=fTrue;state=3;}
				else if (!strncmp(recString,"TEMP.",5)){flagTemp=fTrue;state=3;}
				else if (!strncmp(recString,"HUMIDITY.",9)){flagHumidity=fTrue;state=3;}
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
			default:{flagReceivePi=fFalse;break;}			
		}//end switch
	}//end while ReceivePi	
}//end function