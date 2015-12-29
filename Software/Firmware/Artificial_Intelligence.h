/** @file Artificial_Intelligence.h
 * Randomly between multiple behaviours.
 * @author Adrien RICCIARDI
 */
#ifndef H_ARTIFICIAL_INTELLIGENCE_H
#define H_ARTIFICIAL_INTELLIGENCE_H

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** The robot goes straight until it finds an obstacle, then turn right and continue. */
void ArtificialIntelligenceAvoidObjectsDeterministTurnDirection(void);

/** The robot goes straight until it finds an obstacle, then choose a random direction and turn this way. */
void ArtificialIntelligenceAvoidObjectsRandomTurnDirection(void);

/** The robot avoids obstacles by turning to a random direction (same as ArtificialIntelligenceAvoidObjectsRandomTurnDirection()) and goes nearer to the obstacles until it comes too close and must go backward.
 * The robot seems to gain trust until it comes too close. Then, it becomes again fearful and does not hang over the obstacles. 
 * The led is lighted in red when the robot is scared and lighted in green when it is self-confident.
 */
void ArtificialIntelligenceAvoidObjectsGainingTrust(void);

#endif