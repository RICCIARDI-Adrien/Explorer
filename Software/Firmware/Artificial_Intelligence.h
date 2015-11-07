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

#endif