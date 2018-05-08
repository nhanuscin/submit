/*
 * DA_Final_Project.c
 *
 * Created: 5/5/2018 10:43:46 AM
 * Author : Nathan Hanuscin
 */ 


#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>

#define UBRR_9600 51 // for 8Mhz with .2% error

void USART_init( unsigned int ubrr )
{
	UBRR0H = (unsigned char)(ubrr>>8);			//set baud rate
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << TXEN0) | (1 <<RXEN0); 		// Enable receiver, transmitter
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); 	//asynchronous 8-bit data 1 stop bit
}

void USART_tx_string( char *data )
{
	while ((*data != '\0'))
	{
		while (!(UCSR0A & (1 <<UDRE0)));	//wait for the transmit buffer to empty
		UDR0 = *data;						//put the data into the empty buffer, which sends the data
		_delay_ms(50); 					// wait a bit
		data++;
	}
}

void USART_Transmit( unsigned char data )
{
	
	while ( !( UCSR0A & (1<<UDRE0)) )
	{
		/* Wait for empty transmit buffer */
	}
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

unsigned char USART_Receive( void )
{

	while ( !(UCSR0A & (1<<RXC0)) )
	{
		/* Wait for data to be received */
	}

	/* Get and return received data from buffer */
	return UDR0;
}

unsigned int get_number(int ones, int tens)
{
	unsigned char data;
	data = USART_Receive();					//get the tens digits
	USART_Transmit(data);					//transmit it back
	tens = data - 0x30;						//convert to a digit	
	tens = tens *10;						//convert to tens place
	data = USART_Receive();					//get ones digit
	USART_Transmit(data);					//transmit it back
	ones = data - 0x30;						//convert to ones place
	return tens + ones;						//return position
}
//this function takes in the number to iterations to move the object forward to the desired position
void go_foward(int iter)
{
	int i;
		for (i=0;i<iter;i++)
		{
			PORTB = 0x09;
			_delay_ms(75);
			PORTB = 0x0C;
			_delay_ms(75);
			PORTB = 0x06;
			_delay_ms(75);
			PORTB = 0x03;
			_delay_ms(75);
		}
	
}
//this function takes in the number of iterations to move the object back to the starting position
void go_back(int iter)
{
	int i;
	for (i=0;i<iter;i++)
	{
		PORTB = 0x03;
		_delay_ms(75);
		PORTB = 0x06;
		_delay_ms(75);
		PORTB = 0x0C;
		_delay_ms(75);
		PORTB = 0x09;
		_delay_ms(75);
		
	}
	
}

int main(void)
{	int ones_place, tens_place;
	unsigned int pos1, pos2, pos3;
	int iterations;
	unsigned char digit;
	USART_init(UBRR_9600); 								//Initialize the USART (RS232 interface)
	
	USART_tx_string("Please enter 3 positions in centimeters with one space between each position\r\n"); 		
	USART_tx_string("For single digit positions please put a zero in the tens place... \r\n");
	USART_tx_string("I.E. for 9 cm put 09\r\n");
	USART_tx_string("Example input: 01 10 25\r\n");
	USART_tx_string("Max value is 35cm\r\n");
	
	DDRB = 0xFF;								//set port B as an output
	ones_place = tens_place = 0;
	
	pos1 = get_number(ones_place, tens_place);
	
	digit = USART_Receive();					//get space
	USART_Transmit(digit);
	
	ones_place = tens_place = 0;
	pos2 = get_number(ones_place, tens_place);
	
	digit = USART_Receive();					//get space
	USART_Transmit(digit);
	
	ones_place = tens_place = 0;
	pos3 = get_number(ones_place, tens_place);
	
	if (pos3 > 35)
		pos3 = 35;							//if position 3 is out of range change it to max value
	
	iterations = pos1 * 80;					//80 iterations = 1cm, so multiply position value by 80
	go_foward(iterations);					//move object to position 1
	_delay_ms(5000);						//wait a bit
	
	iterations = (pos2 - pos1) * 80;		//get difference first two positions then multiply by 80
	go_foward(iterations);					//move object to position 2
	_delay_ms(5000);						//wait a bit
	
	iterations = (pos3 - pos2) * 80;		//get difference second and third position then multiply by 80
	go_foward(iterations);					//move object to final position
	_delay_ms(5000);						//wait a bit
	
	iterations = pos3 * 80;					//get total iterations made 
	go_back(iterations);					//to return to starting position

	return 0;
}

