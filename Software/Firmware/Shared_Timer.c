/** @file Shared_Timer.c
 * @see Shared_Timer.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "ADC.h"
#include "Shared_Timer.h"

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void SharedTimerInitialize(void)
{
	// Configure the timer 3 to trigger an interrupt at a 30Hz frequency (the lowest frequency that can be achieved with the available input clocks)
	tmr3h = 0xFF; // Trigger an interrupt immediately after they are enabled
	tmr3l = 0xFF;
	t3con = 0x33; // Use Fosc/4 as clock source, use a 8x prescaler, disable the dedicated secondary oscillator circuit, access to the timer registers in one 16-bit operation, enable the timer
	
	// Enable timer 3 interrupt
	ipr2.TMR3IP = 0; // Set interrupt as low priority
	pie2.TMR3IE = 1;
}

void SharedTimerInterruptHandler(void)
{
	static unsigned char Frequency_Divider_1Hz = 0;
	
	// Schedule a battery voltage measure every second
	Frequency_Divider_1Hz++;
	if (Frequency_Divider_1Hz >= 30)
	{
		ADCScheduleBatteryVoltageSampling();
		Frequency_Divider_1Hz = 0;
	}
	
	// Clear the interrupt flag
	pir2.TMR3IF = 0;
}