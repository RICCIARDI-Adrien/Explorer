/** @file Distance_Sensor.h
 * Measure the distance between the robot and the nearest object in front of it.
 * @author Adrien RICCIARDI
 */
#ifndef H_DISTANCE_SENSOR_H
#define H_DISTANCE_SENSOR_H

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the timer required to measure the time elapsed between the ultrasonic burst emission and reception. */
void DistanceSensorInitialize(void);

/** Start a distance measure. */
void DistanceSensorScheduleDistanceSampling(void);

/** Return the last sampled distance value.
 * @return The distance to the nearest object in centimeters. */
unsigned short DistanceSensorGetLastSampledDistance(void);

/** Called on RB1 pin state change. */
void DistanceSensorInterruptHandler(void);

#endif