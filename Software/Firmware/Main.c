/** @file Main.c
 * Explorer robot artificial intelligence entry point and main loop.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "ADC.h"
#include "Distance_Sensor.h"
#include "Led.h"
#include "Motor.h"
#include "Shared_Timer.h"
#include "UART.h"

//--------------------------------------------------------------------------------------------------
// Microcontroller configuration
//--------------------------------------------------------------------------------------------------
#pragma DATA _CONFIG1H, _IESO_OFF_1H & _FCMEN_OFF_1H & _PRICLKEN_ON_1H & _PLLCFG_ON_1H & _FOSC_INTIO67_1H // Disable oscillator switchover mode, disable fail-safe clock monitor, always enable primary clock, always enable PLL, select the internal oscillator block
#pragma DATA _CONFIG2L, _BORV_285_2L & _BOREN_SBORDIS_2L & _PWRTEN_ON_2L // Use the highest Brown-out reset voltage level, enable the Brown-out reset in hardware only, enable the Power-up timer
#pragma DATA _CONFIG2H, _WDTEN_OFF_2H // Disable the watchdog timer in hardware
#pragma DATA _CONFIG3H, _MCLRE_INTMCLR_3H & _HFOFST_OFF_3H & _PBADEN_OFF_3H & _CCP2MX_PORTC1_3H // Disable /MCLR, wait for the system clock to stabilize before starting the core, PORTB pins are configured as digital I/O, CCP2 is multiplexed with RC1
#pragma DATA _CONFIG4L, _DEBUG_OFF_4L & _XINST_OFF_4L & _LVP_OFF_4L & _STVREN_ON_4L // Disable the background debugger, disable the extended instruction set, disable the Single-supply ICSP, reset the MCU if a stack overflow/underflow occurs
#pragma DATA _CONFIG5L, _CP3_OFF_5L & _CP2_OFF_5L & _CP1_OFF_5L & _CP0_OFF_5L // Disable all code protections
#pragma DATA _CONFIG5H, _CPD_OFF_5H & _CPB_OFF_5H // Disable the data EEPROM and the Boot block code protections
#pragma DATA _CONFIG6L, _WRT3_OFF_6L & _WRT2_OFF_6L & _WRT1_OFF_6L & _WRT0_OFF_6L // Disable all write protections
#pragma DATA _CONFIG6H, _WRTD_OFF_6H & _WRTB_OFF_6H & _WRTC_OFF_6H // Disable data EEPROM, Boot block and configuration registers write protection
#pragma DATA _CONFIG7L, _EBTR3_OFF_7L & _EBTR2_OFF_7L & _EBTR1_OFF_7L & _EBTR0_OFF_7L // Disable all table read protections
#pragma DATA _CONFIG7H, _EBTRB_OFF_7H // Disable Boot block table read protection

// Configure clock frequency
#pragma CLOCK_FREQ 64000000

//--------------------------------------------------------------------------------------------------
// Entry point
//--------------------------------------------------------------------------------------------------
void main(void)
{
	// Set core clock to 64MHz
	osctune = 0x40; // Enable the 4x PLL
	osccon2 = 0; // Disable the secondary clock, disable the primary clock external oscillator circuit
	osccon = 0x78; // Set internal oscillator block frequency to 16MHz, use the clock defined by FOSC bits, use primary clock as core clock
	
	// Initialize the peripherals
	UARTInitialize();
	ADCInitialize();
	DistanceSensorInitialize();
	MotorInitialize();
	SharedTimerInitialize();
	LedInitialize();
	
	// Enable the interrupts
	rcon.IPEN = 1; // Enable interrupt priority
	intcon |= 0xC0; // Enable all high priority and all low priority interrupts
	
	// Stop the motors
	MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
	MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
	
	// System is ready
	LedOnGreen();
	
	// TEST
	while (1)
	{
		latb.5 = !portb.5;
		latb.4 = !portb.4;
		
		delay_ms(200);
		delay_ms(200);
	}
}