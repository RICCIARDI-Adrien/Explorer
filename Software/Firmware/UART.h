/** @file UART.h
 * Interrupt-driven UART driver.
 * @author Adrien RICCIARDI
 */
#ifndef H_UART_H
#define H_UART_H

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize the UART at 115200 bauds, 8 data bits, no parity, 1 stop bit. */
void UARTInitialize(void);

/** The UART interrupt handler. */
void UARTInterruptHandler(void);

#endif