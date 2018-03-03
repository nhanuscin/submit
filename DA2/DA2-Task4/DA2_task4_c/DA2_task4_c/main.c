/*
 * da2_task4c.c
 *
 * Created: 3/1/2018 12:13:08 PM
 * Author : hanusn1
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

int main()
{
    DDRB |= 0x04;			//set PB2 as output
	PORTB = 0;				//initialize LED off
	TCCR1A = 0;				//normal mode
	TCCR1B = 3;				//set pre-scaler to 64
	TCNT1 = 61630;			//set timer value
	TIMSK1 = (1<<TOIE1);	//enable overflow interrupt
	sei ();					//enable interrupts
	
	while(1)
	{
		//wait for interrupt
	}
}

ISR (TIMER1_OVF_vect)
{
	TIFR1 |= (1<<TOV1);		//reset flag bit
	PORTB ^= 0x04;			//toggle LED
	TCNT1 = 61630;			//reset timer
}