.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO

.text
	.global Start

Start:

/*	LDR R0, =BUTTON_PORT 		//port 1
	LDR R1, =PORT_SIZE 			//36 registers
	MUL R0, R0, R1 				//register 36
	LDR R1, =GPIO_BASE 			//start register
	ADD R0, R0, R1 				//startAdress + 36
	LDR R1, =GPIO_PORT_DIN		//button port data in
	ADD R2, R0, R1				//R3 = button DIN address
*/

	// Compressed version of above (for button)
	LDR R0, =GPIO_BASE + (PORT_SIZE * LED_PORT) + GPIO_PORT_DOUTSET
	LDR R1, =GPIO_BASE + (PORT_SIZE * LED_PORT) + GPIO_PORT_DOUTCLR
	LDR R2, =GPIO_BASE + (PORT_SIZE * BUTTON_PORT) + GPIO_PORT_DIN


	LDR R3, =#1
	LSL R4, R3, #LED_PIN			// Set bit 1 (leftshifted to LED_PIN), for setting DOUTSET og DOUTCLR
	LSL R5, R3, #BUTTON_PIN			// Set bit 1 (leftshifted to BUTTON_PIN)

Loop:
	LDR R3, [R2]		// Get button status
	AND R3, R3, R5		// Remove unwanted values
	CMP R3, R5			// If value in R5 is equal to R3 ->
	BEQ On				// JMP to On, else continue to Off
Off:
	STR R4, [R0]		// Turn LED on
	B Loop				// JMP to loop
On:
	STR R4, [R1]		// Turn LED off
	B Loop				// JMP to loop



NOP // Behold denne på bunnen av fila
