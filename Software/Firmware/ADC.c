/** @file ADC.c
 * @see ADC.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "ADC.h"
#include "Led.h"
#include "Motor.h"
#include "Shared_Timer.h"

//--------------------------------------------------------------------------------------------------
// Private variables
//--------------------------------------------------------------------------------------------------
/** The last sampled battery voltage. */
static volatile unsigned short ADC_Last_Sampled_Voltage = 0;

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void ADCInitialize(void)
{
	// Configure RA0 as analog pin
	trisa.0 = 1;
	ansela.0 = 1;
	
	// Configure the ADC module
	adcon1 = 0; // Use Vdd as positive voltage reference and Vss as negative voltage reference
	adcon2 = 0x86; // Right-justify the result, use a 0Tad acquisition time as the acquisition frequency is really slow (1Hz), use a conversion clock of Fosc/64 to grant a correct ADC clock
	adcon0 = 0x01; // Select the RA0 pin and enable the ADC module
	
	// Start a conversion to fill the last sampled voltage with a coherent value
	adcon0.GO = 1;
	while (adcon0.GO); // Wait for the conversion to finish
	ADC_Last_Sampled_Voltage = ((adresh & 0x03) << 8) | adresl;
}

void ADCScheduleBatteryVoltageSampling(void)
{
	// Store the last sampled value
	ADC_Last_Sampled_Voltage = ((adresh & 0x03) << 8) | adresl;
	
	// Put the robot in protection mode if the battery is too weak
	if (ADC_Last_Sampled_Voltage < ADC_WEAK_BATTERY_VOLTAGE)
	{
		// Stop the motors
		MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
		MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
		
		// Make the led blinks in red
		while (1)
		{
			LedOnRed();
			delay_ms(250);
			LedOff();
			delay_ms(250);
		}
	}
	
	// Start a new conversion
	adcon0.GO = 1;
}

unsigned short ADCGetLastSampledBatteryVoltage(void)
{
	unsigned short Voltage;
	
	// Atomically access to the shared variable
	SHARED_TIMER_DISABLE_INTERRUPT();
	Voltage = ADC_Last_Sampled_Voltage;
	SHARED_TIMER_ENABLE_INTERRUPT();
	
	return Voltage;
}