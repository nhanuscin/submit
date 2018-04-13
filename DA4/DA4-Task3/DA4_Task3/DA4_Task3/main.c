/*
 * DA4_Task3.c
 *
 * Created: 4/13/2018 3:54:20 PM
 * Author : Nathan Hanuscin
 */ 

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>

int main(void)
{
	DDRC = 0;												//set PC0 as input for ADC
	ADCSRA = 0x87;											//ADC enable and ADC pre-scaler 128
	ADMUX = 0x60;											//AVcc as reference (5V) and left justified
	TCCR1A |= (1<<COM1A1) | (1<<COM1B1)| (1<<WGM11);		//clear OC1A and OC1B on compare match
	TCCR1B |= (1<<WGM13)| (1<<WGM12)| (1<<CS11)|(1<<CS10);	//pre-scaler 64, fast PWM with ICR1 as top value
	ICR1 = 2500;											//fPWM = 50Hz, period 20ms
	DDRB= 0xFF;												//PortB as output
	unsigned int angle;
	while (1)
	{
		ADCSRA |= (1<<ADSC);								//start conversion
		while ((ADCSRA & (1<<ADIF)) == 0)
		{
			//wait for conversion to finish
		}
		ADCSRA |= (1<<ADIF);								//reset flag bit
		angle = ADCH;										//store ADC value into variable
		OCR1A = (angle/2)+125;								//convert ADC value to PWM value
		_delay_ms(1500);									//delay 1.5 seconds for motor to move
		//OCR1A value will range from 125 to approximately 250 which corresponds to a 1ms to 2ms pulse
		
	}
}


