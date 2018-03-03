/*
 * da2_task3_c.c
 *
 * Created: 2/22/2018 12:52:53 PM
 * Author : hanusn1
 */ 

#include <avr/io.h>


int main(void)
{
    DDRB |= (1<<2);				//set PB2 as output
	PORTB &= ~(1<<2);			//turn PB2 LED off
	TCCR0A = 0;					//Timer0, normal mode, initialize clock
	TCCR0B = 5;					//prescaler of 1024
	TCNT0 = 12;					//250ms delay value
	
	
    while (1) 
    {
		if(TIFR0 & (1 << TOV0))			//If overflow bit is high
		{
			PORTB ^= (1<<2);			//toggle LED
			TIFR0 |= (1<<TOV0);			//reset overflow bit
			TCNT0 = 12;				//reset counter
		}
    }
}

