/*
 * DA4_Task1.c
 *
 * Created: 4/13/2018 3:40:25 PM
 * Author : Nathan Hanuscin
 */ 

#include <avr/io.h>
#define  F_CPU 8000000UL
#include <avr/interrupt.h>
#include <util/delay.h>

int main()
{
	
	ADCSRA = 0x87;			//ADC enable and ADC prescaler 128
	ADMUX = 0x60;			//AVcc as reference (5V) and left justified
	DDRB = 0xFF;			//set PORTB as output
	PORTD |= (1<<2);		//set up pull up resistor
	OCR1A = 0;				//0% duty cycle initially
	TCCR1B = 0x0D;			//prescaler of 1024
	TCCR1A = 0x83;			//non-inverting mode, fast PWM 10 bit
	EIMSK |= (1<<INT0);		//enable external interrupt 0
	EICRA |= (1<<ISC01);	//falling edge trigger
	sei();					//enable interrupts
	
	while (1)
	{
		ADCSRA |= (1<<ADSC);			//start conversion
		while ((ADCSRA & (1<<ADIF)) == 0)
		{
			//wait for conversion to finish
		}
		
	}
}
ISR (INT0_vect)
{
	
	EIFR |= (1<<INTF0);		//reset flag
	if((PORTB & 0b00000001) == 0b00000000)		//check if sending enable signal
	{
		PORTB |= (1<<0);						//toggle enable if not
		if(ADCH > 220)
		{
			OCR1A = 973;		//95% duty cycle
		}
		else
			OCR1A = 0;			//0% duty cycle
	}
	else
	{
		PORTB &= ~(1<<0);		//turn enable off
		OCR1A = 0;				//0% duty cycle
	}
	
	
}
