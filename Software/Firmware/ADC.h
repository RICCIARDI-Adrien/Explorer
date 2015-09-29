/** @file ADC.h
 * Sample the battery voltage.
 * @author Adrien RICCIARDI
 */
#ifndef H_ADC_H
#define H_ADC_H

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the ADC module to sample the battery voltage. */
void ADCInitialize(void);

/** Return the last sampled battery voltage value.
 * @return The raw value of the last sampled battery voltage. */
unsigned short ADCGetLastSampledBatteryVoltage(void);

/** The ADC interrupt handler. */
void ADCInterruptHandler(void);

#endif