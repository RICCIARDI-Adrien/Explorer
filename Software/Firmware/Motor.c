/** @file Motor.c
 * @see Motor.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Motor.h"

//--------------------------------------------------------------------------------------------------
// Private constants and macros
//--------------------------------------------------------------------------------------------------
// PWM pins
#define MOTOR_LEFT_PIN 2
#define MOTOR_RIGHT_PIN 1

// The motors duty cycles
#define MOTOR_LEFT_FORWARD_DUTY_CYCLE (MOTOR_MAXIMUM_DUTY_CYCLE_VALUE - MOTOR_MINIMUM_DUTY_CYCLE_VALUE + 2000)
#define MOTOR_LEFT_BACKWARD_DUTY_CYCLE (MOTOR_MAXIMUM_DUTY_CYCLE_VALUE - MOTOR_MINIMUM_DUTY_CYCLE_VALUE + 4000)
#define MOTOR_RIGHT_FORWARD_DUTY_CYCLE (MOTOR_MAXIMUM_DUTY_CYCLE_VALUE - MOTOR_MINIMUM_DUTY_CYCLE_VALUE + 4000)
#define MOTOR_RIGHT_BACKWARD_DUTY_CYCLE (MOTOR_MAXIMUM_DUTY_CYCLE_VALUE - MOTOR_MINIMUM_DUTY_CYCLE_VALUE + 2000)

// PWM hardware limits
/** The 0% duty cycle value. */
#define MOTOR_MINIMUM_DUTY_CYCLE_VALUE 40000 // = (Fosc/4)/(prescaler*50Hz)
/** The 100% duty cycle value. */
#define MOTOR_MAXIMUM_DUTY_CYCLE_VALUE 65535
/** Make the timer 1 overflows each 50Hz. */
#define MOTOR_PWM_TIMER_RELOAD_VALUE (MOTOR_MAXIMUM_DUTY_CYCLE_VALUE - MOTOR_MINIMUM_DUTY_CYCLE_VALUE)
/** Enable the timer 1. */
#define MOTOR_PWM_TIMER_START() t1con.TMR1ON = 1
/** Disable the timer 1. */
#define MOTOR_PWM_TIMER_STOP() t1con.TMR1ON = 0

/** How many motors to handle. */
#define MOTORS_COUNT 2

//--------------------------------------------------------------------------------------------------
// Private variables
//--------------------------------------------------------------------------------------------------
/** Each motor current duty cycle. */
static unsigned short Motor_Current_Duty_Cycle[MOTORS_COUNT];

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void MotorInitialize(void)
{
	// Configure the pins as digital
	anselc.MOTOR_LEFT_PIN = 0;
	anselc.MOTOR_RIGHT_PIN = 0;
	// Configure PWM pins as outputs
	trisc.MOTOR_LEFT_PIN = 0;
	trisc.MOTOR_RIGHT_PIN = 0;
	
	// Stop motors
	portc.MOTOR_LEFT_PIN = 0;
	portc.MOTOR_RIGHT_PIN = 0;
	
	// Configure the CCP modules in Compare mode
	ccp1con = 0x0A;
	ccp2con = 0x0A;
	ccptmrs0 = 0; // Use timer 1 as clock source for both compare modules
	ipr1.CCP1IP = 0; // Set both interrupts as low priority
	ipr2.CCP2IP = 0;
	
	// Configure the timer 1 to generate a 50Hz frequency
	tmr1h = MOTOR_PWM_TIMER_RELOAD_VALUE >> 8;
	tmr1l = (unsigned char) MOTOR_PWM_TIMER_RELOAD_VALUE;
	t1con = 0x33; // Use Fosc/4 as clock source, use a 8x prescaler, disable the dedicated secondary oscillator circuit, access to the timer registers in one 16-bit operation, enable the timer
	ipr1.TMR1IP = 0; // Set the timer 1 as a low priority interrupt
	pie1.TMR1IE = 1; // Enable the timer 1 interrupt	
}

