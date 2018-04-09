/*
 * DA_Midterm.c
 *
 * Created: 3/27/2018 5:37:20 PM
 * Author : Nathan Hanuscin
 */ 

#define F_CPU 8000000UL
#define UBRR_115200 3 // for 8Mhz with 8.5% error

#define Channel_ID "461798"


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdint.h>

volatile unsigned int adc_temp;
char outs[20];


void adc_init(void)
{
	/** Setup and enable ADC **/
	ADMUX = 0;				//select ADC0 Pin as input
	ADMUX = (0<<REFS1)| 	//Reference Selection Bits
	(1<<REFS0)| 			//AVcc - external cap at AREF
	(1<<ADLAR); 			//ADC left Adjust Result
	
	ADCSRA = (1<<ADEN)| 	//ADC ENable
	(1<<ADSC)| 				//ADC Start Conversion
	(1<<ADATE)| 			//ADC Auto Trigger Enable
	(0<<ADIF)| 				//ADC Interrupt Flag
	(0<<ADIE)| 				//ADC Interrupt Enable
	(1<<ADPS2)| 			//ADC Pre-scaler of 64
	(1<<ADPS1)|
	(0<<ADPS0);

}

/* READ ADC PINS*/
void read_adc(void)
{
	unsigned char i = 4;					//set for 4 ADC reads
	adc_temp = 0;							//initialize temp to 0
	while (i--)
	{
		ADCSRA |= (1<<ADSC);				//start the conversion
		while((ADCSRA & (1<<ADIF)) == 0);	//wait for conversion to finish
		adc_temp += ADCH*2;					//get temp value
		_delay_ms(50);						//wait a bit
	}
	adc_temp = adc_temp / 4; 				// Average a few samples

}

/* INIT USART (RS-232) */
void USART_init( unsigned int ubrr )
{
	UBRR0H = (unsigned char)(ubrr>>8);			//set baud rate
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << TXEN0) | (1 <<RXEN0); 		// Enable receiver, transmitter
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); 	//asynchronous 8-bit data 1 stop bit
}

/* SEND A STRING TO THE RS-232*/
void USART_tx_string( char *data )
{
	while ((*data != '\0'))
	{
		while (!(UCSR0A & (1 <<UDRE0)));	//wait for the transmit buffer to empty
		UDR0 = *data;						//put the data into the empty buffer, which sends the data
		_delay_ms(125); 					// wait a bit
		data++;
	}
}

void usart_send( unsigned char ascii)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = ascii;
}
unsigned char usart_receive(void)
{
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}
void send_AT( unsigned char message[])
{
	unsigned char i=0;
	while(message[i] != '\0')
	{
		usart_send(message[i]); // This sends data to esp
		i++;
	}
}


int main(void)
{
	unsigned char AT[] = "AT\r\n";
	unsigned char CWMODE[] = "AT+CWMODE=3\r\n";
	unsigned char CWJAP[] = "AT+CWJAP= \"SSID\",\"PASSWORD\"\r\n";
	
	unsigned char CIPMUX[] = "AT+CIPMUX=0\r\n";
	//unsigned char CIPMUX[] = "AT+CIPMUX=1\r\n";
	
	unsigned char CIPSTART[] = "AT+CIPSTART=0,\"TCP\",\"api.thingspeak.com\",80\r\n";
	
	unsigned char CIPSEND[] = "AT+CIPSEND=45\r\n";
	//unsigned char CIPSEND[] = "AT+CIPSEND=0,110\r\n";
	
	//unsigned char GET_DATA[] = "GET https://api.thingspeak.com/apps/thinghttp/send_request?api_key=D6UQ4AHE1FSE4808\r\n";
	
	unsigned char SEND_DATA[] = "GET /update?key=PG5YKHOM60E8XQRI&field1=";
	//unsigned char SEND_DATA[] = "GET https://api.thingspeak.com/update?api_key=PG5YKHOM60E8XQRI=50\r\n";
	
	
	adc_init(); 						//Initialize the ADC (Analog / Digital Converter)
	USART_init(UBRR_115200); 			//Initialize the USART (RS232 interface)
	_delay_ms(125); 					//wait a bit
	
	_delay_ms(200);
	send_AT(AT);					//send AT commands
	_delay_ms(2000);
	send_AT(CWMODE);				//initialize wifi mode
	_delay_ms(2000);
	send_AT(CIPMUX);				//single connection
	_delay_ms(2000);
	send_AT(CWJAP);					//connect to wifi network
	_delay_ms(2000);
	send_AT(CIPSTART);				//connect to thingspeak server
	_delay_ms(2000);

	while(1)
	{
		read_adc();
		snprintf(outs,sizeof(outs),"%3d\r\n", adc_temp);		// convert to string
		send_AT(CIPSEND);										// send number of bytes
		_delay_ms(2000);
		send_AT(SEND_DATA);										// send data command
		_delay_ms(2000);
		USART_tx_string(outs);									// send converted value
		_delay_ms(15000);
	
	}
}



