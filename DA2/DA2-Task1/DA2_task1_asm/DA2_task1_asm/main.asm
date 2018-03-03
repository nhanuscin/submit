;
; DA2_task1.asm
;
; Created: 2/22/2018 10:35:16 AM
; Author : nhanu
;


; Replace with your application code
.org 0x00
	LDI R16, HIGH(RAMEND)
	OUT SPH, R16
	LDI R16, LOW(RAMEND)
	OUT SPL, R16

	SBI DDRB, 2				;set PORT2 as output
	LDI R17, 0				;used to intialize PB2 OFF
	OUT PORTB, R17			;turn PB2 off
	LDI R16, 4				;used to toggle LED

LOOP:
	rcall myDelay			;call delay subroutine
	EOR R17, R16			;toggle bits of R17
	OUT PORTB, R17			;toggle LED
	RJMP LOOP				;continue looping

myDelay:
	LDI R18, 250			;counter for delay
L1:	
	LDI R19, 250			;second nested counter for delay
L2:
	NOP						;take 1 clock cycle 250*250 times
	DEC R19					;take 1 clock cycle 250*250 times and decrement 2nd count
	BRNE L2					;keeping delaying if not zero
	DEC R18					;decrement primary counter
	BRNE L1					;go into nested loop if not zero
	RET						;finished .25 second delay
