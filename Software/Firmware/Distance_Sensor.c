/** @file Distance_Sensor.c
 * @see Distance_Sensor.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Distance_Sensor.h"

//--------------------------------------------------------------------------------------------------
// Private constants
//--------------------------------------------------------------------------------------------------
/** The "trigger" pin. */
#define DISTANCE_SENSOR_TRIGGER_PIN 0 // RB0
/** The "echo" pin. */
#define DISTANCE_SENSOR_ECHO_PIN 1 // RB1

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void DistanceSensorInitialize(void)
{
	// Configure the pins direction
	trisb.DISTANCE_SENSOR_TRIGGER_PIN = 0; // Set as output
	trisb.DISTANCE_SENSOR_ECHO_PIN = 1; // Set as input
	// Configure the pins as digital
	anselb.DISTANCE_SENSOR_TRIGGER_PIN = 0;
	anselb.DISTANCE_SENSOR_ECHO_PIN = 0;
	
	
}
