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
	if ((pie3.RC2IE && pir3.RC2IF) || (pie3.TX2IE && pir3.TX2IF)) UARTInterruptHandler();
	
	// Timer 1, ECCP1 and ECCP2 interrupts
	MotorInterruptHandler(); // The relevant interrupt flags are checked in the function
	
	// Timer 2 interrupt
	if (pir1.TMR2IF) DistanceSensorTriggerPinInterruptHandler();
	
	// Timer 3 interrupt
	if (pie2.TMR3IE && pir2.TMR3IF) SharedTimerInterruptHandler();
}