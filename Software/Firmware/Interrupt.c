/** Interrupt.c
 * Gather all interrupt handlers.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "ADC.h"
#include "Distance_Sensor.h"
#include "Motor.h"
#include "Shared_Timer.h"
#include "UART.h"

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void interrupt(void)
{
	// External interrupt 1
	if (intcon3.INT1IE && intcon3.INT1IF) DistanceSensorInterruptHandler();
}

void interrupt_low(void)
{
	// UART RX and TX interrupts
	if (pir3.RC2IF || pir3.TX2IF) UARTInterruptHandler();
	
	// Handle the motors speed
	MotorInterruptHandler(); // The relevant interrupt flags are checked in the function
	
	// Timer 3 interrupts
	if (pie2.TMR3IE && pir2.TMR3IF) SharedTimerInterruptHandler();
}