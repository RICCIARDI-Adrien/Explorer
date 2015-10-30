/** @file Distance_Sensor.c
 * @see Distance_Sensor.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Distance_Sensor.h"

//--------------------------------------------------------------------------------------------------
// Private constants and macros
//--------------------------------------------------------------------------------------------------
/** The "trigger" pin. */
#define DISTANCE_SENSOR_TRIGGER_PIN 0 // RB0
/** The "echo" pin. */
#define DISTANCE_SENSOR_ECHO_PIN 1 // RB1

/** Start the counter. */
#define DISTANCE_SENSOR_COUNTER_TIMER_START() t0con.TMR0ON = 1
/** Stop the counter. */
#define DISTANCE_SENSOR_COUNTER_TIMER_STOP() t0con.TMR0ON = 0
/** Reset the counter. */
#define DISTANCE_SENSOR_COUNTER_TIMER_RESET() \
{ \
	tmr0h = 0; \
	tmr0l = 0; \
}

/** Enable the INT1 interrupt. */
#define DISTANCE_SENSOR_EXTERNAL_INTERRUPT_ENABLE() intcon3.INT1IE = 1
/** Disable the INT1 interrupt. */
#define DISTANCE_SENSOR_EXTERNAL_INTERRUPT_DISABLE() intcon3.INT1IE = 0

//--------------------------------------------------------------------------------------------------
// Private variables
//--------------------------------------------------------------------------------------------------
/** The distance to the nearest object in centimeters. */
static unsigned short Distance_Sensor_Last_Measured_Distance = 0; // Do not allow the motors to move until a real measure has been done

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void DistanceSensorInitialize(void)
{
	// Configure the pins as digital
	anselb.DISTANCE_SENSOR_TRIGGER_PIN = 0;
	anselb.DISTANCE_SENSOR_ECHO_PIN = 0;
	
	// Configure the pins direction
	latb.DISTANCE_SENSOR_TRIGGER_PIN = 0; // Avoid triggering a spurious measure
	trisb.DISTANCE_SENSOR_TRIGGER_PIN = 0; // Set as output
	trisb.DISTANCE_SENSOR_ECHO_PIN = 1; // Set as input
	
	// Configure the timer 0 to increment every microsecond
	t0con = 0x03; // Enable the timer in 16-bit mode, use Fosc/4 as clock source, use a 1:16 prescaler, do not start the timer
	DISTANCE_SENSOR_COUNTER_TIMER_RESET();
	
	// Configure the external interrupt 1
	intcon2.INTEDG1 = 1; // Trigger an interrupt when a rising edge is detected
	intcon3.INT1IP = 1; // Set the interrupt as high priority
	DISTANCE_SENSOR_EXTERNAL_INTERRUPT_ENABLE();
	
	// Configure the timer 2 (this timer releases the trigger pin without using a busy loop in the interrupt handler)
	pr2 = 20; // The timer period is 1us, the recommended trigger pin hold time is 10us, so use 20us for increased safety
	ipr1.TMR2IP = 0; // Set the interrupt as low priority
	pie1.TMR2IE = 1; // Enable the interrupt
}

void DistanceSensorScheduleDistanceSampling(void)
{
	// Trigger a distance measure
	latb.DISTANCE_SENSOR_TRIGGER_PIN = 1;

	// Configure timer 2 to trigger an interrupt 20us later
	tmr2 = 0;
	t2con = 0x06; // Do not use a postscaler, enable the timer, set a 16x prescaler to get a timer frequency of 1MHz (so it is easy to count microseconds)
}

unsigned short DistanceSensorGetLastSampledDistance(void)
{
	unsigned short Distance;
	
	// Atomically access to the shared variable
	DISTANCE_SENSOR_EXTERNAL_INTERRUPT_DISABLE();
	Distance = Distance_Sensor_Last_Measured_Distance;
	DISTANCE_SENSOR_EXTERNAL_INTERRUPT_ENABLE();
	
	return Distance;
}

void DistanceSensorInterruptHandler(void)
{
	static unsigned char Is_Waiting_For_Rising_Edge = 1; // The echo signal starts with a rising edge
	
	// A rising edge has been detected
	if (Is_Waiting_For_Rising_Edge)
	{
		DISTANCE_SENSOR_COUNTER_TIMER_START(); // Start counting
		intcon2.INTEDG1 = 0; // Configure the external interrupt to trigger an interrupt when a falling edge is detected
		Is_Waiting_For_Rising_Edge = 0;
	}
	// A falling edge has been detected
	else
	{
		DISTANCE_SENSOR_COUNTER_TIMER_STOP(); // Stop counting
		intcon2.INTEDG1 = 1; // Configure the external interrupt to trigger an interrupt when a rising edge is detected
		Is_Waiting_For_Rising_Edge = 1;
		
		// Convert the timing to a distance
		Distance_Sensor_Last_Measured_Distance = tmr0l; // TMR0L must be read before TMR0H to grant a valid result
		Distance_Sensor_Last_Measured_Distance |= tmr0h << 8;
		DISTANCE_SENSOR_COUNTER_TIMER_RESET();
	}
	
	// Clear the interrupt flag
	intcon3.INT1IF = 0;
}

void DistanceSensorTriggerPinInterruptHandler(void)
{
	// Stop the timer
	t2con.TMR2ON = 0;
	
	// Release the trigger pin
	latb.DISTANCE_SENSOR_TRIGGER_PIN = 0;
	
	// Clear the interrupt flag
	pir1.TMR2IF = 0;
}
