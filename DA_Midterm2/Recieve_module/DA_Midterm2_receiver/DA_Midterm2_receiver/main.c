/*
 * DA_Midterm2_receiver.c
 *
 * Created: 4/23/2018 3:47:10 PM
 * Author : Nathan Hanuscin and Brain Lopez
 */ 


#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "nrf24l01.h"

volatile bool rf_interrupt = false;

nRF24L01 *setup_rf(void){
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
	EICRA |= _BV(ISC01);
	EIMSK |= _BV(INT0);
	nRF24L01_begin(rf);
	return rf;
}

void spi_init() {
	DDRB &= ~((1<<2)|(1<<3)|(1<<5));   	//SCK, MOSI and SS as inputs
	DDRB |= (1<<4);     			// MISO as output
	SPCR &= !(1<<MSTR);    			// set as slave
	SPCR |= (1<<SPR0)|(1<<SPR1);  		// divide clock by 128
	SPCR |= (1<<SPE);       		// enable SPI
}

void init_uart(){
	// setting the baud rate  based on FCPU and baudrate
	UBRR0H =0x00;
	UBRR0L =0x0C;
	// enabling TX & RX
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);              // enable receive and transmit
	UCSR0A = (1<<UDRE0)|(1<<U2X0);
	UCSR0C =  (1 << UCSZ01) | (1 << UCSZ00);    // Set frame: 8data, 1 stop

}


void USART_Transmit( char *data)
{
	while((*data != '\0')) {    // transmits all chars but null
		while(!(UCSR0A & (1<<UDRE0)));  // waits for transmit flag to clear
		UDR0 = *data;           // transmit next char
		data++;                 // move to next char
	}
}


void process_message(char *message) {
	char out[20];
	snprintf(out, sizeof(out), "Temperature read is: %s", message);
	USART_Transmit(out);
}

// nRF24L01 interrupt

ISR(INT0_vect) {
	rf_interrupt = true;
	EIFR |= (INTF0);
}

int main(void)
{
	init_uart();			//initialize uart
	spi_init();				//initialize spi			
	_delay_ms(150);
	USART_Transmit("Started!\r\n");
	uint8_t address[5] = {0x01, 0x01, 0x01, 0x01, 0x01 };
	sei();
	nRF24L01 *rf = setup_rf();
	nRF24L01_listen(rf, 0, address);
	uint8_t addr[5];
	nRF24L01_read_register(rf, 0x00, addr, 1);
	
	
	while (1)
	{
		if (rf_interrupt)
		{
			rf_interrupt = false;
			while (nRF24L01_data_received(rf)) {
				nRF24L01Message msg;
				nRF24L01_read_received_data(rf, &msg);
				USART_Transmit((char *)msg.data);
				USART_Transmit("\r\n");
			}
			
			nRF24L01_listen(rf, 0, address);
		}
	}
	
	return 0;
}


