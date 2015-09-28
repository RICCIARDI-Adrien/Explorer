/** @file UART.c
 * @see UART.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "UART.h"

//--------------------------------------------------------------------------------------------------
// Private constants
//--------------------------------------------------------------------------------------------------
/** The frequency divider value to achieve a 115200 bit/s baud rate. */
#define UART_BAUD_RATE_DIVIDER 138

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void UARTInitialize(void)
{
	// Configure the baud rate generator
	spbrg2 = (unsigned char) UART_BAUD_RATE_DIVIDER;
	spbrgh2 = UART_BAUD_RATE_DIVIDER >> 8;
	baudcon2 = 0x08; // Use 16-bit baud rate generator, disable Auto Baud Detect mode
	
	// Set the UART pins as inputs
	trisb.7 = 1; // RX
	trisb.6 = 1; // TX
	
	// Configure the module
	rcsta2 = 0x90; // Enable the serial port, select 8-bit reception, enable reception
	txsta2 = 0x24; // Use 8-bit transmission, enable transmission, use asynchronous mode, select high baud rate	
}

unsigned char UARTReadByte(void)
{
	// Wait for a byte to be received
	while (!pir3.RC2IF);
	return rcreg2;
}

void UARTWriteByte(unsigned char Byte)
{
	// Wait for the transmit buffer and FIFO to become free
	while ((!txsta2.TRMT) || (!pir3.TX2IF));
	
	// Send the byte
	txreg2 = Byte;
}