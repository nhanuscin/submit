/*
 * DA4_Task2.c
 *
 * Created: 4/13/2018 3:48:18 PM
 * Author : Nathan Hanuscin
 */ 


#include <avr/io.h>
#define F_CPU 8000000UL

int main(void)
{
	unsigned int speed;										//holder for ADCH value
	DDRC = 0x00;											//set port c as an input
	DDRD = 0xFF;											//set port d as an output
	DDRC = 0;												//set PC0 as input for ADC
	ADCSRA = 0x87;											//ADC enable and ADC prescaler 128
	ADMUX = 0x60;											//AVcc as reference (5V) and left justified
	TCCR1A = 0;												//initialize TCCR1A
	TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10); 	//mode CTC prescaler 1024
	OCR1A = 781;											//100ms second delay
	TCNT1 = 0;												//initialize counter
	
	
	while (1)
	{
		while ((TIFR1 & (1 << OCF1A)) == 0)
		{
			//wait for timer overflow
		}
		PORTD = 0x03;
		TIFR1 |= (1<< OCF1A);		//reset counter flag

		while ((TIFR1 & (1 << OCF1A))== 0)
		{
			//wait for timer overflow
		}
		PORTD = 0x42;
		TIFR1 |= (1<< OCF1A);		//reset counter flag

		while ((TIFR1 & (1 << OCF1A))== 0)
		{
			//wait for timer overflow
		}
		PORTD = 0xC0;
		TIFR1 |= (1<< OCF1A);		//reset counter flag
		
		while ((TIFR1 & (1 << OCF1A))== 0)
		{
			//wait for timer overflow
		}
		PORTD = 0x81;
		TIFR1 |= (1<< OCF1A);		//reset counter flag
		
		//rotation finished
		//get new delay value
		ADCSRA |= (1<<ADSC);			//start conversion
		while ((ADCSRA & (1<<ADIF)) == 0)
		{
			//wait for conversion to finish
		}
		speed = ADCH;					//get ADCH value
		OCR1A = 781 + (speed*12);		//update speed 100ms min to 500ms max
		

		
	}
}


