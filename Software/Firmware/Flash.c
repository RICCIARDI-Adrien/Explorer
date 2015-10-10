/** @file Flash.c
 * @see Flash.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Flash.h"

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void FlashEraseBlock(unsigned long Block_Address)
{
	// Load the block starting address
	tblptru = Block_Address >> 16;
	tblptrh = Block_Address >> 8;
	tblptrl = (unsigned char) Block_Address;
	
	// Configure an erase cycle
	eecon1 = 0x94; // Access flash program memory, enable block erasing on next write cycle, allow writing to the program memory
	
	// Execute the special write sequence
	eecon2 = 0x55;
	eecon2 = 0xAA;
	
	// Start the erase cycle (the core will stall until the erase cycle is finished)
	eecon1.WR =1;
}