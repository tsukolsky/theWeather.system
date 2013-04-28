/*******************************************************************************\
| tinyTWI.h
| Collaborator: Todd Sukolsky
| Collaborator: Michael Gurr
| Initial Build: 4/27/2012
| Last Revised: 4/27/2012
|================================================================================
| Description:	TWI protocols for theWeather.system ATtiny
|--------------------------------------------------------------------------------
| Revisions: 
|
|================================================================================
| *NOTES:
|	(1)Mega is master(transmitter), tiny is slave(receiver)
|	(2)Code based on http://www.engineersgarage.com/embedded/avr-microcontroller-projects/atmega32-twi-two-wire-interface
\*******************************************************************************/

//#include "usi_i2c_slave.h"

void SendToMega(BYTE data){
	//Wait for matching slave address adn direction bit
	TWI_match_read_slave();
	//Read the address
	BYTE received=TWI_read_slave();
	//Wait for a matching write statement
	TWI_match_write_slave();
	//Send the data
	TWI_write_slave(data);
}

void TWI_init_slave(void) // Function to initilaize slave
{
	TWAR=0x0A;    // Fill slave address to TWAR
}

void TWI_match_read_slave(void) //Function to match the slave address and slave dirction bit(read)
{
	while((TWSR & 0xF8)!= 0x60)  // Loop till correct acknoledgement have been received
	{
		// Get acknowlegement, Enable TWI, Clear TWI interrupt flag
		TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWINT);
		while (!(TWCR & (1<<TWINT)));  // Wait for TWINT flag
	}
}

BYTE TWI_read_slave(void)
{
	// Clear TWI interrupt flag,Get acknowlegement, Enable TWI
	TWCR= (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));    // Wait for TWINT flag
	while((TWSR & 0xF8)!=0x80);        // Wait for acknowledgement
	recv_data=TWDR;                    // Get value from TWDR
	PORTB=recv_data;                // send the receive value on PORTB
}

void TWI_match_write_slave(void) //Function to match the slave address and slave dirction bit(write)
{
	while((TWSR & 0xF8)!= 0xA8)    // Loop till correct acknoledgement have been received
	{
		// Get acknowlegement, Enable TWI, Clear TWI interrupt flag
		TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWINT);
		while (!(TWCR & (1<<TWINT)));  // Wait for TWINT flag
	}
}

void TWI_write_slave(BYTE data) // Function to write data
{
	TWDR= data;              // Fill TWDR register whith the data to be sent
	TWCR= (1<<TWEN)|(1<<TWINT);   // Enable TWI, Clear TWI interrupt flag
	while((TWSR & 0xF8) != 0xC0); // Wait for the acknowledgement
}