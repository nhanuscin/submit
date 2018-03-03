;
; da2_task4asm.asm
;
; Created: 3/1/2018 11:45:05 AM
; Author : hanusn1
;


; Replace with your application code
.org 0x0
	jmp MAIN
.org 0x1A						;addr for Timer1 overflow
	jmp T1_OV_ISR
.org 0x100
MAIN:
	ldi R17, HIGH(RAMEND)		;initialize the stack
	out SPH, R17
	ldi R17, LOW(RAMEND)
	out SPL, R17
	sbi DDRB, 2				;set PB2 as output
	ldi R17, 0			
	out PORTB, R17			;turn off LED initially
	ldi R17, 0xF0			;upper bits of 61630
	sts TCNT1H, R17			;set high bits of counter
	ldi R17, 0xBE			;lower bits of 61630
	sts TCNT1L, R17			;set low bits
	ldi R17, 0			
	sts TCCR1A, R17			;normal mode
	ldi R17, 3					
	sts TCCR1B, R17			;set prescaler to 64 
	ldi R17, (1<<TOIE1)
	sts TIMSK1, R17			;set flag bit
	sei						;enable the interrupt
again:
	jmp again				;loop until interrupt occurs

T1_OV_ISR:
	LDI R20, 1<<TOV1		;clear the flag bit
	sts TIFR1, R20			;flag bit cleared
	IN R16, PORTB			;read in PB2
	LDI R17, 0x04
	EOR R16, R17			;toggle PB2
	OUT PORTB, R16			;toggle LED
	ldi R17, 0xF0			;reload upper bits
	sts TCNT1H, R17			;reset the couter
	ldi R17, 0xBE			;reload lower bits	
	sts TCNT1L, R17			;reset the counter	
	RETI					;return