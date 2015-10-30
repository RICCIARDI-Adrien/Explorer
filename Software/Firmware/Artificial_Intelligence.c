/** @file Artificial_Intelligence.c
 * @see Artificial_Intelligence.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Artificial_Intelligence.h"
#include "Distance_Sensor.h"
#include "Led.h"
#include "Motor.h"

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
	
	// Get 4 samples
	for (i = 0; i < 4; i++)
	{
		Distance += DistanceSensorGetLastSampledDistance();
		delay_ms(100);
	}
	
	// Compute the average
	Distance >>= 2; // Fast division per 4
	
	return (unsigned short) Distance;
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

void ArtificialIntelligenceAvoidObjects(void)
{
	unsigned short Distance;
	
	MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
	MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
	
	while (1)
	{
		Distance = ArtificialIntelligenceSampleDistance();
		
		// Turn 90� right if an obstacle was detected
		if (Distance < DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(40))
		{
			LedOnRed();
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
			delay_s(1);
		}
		// Go straight if nothing at sight
		else
		{
			LedOnGreen();
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
		}
	}
}