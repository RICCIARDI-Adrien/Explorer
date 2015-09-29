/** Interrupt.c
 * Gather all interrupt handlers.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "ADC.h"
#include "UART.h"

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void interrupt(void)
{
}

void interrupt_low(void)
{
	// UART interrupts
	if (pir3.RC2IF || pir3.TX2IF) UARTInterruptHandler();
	
	// Timer 0 interrupt (which starts the battery sampling)
	if (intcon.TMR0IF) ADCInterruptHandler();
}