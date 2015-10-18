/** @file UART.h
 * Simple polling UART driver.
 * @author Adrien RICCIARDI
 */
#ifndef H_UART_H
#define H_UART_H

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the UART at 115200 bauds, 8 data bits, no parity, 1 stop bit. */
void UARTInitialize(void);

/** Block until a byte is received from the UART.
 * @return The read byte.
 */
unsigned char UARTReadByte(void);

/** Tell if a byte has been received or not.
 * @return 1 if a byte is available to read,
 * @return 0 if no byte was received.
 */
unsigned char UARTIsByteReceived(void);

/** Write a byte to the UART.
 * @param Byte The byte to send.
 */
void UARTWriteByte(unsigned char Byte);

#endif