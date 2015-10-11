/** @file Motor.h
 * Use CCP modules in compare mode to control the servomotors.
 * @author Adrien RICCIARDI
 */
#ifndef H_MOTOR_H
#define H_MOTOR_H

//--------------------------------------------------------------------------------------------------
// Types
//--------------------------------------------------------------------------------------------------
/** Identify the motor. */
typedef enum 
{
	MOTOR_LEFT,
	MOTOR_RIGHT
} TMotor;

/** Identify the state of a motor. */
typedef enum
{
	MOTOR_STATE_STOPPED,
	MOTOR_STATE_FORWARD,
	MOTOR_STATE_BACKWARD
} TMotorState;

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the PWM modules and the timer used to control the motors. */
void MotorInitialize(void);

/** Handle all motor-related interrupts. */
void MotorInterruptHandler(void);

/** Set the state of a motor (running forward, running backward or stopped).
 * @param Motor The motor to command.
 * @param State The new motor state.
 */
void MotorSetState(TMotor Motor, TMotorState State);

#endif