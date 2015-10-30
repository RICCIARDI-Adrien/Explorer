/** @file UART.c
 * @see UART.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "ADC.h"
#include "Distance_Sensor.h"
#include "UART.h"

//--------------------------------------------------------------------------------------------------
// Private constants and macros
//--------------------------------------------------------------------------------------------------
/** The frequency divider value to achieve a 115200 bit/s baud rate. */
#define UART_BAUD_RATE_DIVIDER 138

/** Enable the UART transmission interrupt. */
#define UART_ENABLE_TRANSMISSION_INTERRUPT() pie3.TX2IE = 1
/** Disable the UART transmission interrupt. */
#define UART_DISABLE_TRANSMISSION_INTERRUPT() pie3.TX2IE = 0

/** The protocol magic number. */
#define UART_PROTOCOL_MAGIC_NUMBER 0xA5
/** How many bytes are expected as answer. */
#define UART_PROTOCOL_COMMAND_ANSWER_MAXIMUM_SIZE 2

//--------------------------------------------------------------------------------------------------
// Private types
//--------------------------------------------------------------------------------------------------
/** All existing commands. */
typedef enum
{
	UART_COMMAND_GET_BATTERY_VOLTAGE,
	UART_COMMAND_GET_DISTANCE_SENSOR_VALUE
} TUARTCommand;

//--------------------------------------------------------------------------------------------------
// Private variables
//--------------------------------------------------------------------------------------------------
/** The data to transmit. */
static unsigned char UART_Transmission_Buffer[UART_PROTOCOL_COMMAND_ANSWER_MAXIMUM_SIZE];
/** How many bytes to send. */
static unsigned char UART_Remaining_Bytes_To_Send = 0;

//--------------------------------------------------------------------------------------------------
// Private functions
//--------------------------------------------------------------------------------------------------
/** Start transmitting data.
 * @param Bytes_To_Send_Count How many bytes to send.
 * @warning There is no check on the amount of bytes to send to save some cycles.
 */
inline void UARTStartTransmission(unsigned char Bytes_To_Send_Count)
{
	UART_Remaining_Bytes_To_Send = Bytes_To_Send_Count;
	UART_ENABLE_TRANSMISSION_INTERRUPT(); // This will immediately vector to the TX interrupt
}

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
	
	// Enable the UART interrupts
	ipr3 &= ~0x30; // Set both interrupts as low priority
	pie3.RC2IE = 1; // Enable only the reception interrupt (the transmission interrupt is enabled only when transmitting, moreover it would immediately trigger an interrupt if enabled as told in datasheet §16.1.1.7)
}

void UARTInterruptHandler(void)
{
	static unsigned char Is_Magic_Number_Received = 0;
	unsigned char Byte;
	unsigned short Word;
	
	// A byte has been received
	if (pie3.RC2IE && pir3.RC2IF)
	{
		Byte = rcreg2;
		
		// Handle an overflow error
		if (rcsta.OERR)
		{
			rcsta.CREN = 0; // Disable the reception to clear the error bit
			rcsta.CREN = 1; // Re-enable it
			return;
		}
		
		// Wait for the magic number
		if (!Is_Magic_Number_Received && (Byte == UART_PROTOCOL_MAGIC_NUMBER)) Is_Magic_Number_Received = 1;
		// Execute the command only if the magic number has been received first
		else if (Is_Magic_Number_Received)
		{
			switch (Byte)
			{
				case UART_COMMAND_GET_BATTERY_VOLTAGE:
					Word = ADCGetLastSampledBatteryVoltage();
					UART_Transmission_Buffer[0] = Word >> 8;
					UART_Transmission_Buffer[1] = (unsigned char) Word;
					UARTStartTransmission(2);
					break;
					
				case UART_COMMAND_GET_DISTANCE_SENSOR_VALUE:
					Word = DistanceSensorGetLastSampledDistance();
					UART_Transmission_Buffer[0] = Word >> 8;
					UART_Transmission_Buffer[1] = (unsigned char) Word;
					UARTStartTransmission(2);
					break;
					
				// Unknown command, do nothing
				default:
					break;
			}
			
			Is_Magic_Number_Received = 0;
		}
	}
	
	// A byte has been sent
	if (pie3.TX2IE && pir3.TX2IF)
	{
		// Send the next byte
		if (UART_Remaining_Bytes_To_Send > 0)
		{
			txreg2 = UART_Transmission_Buffer[UART_PROTOCOL_COMMAND_ANSWER_MAXIMUM_SIZE - UART_Remaining_Bytes_To_Send];
			UART_Remaining_Bytes_To_Send--;
			
			// Disable the transmission interrupt if there is no more byte to send
			if (UART_Remaining_Bytes_To_Send == 0) UART_DISABLE_TRANSMISSION_INTERRUPT();
		}
	}
}