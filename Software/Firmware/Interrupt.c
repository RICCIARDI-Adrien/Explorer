/** Interrupt.c
 * Gather all interrupt handlers.
 * @author Adrien RICCIARDI
 */
#include <system.h>
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
}