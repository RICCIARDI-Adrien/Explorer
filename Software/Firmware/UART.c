/** @file UART.c
 * @see UART.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "ADC.h"
#include "UART.h"

//--------------------------------------------------------------------------------------------------
// Private constants
//--------------------------------------------------------------------------------------------------
/** The frequency divider value to achieve a 115200 bit/s baud rate. */
#define UART_BAUD_RATE_DIVIDER 138

/** The protocol magic number. */
#define UART_PROTOCOL_MAGIC_NUMBER 0xA5
/** How many bytes are expected as answer. */
#define UART_PROTOCOL_COMMAND_ANSWER_SIZE 2

//--------------------------------------------------------------------------------------------------
// Private types
//--------------------------------------------------------------------------------------------------
/** All existing commands. */
typedef enum
{
	UART_COMMAND_GET_BATTERY_VOLTAGE,
	UART_COMMAND_GET_DISTANCE_SENSOR_VALUE,
	UART_COMMAND_START_FIRMWARE_UPDATE
} TUARTCommand;

//--------------------------------------------------------------------------------------------------
// Private functions
//--------------------------------------------------------------------------------------------------
/** Start transmitting the answer.
 * @param Answer_First_Byte The first byte to send.
 */
static void UARTStartAnswerTransmission(unsigned char Answer_First_Byte)
{
	// Enable the transmission interrupt
	pie3.TX2IE = 1;
	
	// Start transmission
	txreg2 = Answer_First_Byte;
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
	pie3 |= 0x30; // Enable reception and transmission interrupts
}

void UARTInterruptHandler(void)
{
	static unsigned char Is_Magic_Number_Received = 0, Command_Answer[UART_PROTOCOL_COMMAND_ANSWER_SIZE];
	unsigned char Byte;
	unsigned short Word;
	
	// A byte has been received
	if (pie3.RC2IE && pir3.RC2IF)
	{
		Byte = rcreg2;
		
		// TODO handle overflow
		
		// Wait for the magic number
		if (!Is_Magic_Number_Received && (Byte == UART_PROTOCOL_MAGIC_NUMBER)) Is_Magic_Number_Received = 1;
		// Execute the command only if the magic number has been received first
		else if (Is_Magic_Number_Received)
		{
			switch (Byte)
			{
				case UART_COMMAND_GET_BATTERY_VOLTAGE:
					Word = ADCGetLastSampledBatteryVoltage();
					Command_Answer[0] = Word >> 8;
					Command_Answer[1] = (unsigned char) Word;
					UARTStartAnswerTransmission(Command_Answer[0]);
					break;
					
				case UART_COMMAND_GET_DISTANCE_SENSOR_VALUE:
					// TODO
						// TEST
						Command_Answer[0] = 0x23;
						Command_Answer[1] = 0x45;
						//latb.5=1;latb.4=0;
					UARTStartAnswerTransmission(Command_Answer[0]);
					break;
				
				case UART_COMMAND_START_FIRMWARE_UPDATE:
					// TODO
					// set the flag
					// reboot
					
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
		// Disable the transmission interrupt as it is the last byte to send
		pie3.TX2IE = 0;
	
		// Send the command answer second byte
		txreg2 = Command_Answer[1];
	}
}