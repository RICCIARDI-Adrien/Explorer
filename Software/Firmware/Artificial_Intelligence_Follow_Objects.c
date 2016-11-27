/** @file Artificial_Intelligence_Follow_Objects.c
 * @see Artificial_Intelligence.h for description.
 * @author Adrien RICCIARDI
 */

void ArtificialIntelligenceFollowObjects(void)
{
	unsigned short Distance;
	unsigned char Is_Target_Found = 0;
	
	LedOnRed();
	MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
	MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
	
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
			delay_ms(250); // Wait some time for the motors inductive current to dissipate (or it will generate a short circuit)
			delay_ms(250);
			
			// Go straight backward for some time
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_BACKWARD);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_BACKWARD);
			delay_s(3);
		}
		// Stop if the object is close enougth
		else if (Distance < DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(20))
		{
			LedOnRed();
			
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
		}
		// Follow the object if it not too far
		else if (Distance < DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(50))
		{
			LedOnGreen();
			
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_FORWARD);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
			Is_Target_Found = 1;
		}
		// Stop if there is nothing at sight
		else if (Distance >= DISTANCE_SENSOR_CONVERT_CENTIMETERS_TO_SENSOR_UNIT(100))
		{
			LedOnRed();
			
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
			Is_Target_Found = 0;
		}
		// The target has been lost, try to find it by looking on sides
		else if (Is_Target_Found)
		{
			// Go left some times
			MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
			MotorSetState(MOTOR_RIGHT, MOTOR_STATE_FORWARD);
			
			
		}
	}
}