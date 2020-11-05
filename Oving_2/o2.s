.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO
.include "sys-tick_constants.s" // Register-adresser og konstanter for SysTick

.text
	.global Start

Start:
	//SYSTICK
	LDR R0, =SYSTICK_BASE
    LDR R1, [R0]
    ADD R1, R1, 0b110
    STR R1, [R0]

    //SET CLOCKCYCLES
	LDR R2, =FREQUENCY/10
	LDR r3, =SYSTICK_BASE + SYSTICK_LOAD
    STR R2, [R3]

    //SET VAL
    LDR R0, =SYSTICK_BASE + SYSTICK_VAL
    MOV R1, #0
    STR R1, [R0]

	//SET PIN
	ldr r2, =SYSTICK_BASE + SYSTICK_CTRL
	ldr r3, =SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk
	str r3, [r3]

	//SET GPIO INTERRUPT
	ldr R2, =GPIO_BASE + GPIO_EXTIPSELH
	ldr r3, [r2]
	and r3, ~(0b1111 << 4)
	orr r3, 0b0001 << 4
	str r3, [r2]

	//EXTIFALL
	LDR R0, =GPIO_BASE + GPIO_EXTIFALL
	LDR R1, [R0]
	MOV R2, #1
	LSL R2, R2, #9
	ORR R2, R2, R1
	STR R2, [R0]

	//RESET IINTERUPT FLAG
	LDR R2, =GPIO_BASE+GPIO_IFC
	LDR R3, =1<<9
	STR R3, [R2]

	//INTERUPT ENABLE
	LDR R0, =GPIO_BASE + GPIO_IEN
	LDR R1, [R0]
	MOV R2, #1
	LSL R2, R2, #9
	ORR R2, R2, R1
	STR R2, [R0]

	B LOOP

	LOOP:
		B LOOP

	.global SysTick_Handler
	.thumb_func
	SysTick_Handler:
		COUNT_TENTHS:
		//COUNTING
		LDR R0, =tenths
    	LDR R1, [R0]
    	ADD R1, R1, #1
    	STR R1, [R0]
    	CMP R1, #10
    	BEQ COUNT_SECONDS
    	BX LR

    	COUNT_SECONDS:
		//COUNTING
		LDR R2, =seconds
		LDR R3, [R2]
    	ADD R3, R3, #1
    	STR R3, [R2]

    	//LIGHT
    	MOV R6, #0
    	STR R6, [R0]
    	LDR R0, =GPIO_BASE + LED_PORT * PORT_SIZE + GPIO_PORT_DOUTTGL
		LDR R6, =1<<LED_PIN
		STR R6, [r0]

    	CMP R3, #60
    	BEQ COUNT_MINUTES
    	BX LR

    	COUNT_MINUTES:
    	//COUNTING
    	STR R6, [R2]

		LDR R3, =minutes
    	LDR R7, [R3]
    	ADD R7, R7, #1
    	STR R7, [R3]

    	BX LR

	.global GPIO_ODD_IRQHandler
	.thumb_func
	GPIO_ODD_IRQHandler:

		LDR R4, =SYSTICK_BASE
		LDR R5, [R4]
		EOR R5, R5, #1
		STR R5, [R4]

		//RESET IINTERUPT FLAG
		LDR R4, =GPIO_BASE+GPIO_IFC
		LDR R5, =1<<9
		STR R5, [R4]

		BX LR

NOP // Behold denne på bunnen av fila
