/** @file Artificial_Intelligence_Avoid_Objects.c
 * @see Artificial_Intelligence.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Artificial_Intelligence.h"
#include "Distance_Sensor.h"
#include "Led.h"
#include "Motor.h"
#include "Random.h"
#include "Shared_Timer.h"

//--------------------------------------------------------------------------------------------------
// Private constants
//--------------------------------------------------------------------------------------------------
/** The obstacle detection distance used when the robot is scared (cm). */
#define ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_DEFAULT_OBSTACLE_DETECTION_DISTANCE 40
/** How many time (in ms * 100) before the robot comes nearer to an object (i.e. gain trust). */
#define ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_TRUST_TIMER_VALUE 50

/** How many time to wait while the robot is going straight for it to turn. */
#define ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_STRAIGHT_TIMER_VALUE 200
 
//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void ArtificialIntelligenceAvoidObjects(void)
{
	unsigned char Turn_Direction;
	unsigned short Distance, Obstacle_Detection_Distance = DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_DEFAULT_OBSTACLE_DETECTION_DISTANCE);
	
	MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
	MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
	
	// Start the "trust" timer
	SharedTimerStartTimer(1, ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_TRUST_TIMER_VALUE);
	// Start the "going straight" timer
	SharedTimerStartTimer(2, ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_STRAIGHT_TIMER_VALUE);
	
	while (1)
	{
		// Make the robot react in 20ms (it is useless to react too fast because motors and distance sensor are far more slower than the microcontroller running frequency)
		delay_ms(20);
	
		Distance = DistanceSensorGetLastSampledDistance();
		
		// Go backward if the obstacle is too close
		if (Distance < DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(15))
		{
			LedOnRed();
			
			// Stop motors
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
			delay_ms(250); // Wait some time for the motors inductive current to dissipate (or it will generate a short circuit)
			delay_ms(250);
			
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
			
			// Reset the object detection distance to farthest distance (the robot went too far and was scared, so it becomes fearful again)
			Obstacle_Detection_Distance = DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_DEFAULT_OBSTACLE_DETECTION_DISTANCE);
			SharedTimerStartTimer(1, ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_TRUST_TIMER_VALUE); // Reset the timer too (do that after the delay to avoid loosing 1 second due to the delay)
			
			// Reset the "going straight" timer
			SharedTimerStartTimer(2, ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_STRAIGHT_TIMER_VALUE);
		}
		// Turn from 45° (turning time is 500ms) to 180° (turning time is 2s) if an obstacle was detected
		else if (Distance < Obstacle_Detection_Distance)
		{
			LedOnRed();
		
			// Choose a new random direction if the previous one lasted long enough. By keeping the same direction for some time, the robot avoids turning left then right then left and so on when it is blocked until the random choice keeps a direction long enough
			if (SharedTimerIsTimerStopped(0))
			{
				Turn_Direction = ArtificialIntelligenceRandomBinaryChoice();
				SharedTimerStartTimer(0, 60); // 6s should be enough for most blocking situations, even if the robot does not turn 90° every time
			}
			
			ArtificialIntelligenceRandomAngleTurn(Turn_Direction);
			
			// Reset the "going straigh" timer
			SharedTimerStartTimer(2, ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_STRAIGHT_TIMER_VALUE);
		}
		// Go straight if nothing at sight
		else
		{
			LedOnGreen();
			
			// Turn in a random direction if the robot is going straight since several seconds
			if (SharedTimerIsTimerStopped(2))
			{
				// Turn left or right
				ArtificialIntelligenceRandomAngleTurn(ArtificialIntelligenceRandomBinaryChoice());
			
				// Restart the timer for the next straight line
				SharedTimerStartTimer(2, ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_STRAIGHT_TIMER_VALUE);
			}
			else
			{
				MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
				MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
			}
		}
		
		// Decrement the object detection distance if the robot did not went too far to an object for some time (it's like the robot is gaining trust)
		if (SharedTimerIsTimerStopped(1))
		{
			// Make the robot come closer to the obstacles
			if (DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(Obstacle_Detection_Distance) > DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(20)) Obstacle_Detection_Distance -= DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(2);
			
			// Restart the timer
			SharedTimerStartTimer(1, ARTIFICIAL_INTELLIGENCE_AVOID_OBJECTS_TRUST_TIMER_VALUE);
		}	
	}
}