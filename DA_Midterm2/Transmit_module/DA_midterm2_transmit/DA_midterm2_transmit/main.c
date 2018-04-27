/*
 * DA_midterm2_transmit.c
 *
 * Created: 4/23/2018 3:24:57 PM
 * Author : Nathan Hanuscin and Brian Lopez
 */ 
//transmit module setting atmega as master for spi

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "nrf24l01.h"
#define UBRR_9600 51 // for 8Mhz with .2% error
#define F_CPU 8000000UL
#include <util/delay.h>


void spi_init(void);
void setup_timer(void);
nRF24L01 *setup_rf(void);
void adc_init(void);
void read_adc(void);
void USART_init( unsigned int ubrr );
void USART_tx_string( char *data );
volatile bool rf_interrupt = false;
volatile bool send_message = false;
volatile unsigned int adc_temp;
char outs[20];



int main(void)
{

	uint8_t to_address[5] = { 0x01, 0x01, 0x01, 0x01, 0x01 };
	spi_init();										//Initialize SPI 
	USART_init(UBRR_9600); 							//Initialize the USART (RS232 interface)
	USART_tx_string("Connected!\r\n"); 	      		//Display connected
	_delay_ms(125); 								//wait a bit
	sei();				
	nRF24L01 *rf = setup_rf();					
	adc_init();										//Initialize ADC conversion
	setup_timer();									//Set up timer



	while (true)
	{
		if (rf_interrupt)
		{
			rf_interrupt = false;
			int success = nRF24L01_transmit_success(rf);
			if (success != 0)
			nRF24L01_flush_transmit_message(rf);
		}

		if (send_message)
		{
			read_adc();											//get converted ADC value
			send_message = false;								
			nRF24L01Message msg;
			snprintf(outs,sizeof(outs),"%3d\r\n", adc_temp);	//convert temp to a string
			USART_tx_string(outs);								//display the value
			memcpy(msg.data, outs, 3);
			msg.length = strlen((char *)msg.data) + 1;
			nRF24L01_transmit(rf, to_address, &msg);			//transmit the value to receiver
		}
	}
	return 0;
}

void adc_init(void)
{
	/** Setup and enable ADC **/
	ADMUX = 0;					//select ADC0 Pin as input
	ADMUX = (0<<REFS1)| 		//Reference Selection Bits
	(1<<REFS0)| 				//AVcc - external cap at AREF
	(1<<ADLAR); 				//ADC right Adjust Result
	
	ADCSRA = (1<<ADEN)| 		//ADC ENable
	(1<<ADSC)| 					//ADC Start Conversion
	(1<<ADATE)| 				//ADC Auto Trigger Enable
	(0<<ADIF)| 					//ADC Interrupt Flag
	(0<<ADIE)| 					//ADC Interrupt Enable
	(1<<ADPS2)| 				//ADC Prescaler of 64
	(1<<ADPS1)|
	(0<<ADPS0);
	ADCSRB = 0;					//free running mode
}



nRF24L01 *setup_rf(void)
{
	nRF24L01 *rf = nRF24L01_init();

	rf->ss.port = &PORTB;
	rf->ss.pin = PB2;
	rf->ce.port = &PORTB;
	rf->ce.pin = PB1;
	rf->sck.port = &PORTB;
	rf->sck.pin = PB5;
	rf->mosi.port = &PORTB;
	rf->mosi.pin = PB3;
	rf->miso.port = &PORTB;
	rf->miso.pin = PB4;
	// interrupt on falling edge of INT0 (PD2)
	EICRA |= _BV(ISC01);
	EIMSK |= _BV(INT0);
	nRF24L01_begin(rf);
	return rf;
}

void read_adc(void)
{
	
	adc_temp = 0;							//initalize temp to 0
	ADCSRA |= (1<<ADSC);					//start the conversion
	while((ADCSRA & (1<<ADIF)) == 0);
	{
		//wait for conversion to finish
	}
	adc_temp = ADCH;						//get temp value
}



void spi_init(void)
{
	DDRB |= (1<<2)|(1<<3)|(1<<5); 		// SCK, MOSI and SS as outputs
	DDRB &= ~(1<<4); 					// MISO as input
	SPCR |= (1<<MSTR); 					// Set as Master
	SPCR |= (1<<SPR0)|(1<<SPR1); 		// divided clock by 128
	SPCR |= (1<<SPE); 					// Enable SPI
}

// setup timer to trigger interrupt every second when at 8MHz
void setup_timer(void)
{
	TCCR1B |= _BV(WGM12);
	TIMSK1 |= _BV(OCIE1A);
	OCR1A = 31250;
	TCCR1B |= _BV(CS12);
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
		while (!(UCSR0A & (1 <<UDRE0)))
		{
			//wait for the transmit buffer to empty
		}
		UDR0 = *data;				//put the data into the empty buffer, which sends the data
		_delay_ms(125); 			// wait a bit
		data++;
	}
}

// each one second interrupt
ISR(TIMER1_COMPA_vect)
{
	send_message = true;		
	TIFR1 |= (1<<OCF1A);
}

// nRF24L01 interrupt
ISR(INT0_vect)
{
	rf_interrupt = true;
	EIFR |= (1<<INTF0);
}
