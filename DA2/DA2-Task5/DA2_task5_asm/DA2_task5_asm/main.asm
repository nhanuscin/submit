;
; DA2_task5_asm.asm
;
; Created: 3/2/2018 4:47:15 PM
; Author : nhanus
;


.ORG 0							;location for reset
	JMP MAIN
.ORG 0x02						;location for EXT_INT0
	JMP EX0_ISR
.ORG 0x1A						;location for TIM1_OVF
	JMP T1_OV_ISR

MAIN: 
	LDI R20,HIGH(RAMEND)		;initialize the stack
	OUT SPH,R20
	LDI R20,LOW(RAMEND)
	OUT SPL,R20
	SBI DDRB,2 				;PC.3 = output
	SBI PORTD,2 			;pull-up activated
	LDI R20,1<<INT0 		;Enable INT0
	OUT EIMSK,R20			;Enable INT0
	LDI R20,(1<<ISC01) 		;Configure to falling edge triggered
	sts EICRA,R20			
	LDI R20,(1<<TOIE1)		;Enable timer1 overflow interrupt
	STS TIMSK1, R20			;Enable TOIE1
	SEI 					;Set I (Enable Interrupts)
	
HERE:
	JMP HERE				;wait for interrupt

EX0_ISR: 
	LDI R20, 1<<INTF0		;load in flag position
	OUT EIFR, R20 			;clear flag
	LDI R22,0x04			;send 1 to PB2
	OUT PORTB,R22			;turn on LED
	LDI R17, 0xF0			;get upper bits of 61630
	STS TCNT1H, R17			;set high bits of counter
	LDI R17, 0xBE			;get lower bits of 61630	
	STS TCNT1L, R17			;set lower bits
	LDI R17, 0			
	STS TCCR1A, R17			;normal mode
	LDI R17, 4					
	STS TCCR1B, R17			;set prescaler to 256 
	RETI					;finish interrupt

T1_OV_ISR:
	LDI R20, 1<<TOV1		;clear flag bit
	STS TIFR1, R20			
	LDI R17, 0	
	OUT PORTB, R17			;turn off LED
	STS TCCR1B, R17			;turn off timer			
	RETI					;finish interrupt
