/*
 * da2_task5_c.c
 *
 * Created: 3/1/2018 1:15:57 PM
 * Author : hanusn1
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

int main()
{
    DDRB = 1<<2;			//set PB2 as output
	PORTD = 1<<2;			//set up pull up resistor
	EIMSK = (1<<INT0);		//enable external interrupt 0
	EICRA = (1<<ISC01);		//falling edge trigger
	TIMSK1 = (1<<TOIE1);	//overflow interrupt timer1 enabled
	sei();
	
    while (1) 
    {
		//wait for interrupts
    }
}
ISR (INT0_vect)
{
	EIFR |= (1<<INTF0);		//reset flag
	PORTB |= (1<<2);		//turn on LED
	TCNT1 = 61630;			//set counter value
	TCCR1A = 0;				//normal mode
	TCCR1B = (1<<CS12);		//set prescaler to 256
}

ISR (TIMER1_OVF_vect)
{
	TIFR1 |= (1<<TOV1);		//reset flag
	PORTB &= ~(1<<2);		//turn LED off
	TCCR1B = 0;				//turn off timer
}