void MotorInterruptHandler(void)
{
	MOTOR_PWM_TIMER_STOP();

	// Handle timer 1 overflow : the PWM period is over
	if (pir1.TMR1IF)
	{
		// Update the left motor duty cycle
		// Schedule an interrupt if the left motor duty cycle is in range ]0;100%[
		if ((Motor_Current_Duty_Cycle[MOTOR_LEFT] > 0) && (Motor_Current_Duty_Cycle[MOTOR_LEFT] < MOTOR_MAXIMUM_DUTY_CYCLE_VALUE))
		{
			// Update the left motor PWM duty cycle
			ccpr1h = Motor_Current_Duty_Cycle[MOTOR_LEFT] >> 8;
			ccpr1l = (unsigned char) Motor_Current_Duty_Cycle[MOTOR_LEFT];
			pie1.CCP1IE = 1;
		}
		else pie1.CCP1IE = 0;
		// Set the pin state
		if (Motor_Current_Duty_Cycle[MOTOR_LEFT] == 0) portc.MOTOR_LEFT_PIN = 0;
		else portc.MOTOR_LEFT_PIN = 1;
		
		// Update the right motor duty cycle
		// Schedule an interrupt if the right motor duty cycle is in range ]0;100%[
		if ((Motor_Current_Duty_Cycle[MOTOR_RIGHT] > 0) && (Motor_Current_Duty_Cycle[MOTOR_RIGHT] < MOTOR_MAXIMUM_DUTY_CYCLE_VALUE))
		{
			// Update the right motor PWM duty cycle
			ccpr2h = Motor_Current_Duty_Cycle[MOTOR_RIGHT] >> 8;
			ccpr2l = (unsigned char) Motor_Current_Duty_Cycle[MOTOR_RIGHT];
			pie2.CCP2IE = 1;
		}
		else pie2.CCP2IE = 0;
		// Set the pin state
		if (Motor_Current_Duty_Cycle[MOTOR_RIGHT] == 0) portc.MOTOR_RIGHT_PIN = 0;
		else portc.MOTOR_RIGHT_PIN = 1;
	
		// Reload the timer
		tmr1h = MOTOR_PWM_TIMER_RELOAD_VALUE >> 8;
		tmr1l = (unsigned char) MOTOR_PWM_TIMER_RELOAD_VALUE;
		
		// Reset the interrupt flag
		pir1.TMR1IF = 0;
	}
	
	// Handle the CCP1 interrupt : the left motor pin must change state
	if ((pie1.CCP1IE) && (pir1.CCP1IF))
	{
		portc.MOTOR_LEFT_PIN = 0;
		
		// Reset interrupt flag
		pir1.CCP1IF = 0;
	}
	
	// Handle the CCP2 interrupt : the right motor pin must change state
	if ((pie2.CCP2IE) && (pir2.CCP2IF))
	{
		portc.MOTOR_RIGHT_PIN = 0;
		
		// Reset interrupt flag
		pir2.CCP2IF = 0;
	}
	
	MOTOR_PWM_TIMER_START();
}

void MotorSetState(TMotor Motor, TMotorState State)
{
	MOTOR_PWM_TIMER_STOP();
	
	// Choose the right duty cycle according to selected state
	switch (State)
	{
		case MOTOR_STATE_STOPPED:
			Motor_Current_Duty_Cycle[Motor] = 0;
			break;
			
		case MOTOR_STATE_FORWARD:
			if (Motor == MOTOR_LEFT) Motor_Current_Duty_Cycle[MOTOR_LEFT] = MOTOR_LEFT_FORWARD_DUTY_CYCLE;
			else Motor_Current_Duty_Cycle[MOTOR_RIGHT] = MOTOR_RIGHT_FORWARD_DUTY_CYCLE;
			break;
			
		case MOTOR_STATE_BACKWARD:
			if (Motor == MOTOR_LEFT) Motor_Current_Duty_Cycle[MOTOR_LEFT] = MOTOR_LEFT_BACKWARD_DUTY_CYCLE;
			else Motor_Current_Duty_Cycle[MOTOR_RIGHT] = MOTOR_RIGHT_BACKWARD_DUTY_CYCLE;
			break;
	}
		
	MOTOR_PWM_TIMER_START();
}
