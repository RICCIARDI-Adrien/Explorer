/** @file Artificial_Intelligence.c
 * @see Artificial_Intelligence.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Artificial_Intelligence.h"
#include "Distance_Sensor.h"
#include "Motor.h"
#include "Random.h"

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
unsigned char ArtificialIntelligenceRandomBinaryChoice(void)
{
	if (RandomGetNumber() < 128) return 0; // Do not use modulo operator to be faster
	return 1;
}

void ArtificialIntelligenceRandomAngleTurn(unsigned char Is_Turning_Left)
{
	if (Is_Turning_Left)
	{
		// Turn left
		MotorSetState(MOTOR_LEFT, MOTOR_STATE_BACKWARD);
		MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
	}
	else
	{
		// Turn right
		MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
		MotorSetState(MOTOR_RIGHT, MOTOR_STATE_BACKWARD);
	}
	
	// Choose the turning time, which determine the turning angle, from 500ms (45°) to 2s (180°)
	switch (RandomGetNumber() % 4)
	{
		// 45°
		case 0:
			delay_ms(250);
			delay_ms(250);
			break;
		
		// 90°
		case 1:
			delay_s(1);
			break;
		
		// 135°
		case 2:
			delay_s(1);
			delay_ms(250);
			delay_ms(250);
			break;
		
		// 180°	
		default:
			delay_s(2);
			break;
	}
}
