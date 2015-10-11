/** @file ADC.c
 * @see ADC.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "ADC.h"

//--------------------------------------------------------------------------------------------------
// Private constants and macros
//--------------------------------------------------------------------------------------------------
/** Enable the timer 0 interrupt. */
#define ADC_ENABLE_TIMER_0_INTERRUPT() intcon.TMR0IE = 1
/** Disable the timer 0 interrupt. */
#define ADC_DISABLE_TIMER_0_INTERRUPT() intcon.TMR0IE = 0

//--------------------------------------------------------------------------------------------------
// Private variables
//--------------------------------------------------------------------------------------------------
/** The last sampled battery voltage. */
static volatile unsigned short ADC_Last_Sampled_Voltage;

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
	
	// Configure the timer0 to overflow every second
	t0con = 0x87; // Enable the timer in 16-bit mode, use Fosc/4 as clock source, use a 1:256 prescaler
	
	// Enable the timer interrupt
	intcon2.TMR0IP = 0; // Set the interrupt as low priority
	intcon.TMR0IF = 0; // Reset the interrupt flag
	ADC_ENABLE_TIMER_0_INTERRUPT();
}

unsigned short ADCGetLastSampledBatteryVoltage(void)
{
	unsigned short Voltage;
	
	// Atomically access to the shared variable
	ADC_DISABLE_TIMER_0_INTERRUPT();
	Voltage = ADC_Last_Sampled_Voltage;
	ADC_ENABLE_TIMER_0_INTERRUPT();
	
	return Voltage;
}

void ADCInterruptHandler(void)
{
	// Store the last sampled value
	ADC_Last_Sampled_Voltage = ((adresh & 0x03) << 8) | adresl;
	
	// TODO : weak battery protection code
	
	// Start a new conversion
	adcon0.GO = 1;

	// Reset the interrupt flag
	intcon.TMR0IF = 0;
}