;
; CPE301_Ast1.asm
;
; Created: 2/11/2018 6:48:14 PM
; Author : Nathan Hanuscin
; Assignment 1


; Replace with your application code
.def countL = r24					;counter lower bits
.def countH = r25					;counter higher bits
.def sum_5L = r16					;sum of numbers divisible by 5 lower bits
.def sum_5H	= r17					;upper bits
.def sum_N5L = r18					;sum of numbers not divisible by 5 lower bits
.def sum_N5H = r19					;upper bits

.org 0x0

	ldi r16, LOW(RAMEND)			;initialize the stack
	out spl, r16
	ldi r16, HIGH(RAMEND)
	out sph, r16

	ldi ZH, HIGH(0x0222)			;starting address upper bits
	ldi ZL, LOW(0x0222)				;starting address lower bits
	ldi countL, LOW(300)			;initalizing count to 300 in 
	ldi countH, HIGH(300)			;registers r24 and r25
	ldi r16, 1						;staring value to store at address 0x0222
LOOP1:
	st Z+, r16						;store number at address Z is pointing to then increment it
	inc r16							;increment the value being stored
	sbiw countL, 1					;decrement the counter
	brne LOOP1						;if counter > 0 keep storing
	
	;loop:							
		;jmp loop

	ldi XH, HIGH(0x0400)			;start address of where to 
	ldi XL, LOW(0x0400)				;store multiples of 5
	ldi YH, HIGH(0x0600)			;start address of where to
	ldi YL, LOW(0x0600)				;store non-multiples of 5
	ldi countL, LOW(300)			;reset counter to 300
	ldi countH, HIGH(300)			
	ldi sum_5L, 0					;initialize divisible by 5 sum
	ldi sum_5H, 0					;to zero
	ldi sum_N5L, 0					;initialize not divisible by 5
	ldi sum_N5H, 0					;sum of zero
	ld r20, -Z						;get last value inputted into r20

check5:
	cpi r20, 4						;if r20 % 5 = 4
	breq notMultiple				;not a multiple
	cpi r20, 3						;if r20 % 5 = 3
	breq notMultiple				;not a multiple
	cpi r20, 2						;if r20 % 5 = 2
	breq notMultiple				;not a multiple
	cpi r20, 1						;if r20 % 5 = 1
	breq notMultiple				;not a multiple
	cpi r20, 0						;if r20 % 5 = 0
	breq Multiple					;multiple of 5
	subi r20, 5						;else subtract 5 from number
	jmp check5						;check remainders again

notMultiple:
	ld r20, Z						;re-read in original value before subtraction
	st Y+, r20						;store non-multiple of 5 at 0x0600 start addr then inc addr
	add sum_N5L, r20				;add value to not divisible by 5 sum
	brcc skip1						;if carry bit cleared skip incrementing the uppper bit
	inc sum_N5H						;add carry to upper bits
skip1:
	jmp getNext

Multiple:
	ld r20, Z						;re-read in original value
	st X+, r20						;store multiple of 5 at 0x0400 start addr then inc addr
	add sum_5L, r20					;add value to divisible by 5 sum
	brcc skip2						;if carry bit cleared skip incrementing the upper bit
	inc sum_5H						;add carry to upper bits
skip2:
	jmp getNext

getNext:
	ld r20, -Z						;decrement stored values addr and store next value in r20
	sbiw countL, 1					;decrement the counter
	brne check5						;if count > 0 keep looping

div5Done:
	jmp div5Done