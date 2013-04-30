/*******************************************************************************\
| myTWI.h
| Collaborator: Todd Sukolsky
| Collaborator: Michael Gurr
| Initial Build: 4/27/2012
| Last Revised: 4/27/2012
|================================================================================
| Description: TWI protocols for theWeather.system MEGA
|--------------------------------------------------------------------------------
| Revisions:
|================================================================================
| *NOTES:
|	(1)Mega is master(transmitter), tiny is slave(receiver)
|	(2)Code based on http://www.engineersgarage.com/embedded/avr-microcontroller-projects/atmega32-twi-two-wire-interface
\*******************************************************************************/
//WOrkflow:
/*Mega(master-transmitter)->tiny(slave-receiver), slave deals with data; tiny(slave-transmitter)->mega(master->receiver)*/

#define tiny (0x0A<<1)

//Forward Declarations
void Wait_sec(WORD delay);
void TWI_init_master();
void TWI_start();
void TWI_stop();
void TWI_read_address(BYTE data);
BYTE TWI_read_data();
void TWI_write_data(BYTE data);




BYTE getTempTiny(){
	//Send the start condition
	TWI_start();
	//Send the 7-bit slave address, data direction bit (0)
	TWI_read_address(tiny);			//slave address should be 1010
	//Send the data.
	TWI_write_data(0x04);			//4 represents=>SendThermTemp
	//Send Stop Condition
	TWI_stop();
	
	//Wait two seconds before getting data off the bus
	Wait_sec(2);
	
	//Send Start Condition
	TWI_start();
	//Send slave address with read instruction
	TWI_read_address(tiny+1);
	//Read the data from the line
	BYTE output=TWI_read_data();
	//Send Stop Condition
	TWI_stop();
	
	return output;
}


//Initialize Master
void TWI_init_master(void){
	TWBR=0x01;		//Bit rate
	TWSR=(0<<TWPS1)|(0<<TWPS0); 
}

void TWI_start(void){
	// Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
	TWCR= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT))); // Wait till start condition is transmitted
	while((TWSR & 0xF8)!= 0x08); // Check for the acknowledgement
}

void TWI_read_address(BYTE data)
{
	TWDR=data;    // Address and read instruction (read=1, write=0) in low bit.
	TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte received
	while((TWSR & 0xF8)!= 0x40);  // Check for the acknoledgement
}

void TWI_write_data(BYTE data)
{
	TWDR=data;    // put data in TWDR, direction bit (write=0, read=1).
	TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
	while((TWSR & 0xF8) != 0x28); // Check for the acknoledgement
}

BYTE TWI_read_data(void)
{
	TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
	while((TWSR & 0xF8) != 0x58); // Check for the acknoledgement
	return TWDR;
}
void TWI_stop(void)
{
	// Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
	TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while(!(TWCR & (1<<TWSTO)));  // Wait till stop condition is transmitted
}