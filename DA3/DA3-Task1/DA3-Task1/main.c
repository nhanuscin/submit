/*
 * DA3-Task1.c
 *
 * Created: 3/27/2018 10:28:06 AM
 * Author : Nathan Hanuscin
 */ 

#define F_CPU 8000000UL
#define UBRR_9600 51 // for 8Mhz with .2% error

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdint.h>

// Function Declarations
void read_adc(void);
void adc_init(void);
void USART_init( unsigned int ubrr );
void USART_tx_string( char *data );
volatile unsigned int adc_temp;
char outs[20];


int main(void)
{
	adc_init(); 						//Initialize the ADC (Analog / Digital Converter)
	USART_init(UBRR_9600); 				//Initialize the USART (RS232 interface)
	USART_tx_string("Connected!\r\n"); 	//Display connected
	_delay_ms(125); 					//wait a bit
	TIMSK1 = (1<<TOIE1);				//enable timer overflow interrupt
	TCNT1 = 57723;						//set counter value
	TCCR1A = 0;							//normal mode
	TCCR1B = (1<<CS12) | (1<<CS10);		//pre-scaler of 1024
	sei();								//enable interrupts
	while(1)
	{
		// wait for interrupt
	}
}

ISR (TIMER1_OVF_vect)
{
	TCCR1B = 0;											//turn timer off
	read_adc();											//read the adc values
	snprintf(outs,sizeof(outs),"%3d\r\n", adc_temp);	//print it
	USART_tx_string(outs);
	_delay_ms(125); 									// wait a bit
	TCNT1 = 57723;										//reset timer value
	TCCR1B = (1<<CS12) | (1<<CS10);						//restart the clock
	
}

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
	(1<<ADPS2)| 			//ADC Prescaler of 64
	(1<<ADPS1)|
	(0<<ADPS0);

}

/* READ ADC PINS*/
void read_adc(void)
{
	unsigned char i = 4;					//set for 4 ADC reads
	adc_temp = 0;							//initalize temp to 0
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
