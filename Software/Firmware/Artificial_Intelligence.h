/** @file Artificial_Intelligence.h
 * Provide utility functions used by all artificial intelligence behaviors and gather all behaviors.
 * @author Adrien RICCIARDI
 */
#ifndef H_ARTIFICIAL_INTELLIGENCE_H
#define H_ARTIFICIAL_INTELLIGENCE_H

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
// Utility functions
/** Randomly returns 0 or 1.
 * @return 0 or 1.
 */
unsigned char ArtificialIntelligenceRandomBinaryChoice(void);

/** Turn to the specified direction with a random angle (from 45 degrees to 180 degrees).
 * @param Is_Turning_Left Set to 1 to turn left or to 0 to turn right.
 */
void ArtificialIntelligenceRandomAngleTurn(unsigned char Is_Turning_Left);

// Artificial intelligence algorithms
/** The robot avoids obstacles by turning to a random direction and goes nearer to the obstacles until it comes too close and must go backward.
 * The robot seems to gain trust until it comes too close. Then, it becomes again fearful and does not hang over the obstacles. 
 * The led is lighted in red when the robot is scared and lighted in green when it is self-confident.
 * The robot can randomly change direction even if no obstacle is signaled to seem more "alive".
 */
void ArtificialIntelligenceAvoidObjects(void);

#endif