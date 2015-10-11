/** @file Led.h
 * Allow to light or to turn off the led.
 * @author Adrien RICCIARDI
 */
#ifndef H_LED_H
#define H_LED_H

#include <system.h>

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the led pins. */
inline void LedInitialize(void)
{
	// Set the led pins as output
	trisb.5 = 0;
	trisb.4 = 0;
	
	// Turn off the led
	latb.5 = 0;
	latb.4 = 0;
}

/** Light the led in green. */
inline void LedOnGreen(void)
{
	latb.5 = 1;
	latb.4 = 0;
}

/** Light the led in red. */
inline void LedOnRed(void)
{
	latb.5 = 0;
	latb.4 = 1;
}

/** Turn the led off. */
inline void LedOff(void)
{
	latb.5 = 0;
	latb.4 = 0;
}

#endif