/** @File Random.h
 * Generate pseudo-random numbers.
 * @author Adrien RICCIARDI
 */
#ifndef H_RANDOM_H
#define H_RANDOM_H

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the random numbers generator with a truly random seed sampled from a non-connected ADC pin.
 * @warning This function must be called after ADCInitialize() and before the interrupts enabling.
 */
void RandomInitialize(void);

/** Generate a pseudo-random number.
 * @Return The pseudo-random number.
 */
unsigned char RandomGetNumber(void);

#endif