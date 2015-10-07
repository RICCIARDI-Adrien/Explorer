/** @file Flash.h
 * Read from and write to the flash memory.
 * @author Adrien RICCIARDI
 */
#ifndef H_FLASH_H
#define H_FLASH_H

//--------------------------------------------------------------------------------------------------
// Constants
//--------------------------------------------------------------------------------------------------
/** A program memory block size in bytes. */
#define FLASH_BLOCK_SIZE 64

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Read a single byte from the flash memory.
 * @param Byte_Address The byte to read address.
 * @return the byte value.
 */
unsigned char FlashReadByte(unsigned long Byte_Address);

/** Erase the specified block and write the provided data.
 * @param Block_Address The block beginning address.
 * @param Pointer_Data_Buffer The data to write. The buffer must be FLASH_BLOCK_SIZE bytes.
 */
void FlashWriteBlock(unsigned long Block_Address, unsigned char *Pointer_Data_Buffer);

#endif
