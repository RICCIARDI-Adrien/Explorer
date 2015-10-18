/** @file Flash.c
 * @see Flash.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Flash.h"

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void FlashWriteBlock(unsigned long Block_Address, unsigned char *Pointer_Data_Buffer)
{
	unsigned char i;

	// Erase the block
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
	
	// Write the block
	// Reload the block start address as it has been altered by the erase cycle
	tblptru = Block_Address >> 16;
	tblptrh = Block_Address >> 8;
	tblptrl = (unsigned char) Block_Address;
	// Write the data to the holding registers
	for (i = 0; i < FLASH_BLOCK_SIZE; i++)
	{
		tablat = *Pointer_Data_Buffer;
		asm tblwt*+; // Write the byte to the next holding register and automatically increment the address
		Pointer_Data_Buffer++;
	}
	// Return to the good block without setting the address another time
	asm tblrd*-;
	// Configure a write cycle
	eecon1 = 0x84; // Access flash program memory, allow writing to the program memory
	// Execute the special write sequence
	eecon2 = 0x55;
	eecon2 = 0xAA;
	// Start the write cycle
	eecon1.WR =1;
	
	// Disable writing to memory
	eecon1.WREN = 0;
}