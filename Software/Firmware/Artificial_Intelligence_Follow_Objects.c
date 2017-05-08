/** @file Artificial_Intelligence_Follow_Objects.c
 * @see Artificial_Intelligence.h for description.
 * @author Adrien RICCIARDI
 */
#include "Artificial_Intelligence.h"
#include "Distance_Sensor.h"
#include "Led.h"
#include "Motor.h"
#include "Shared_Timer.h"

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** Robot escapes when an object comes too close. */
#define ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_DISTANCE_ESCAPING 20
/** The distance at which the robot starts following an object (in centimeters). */
#define ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_DISTANCE_START_FOLLOWING 80
/** The distance at which the robot stops going close to an object (in centimeters). */
#define ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_DISTANCE_STOP_FOLLOWING 30

/** The shared timer used to measure the elapsed time since the last object was detected. */
#define ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_DETECTION 0
/** How many time until the timer overflows (in 100ms unit). */
#define ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_VALUE_OBJECT_DETECTION 100
/** The shared timer used to measure the rotation time. */
#define ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_SEARCH 1

//-------------------------------------------------------------------------------------------------
// Private types
//-------------------------------------------------------------------------------------------------
typedef enum
{
	ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_WAIT_FOR_OBJECT,
	ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_FOLLOW_OBJECT,
	ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_SEARCH_OBJECT_ON_LEFT,
	ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_SEARCH_OBJECT_ON_RIGHT,
	ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_ESCAPE,
} TArtificialIntelligenceFollowObjectsState;

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
void ArtificialIntelligenceFollowObjects(void)
{
	unsigned short Distance = 0;
	TArtificialIntelligenceFollowObjectsState State = ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_WAIT_FOR_OBJECT;
	unsigned char Is_Object_Moving_To_Left = 0;
	
	while (1)
	{
		// Make the robot react in 20ms (it is useless to react too fast because motors and distance sensor are far more slower than the microcontroller running frequency)
		delay_ms(20);
	
		// Get the distance from the nearest object
		Distance = DISTANCE_SENSOR_CONVERT_SENSOR_UNIT_TO_CENTIMETERS(DistanceSensorGetLastSampledDistance());
			
		// Escape if the object comes too close
		if (Distance <= ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_DISTANCE_ESCAPING)
		{
			LedOnRed();
		
			// Go rear
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_BACKWARD);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_BACKWARD);
			delay_s(3);
			
			// Do a 180 degrees turn
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
			delay_s(2);
			// Then stop motors so next behavior find them stopped
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
			
			// TODO call avoid objects AI ?
			LedOff();
			return;
		}
		
		// TODO Change behavior if no object was detected for a long time
		/*if (SharedTimerIsTimerStopped(ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_DETECTION))
		{
			LedOff();
			
			// Stop motors so next behavior find them stopped
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
			return;
		}*/
		
		// Decide what to do next
		switch (State)
		{
			case ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_WAIT_FOR_OBJECT:
				// Start the object detection timer (behavior changes when this timer overflows, it is reset every time an object has been found)
				SharedTimerStartTimer(ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_DETECTION, ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_VALUE_OBJECT_DETECTION);
			
				// Is there an object at sight ?
				if (Distance <= ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_DISTANCE_START_FOLLOWING)
				{
					LedOnGreen();
					
					// Reset timer as an object was detected
					SharedTimerStartTimer(ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_DETECTION, ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_VALUE_OBJECT_DETECTION);
					
					State = ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_FOLLOW_OBJECT;
				}
				break;
			
			// An object is at sight, go close to it (but not too close)	
			case ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_FOLLOW_OBJECT:
				if (Distance <= ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_DISTANCE_START_FOLLOWING)
				{
					LedOnGreen();
					
					// Move close to the object only if the robot is not too close yet
					if (Distance > ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_DISTANCE_STOP_FOLLOWING)
					{
						MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
						MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
					}
					else
					{
						MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
						MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
					}
					
					// Reset timer as an object was detected
					SharedTimerStartTimer(ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_DETECTION, ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_VALUE_OBJECT_DETECTION);
				}
				// Object is lost
				else
				{
					LedOnRed();
				
					MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
					MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
					
					// Start searching in the last direction the object moved to
					if (Is_Object_Moving_To_Left) State = ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_SEARCH_OBJECT_ON_LEFT;
					else State = ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_SEARCH_OBJECT_ON_RIGHT;
					
					// Start a timer that will make the robot turn to the other direction on overflow
					SharedTimerStartTimer(ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_SEARCH, 10); // Robot needs 1s to turn 90°
				}
				break;
				
			case ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_SEARCH_OBJECT_ON_LEFT:
				// Go left until the object is found or a 90° turn has been made
				if (Distance <= ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_DISTANCE_START_FOLLOWING)
				{
					LedOnGreen();
				
					MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
					MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
					
					// Reset timer as an object was detected
					SharedTimerStartTimer(ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_DETECTION, ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_VALUE_OBJECT_DETECTION);
					
					Is_Object_Moving_To_Left = 1;
					State = ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_FOLLOW_OBJECT;
				}
				// Object is out of sight
				else
				{
					// Robot has finished turning
					if (SharedTimerIsTimerStopped(ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_SEARCH))
					{
						MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
						MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
					
						// Search the object on the right
						SharedTimerStartTimer(ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_SEARCH, 20); // Robot needs 2s to turn 180° (90° turned yet on the left + 90° on the right)
					
						State = ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_SEARCH_OBJECT_ON_RIGHT;
					}
					else
					{
						LedOnRed();
						MotorSetState(MOTOR_LEFT, MOTOR_STATE_BACKWARD);
						MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
					}
				}
				break;
				
			case ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_SEARCH_OBJECT_ON_RIGHT:
				// Go right until the object is found or a 90° turn has been made
				if (Distance <= ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_DISTANCE_START_FOLLOWING)
				{
					LedOnGreen();
				
					MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
					MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
					
					// Reset timer as an object was detected
					SharedTimerStartTimer(ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_DETECTION, ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_VALUE_OBJECT_DETECTION);
					
					Is_Object_Moving_To_Left = 0;
					State = ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_FOLLOW_OBJECT;
				}
				// Object is out of sight
				else
				{
					// Robot has finished turning
					if (SharedTimerIsTimerStopped(ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_SEARCH))
					{
						MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
						MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
					
						// Search the object on the left
						SharedTimerStartTimer(ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_TIMER_INDEX_OBJECT_SEARCH, 20); // Robot needs 2s to turn 180° (90° turned yet on the right + 90° on the left)
					
						State = ARTIFICIAL_INTELLIGENCE_FOLLOW_OBJECTS_STATE_SEARCH_OBJECT_ON_LEFT;
					}
					else
					{
						LedOnRed();
						MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
						MotorSetState(MOTOR_RIGHT, MOTOR_STATE_BACKWARD);
					}
				}
				break;
		}
	}
}