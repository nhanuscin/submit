;
; DA2_task3_asm.asm
;
; Created: 3/2/2018 4:15:12 PM
; Author : Owner
;


; Replace with your application code
.org 0
	LDI R16, 4			;used to toggle LED
	LDI R18, 0			;used to initialize TCCR0A
	SBI DDRB, 2 		;PB2 as output
	LDI R17,0 			;needed to toogle led
	OUT PORTB, R17		;turn LED off
begin:
	LDI R20, 12			;250ms delay with 1024 prescaler
	OUT TCNT0, R20		;load value into timer
	LDI R20, 5 			;to set prescaler
	OUT TCCR0B, R20 	;Prescaler: 1024
	OUT TCCR0A, R18		;Timer0, normal mode, initalize clock
loop:
	IN R20, TIFR0		;read in TIFR0
	SBRS R20, 0			;if TOV0 is set skip next instruction
	RJMP LOOP			;keep polling

	LDI R20, 0
	OUT TCCR0B, R20		;stop the timer
	LDI R20, 1	
	OUT TIFR0, R20		;reset TOV0 flag
	EOR R17, R16 		;XOR to toogle led
	OUT PORTB, R17		;toggle LED
	RJMP begin 			;reset
