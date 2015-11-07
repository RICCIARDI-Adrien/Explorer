/** @file Artificial_Intelligence.c
 * @see Artificial_Intelligence.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Artificial_Intelligence.h"
#include "Distance_Sensor.h"
#include "Led.h"
#include "Motor.h"
#include "Random.h"

//--------------------------------------------------------------------------------------------------
// Private functions
//--------------------------------------------------------------------------------------------------
/** Accumulate distance samples for 400ms and return the average.
 * @return The average distance.
 */
inline unsigned short ArtificialIntelligenceSampleDistance(void)
{
	unsigned long Distance = 0;
	unsigned char i;
	
	// Get several samples
	for (i = 0; i < 4; i++) Distance += DistanceSensorGetDistance();
		
	// Compute the average
	Distance >>= 2; // Fast division per 4
	
	return (unsigned short) Distance;
}

/** Randomly returns 0 or 1.
 * @return 0 or 1.
 */
inline unsigned char ArtificialIntelligenceRandomBinaryChoice(void)
{
	if (RandomGetNumber() % 2) return 0;
	return 1;
}

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
/*void ArtificialIntelligenceSearchForGreatOutdoors(void)
{
	unsigned short Distance;
	
	while (1)
	{
		Distance = ArtificialIntelligenceSampleDistance();
		
		
	}
}*/

void ArtificialIntelligenceAvoidObjectsDeterministTurnDirection(void)
{
	unsigned short Distance;
	
	MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
	MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
	
	while (1)
	{
		Distance = ArtificialIntelligenceSampleDistance();
		
		// Go backward if the obstacle is too close
		if (Distance < DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(15))
		{
			LedOnRed();
			
			// Stop motors
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
			delay_s(1);	// Wait 1 second for the motors inductive current to dissipate (or it will generate a short circuit)
			
			// Go straight backward for some time
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_BACKWARD);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_BACKWARD);
			delay_s(3);
			
			// Quickly turn left in backward mode
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_BACKWARD);
			delay_s(1);
		}
		// Turn 90° right if an obstacle was detected
		else if (Distance < DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(40))
		{
			LedOnRed();
			
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_BACKWARD);
			delay_s(1);
		}
		// Go straight if nothing at sight
		else
		{
			LedOnGreen();
			
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
		}
	}
}

void ArtificialIntelligenceAvoidObjectsRandomTurnDirection(void)
{
	unsigned short Distance;
	
	MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
	MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
	
	while (1)
	{
		Distance = ArtificialIntelligenceSampleDistance();
		
		// Go backward if the obstacle is too close
		if (Distance < DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(15))
		{
			LedOnRed();
			
			// Stop motors
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
			delay_s(1);	// Wait 1 second for the motors inductive current to dissipate (or it will generate a short circuit)
			
			// Go straight backward for some time
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_BACKWARD);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_BACKWARD);
			delay_s(3);
			
			// Quickly turn in a random direction in backward mode
			if (ArtificialIntelligenceRandomBinaryChoice())
			{
				// Turn left
				MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
				MotorSetState(MOTOR_RIGHT, MOTOR_STATE_BACKWARD);
			}
			else
			{
				// Turn right
				MotorSetState(MOTOR_LEFT, MOTOR_STATE_BACKWARD);
				MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
			}
			delay_s(1);
		}
		// Turn 90° if an obstacle was detected
		else if (Distance < DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(40))
		{
			LedOnRed();
			
			if (ArtificialIntelligenceRandomBinaryChoice())
			{
				// Turn right
				MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
				MotorSetState(MOTOR_RIGHT, MOTOR_STATE_BACKWARD);
			}
			else
			{
				// Turn left
				MotorSetState(MOTOR_LEFT, MOTOR_STATE_BACKWARD);
				MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
			}
			delay_s(1);
		}
		// Go straight if nothing at sight
		else
		{
			LedOnGreen();
			
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
		}
	}
}