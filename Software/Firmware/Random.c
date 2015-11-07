/** @file Random.c
 * @see Random.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Random.h"

//--------------------------------------------------------------------------------------------------
// Private variables
//--------------------------------------------------------------------------------------------------
/** The random seed. */
static unsigned char Random_Seed;

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void RandomInitialize(void)
{
	// Use the unknown value stored in the CCPR5 registers to initialize the random number generator (it is possible because CCP5 module is not used, so it's content is not altered)
	Random_Seed = ccpr5l;

	// Initialize the timer 4 to count at its maximum frequency
	t4con = 0x04; // Enable the timer with a 1:1 prescaler and 1:1 postscaler
}

unsigned char RandomGetNumber(void)
{
	Random_Seed = (((Random_Seed << 1) + Random_Seed) - 7) ^ tmr4; // New_Seed = (Previous_Seed * 3 - 7) XOR Arbitrary_Value
	return Random_Seed;
}