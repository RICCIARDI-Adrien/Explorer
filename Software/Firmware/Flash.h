/** @file Flash.h
 * A minimalist driver that only allows to erase a block.
 * @author Adrien RICCIARDI
 */
#ifndef H_FLASH_H
#define H_FLASH_H

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Erase the specified block.
 * @param Block_Address The block starting address.
 */
void FlashEraseBlock(unsigned long Block_Address);

#endif