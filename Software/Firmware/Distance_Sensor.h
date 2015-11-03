/** @file Distance_Sensor.h
 * Measure the distance between the robot and the nearest object in front of it.
 * @author Adrien RICCIARDI
 */
#ifndef H_DISTANCE_SENSOR_H
#define H_DISTANCE_SENSOR_H

//--------------------------------------------------------------------------------------------------
// Constants
//--------------------------------------------------------------------------------------------------
/** Convert a value from centimeters to the distance sensor unit.
 * @param Value The value to convert.
 */
#define DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(Value) (Value * 58)

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the timer required to measure the time elapsed between the ultrasonic burst emission and reception. */
void DistanceSensorInitialize(void);

/** Return the last sampled distance value.
 * @return The distance to the nearest object in sensor units (must divide by 58 to convert to cm). */
unsigned short DistanceSensorGetLastSampledDistance(void);

/** Sample the distance with the nearest object. The maximum sampling time is 38ms.
 * @return The distance to the nearest object in sensor units (must divide by 58 to convert to cm). */
unsigned short DistanceSensorGetDistance(void);

/** Called on RB1 pin state change. */
void DistanceSensorInterruptHandler(void);

/** Called when the trigger pin hold timer times out. */
void DistanceSensorTriggerPinInterruptHandler(void);

#endif