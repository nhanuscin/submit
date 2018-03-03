/*
 * DA2_task1_c.c
 *
 * Created: 2/22/2018 11:02:39 AM
 * Author : nhanu
 */ 

#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>

int main()
{
    DDRB |= (1<<2);				//set PB2 as output
	PORTB &= ~(1<<2);			//set PB2 OFF
	
    while (1) 
    {
		PORTB |= (1<<2);		//Turn on LED
		_delay_ms(250);			//wait 250ms
		PORTB &= ~(1<<2);		//Turn off LED
		_delay_ms(250);			//wait 250ms
    }
}


