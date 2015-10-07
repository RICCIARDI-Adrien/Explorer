/** @file Main.c
 * Allow to flash a new firmware without using a programmer.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Flash.h"
#include "Led.h"
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
// Constants and macros
//--------------------------------------------------------------------------------------------------
/** The firmware base address. */
#define MAIN_FIRMWARE_BASE_ADDRESS 0x400
/** The update flag location. */
#define MAIN_UPDATE_FLAG_ADDRESS 0xFFC0 //The PIC18F26K22 flash last block

/** The protocol magic number. */
#define MAIN_PROTOCOL_MAGIC_NUMBER 0xA5
/** The "start update" command. */
#define MAIN_PROTOCOL_COMMAND_START_FIRMWARE_UPDATE 2
/** The bootloader acknowledges that it has received and flashed a block. */
#define MAIN_PROTOCOL_ACKNOWLEDGE 0x42

//--------------------------------------------------------------------------------------------------
// Private functions
//--------------------------------------------------------------------------------------------------
/** High-priority interrupt redirection to the firmware code. */
static void MainHighPriorityInterruptRedirection(void) @ 0x08
{
	asm goto MAIN_FIRMWARE_BASE_ADDRESS + 0x08;
}

/** Low-priority interrupt redirection to the firmware code. */
static void MainLowPriorityInterruptRedirection(void) @ 0x18
{
	asm goto MAIN_FIRMWARE_BASE_ADDRESS + 0x18;
}

//--------------------------------------------------------------------------------------------------
// Entry point
//--------------------------------------------------------------------------------------------------
void main(void)
{
	unsigned char Is_Magic_Number_Received = 0, Byte, i, Block_Buffer[FLASH_BLOCK_SIZE], Bytes_To_Receive_Count;
	unsigned short Firmware_Size;
	unsigned long Block_Address = MAIN_FIRMWARE_BASE_ADDRESS;
	
	// Set core clock to 64MHz
	osctune = 0x40; // Enable the 4x PLL
	osccon2 = 0; // Disable the secondary clock, disable the primary clock external oscillator circuit
	osccon = 0x78; // Set internal oscillator block frequency to 16MHz, use the clock defined by FOSC bits, use primary clock as core clock
	
	// Is the update flag byte location erased ?
	if (FlashReadByte(MAIN_UPDATE_FLAG_ADDRESS) == 0xFF)
	{
		// Initialize the peripherals
		LedInitialize();
		UARTInitialize();
		
		LedOnGreen();
		
		// TODO stop the motors ?
		
		// Wait for the "start programming" command
		while (1)
		{
			Byte = UARTReadByte();
			
			// Wait for the magic number
			if (!Is_Magic_Number_Received && (Byte == MAIN_PROTOCOL_MAGIC_NUMBER)) Is_Magic_Number_Received = 1;
			// Wait for the command if the magic number was received
			else if (Is_Magic_Number_Received && (Byte == MAIN_PROTOCOL_COMMAND_START_FIRMWARE_UPDATE)) break;
			else Is_Magic_Number_Received = 0;
		}
		
		// Receive the firmware size
		Firmware_Size = (UARTReadByte() << 8) | UARTReadByte();
		
		// Receive the firmware data and flash it
		while (Firmware_Size > 0)
		{
			// Compute the amount of bytes to received
			if (Firmware_Size >= FLASH_BLOCK_SIZE) Bytes_To_Receive_Count = FLASH_BLOCK_SIZE;
			else Bytes_To_Receive_Count = Firmware_Size;
		
			// Receive up to a full block of data
			for (i = 0; i < Bytes_To_Receive_Count; i++) Block_Buffer[i] = UARTReadByte();
			
			// Flash the block
			FlashWriteBlock(Block_Address, Block_Buffer);
			Block_Address += FLASH_BLOCK_SIZE;
			
			// Send an acknowledge to the PC
			UARTWriteByte(MAIN_PROTOCOL_ACKNOWLEDGE);
			
			Firmware_Size -= Bytes_To_Receive_Count;
		}
		
		// Set the update flag
		Block_Buffer[0] = 0x42; // Everything but 0xFF
		FlashWriteBlock(MAIN_UPDATE_FLAG_ADDRESS, Block_Buffer);
			
		// Reboot the microcontroller
		asm reset;
	}
	// Boot the firmware
	else asm goto MAIN_FIRMWARE_BASE_ADDRESS;
	
	// Safety infinite loop
	while (1);
}