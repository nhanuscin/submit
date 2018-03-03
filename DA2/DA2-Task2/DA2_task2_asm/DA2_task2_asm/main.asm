;
; DA2_task2_asm.asm
;
; Created: 2/22/2018 11:13:52 AM
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
	OUT PORTB, R17			;set PB2 OFF
	CBI DDRD, 2				;set PD2 as input
	LDI R17 ,0x04			;set PD2 as input
	OUT PORTD, R17			;set PD2 as input
	LDI R25, 0

LOOP:
	IN R16, PIND			;R16 gets PIND values
	CPI R16, 0x00			;check if button was pressed 
	BRNE LOOP				;if not 0 keep polling
	OUT PORTB, R17			;Turn on LED
	rcall myDelay250ms		;call 250ms delay 4 times for 
	rcall myDelay250ms		;overall 1 second delay
	rcall myDelay250ms
	rcall myDelay250ms
	OUT PORTB, R25			;turn off LED
	jmp LOOP

myDelay250ms:
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



