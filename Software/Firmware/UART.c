/** @file UART.c
 * @see UART.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "ADC.h"
#include "Distance_Sensor.h"
#include "Flash.h"
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
#define UART_PROTOCOL_COMMAND_ANSWER_SIZE 2

/** The update flag location. */
#define UART_UPDATE_FLAG_ADDRESS 0xFFC0 //The PIC18F26K22 flash last block

//--------------------------------------------------------------------------------------------------
// Private types
//--------------------------------------------------------------------------------------------------
/** All existing commands. */
typedef enum
{
	UART_COMMAND_GET_BATTERY_VOLTAGE,
	UART_COMMAND_GET_DISTANCE_SENSOR_VALUE,
	UART_COMMAND_START_FIRMWARE_UPDATE,
	UART_COMMAND_GET_RUNNING_MODE
} TUARTCommand;

//--------------------------------------------------------------------------------------------------
// Private functions
//--------------------------------------------------------------------------------------------------
/** Start transmitting a single byte.
 * @param Byte The byte to send.
 */
inline void UARTStartOneByteTransmission(unsigned char Byte)
{
	// Disable the transmission interrupt as only one byte will be sent
	UART_DISABLE_TRANSMISSION_INTERRUPT();
	
	// Start transmission
	txreg2 = Byte;
}

/** Start transmitting two bytes.
 * @param First_Byte The first byte to send.
 */
inline void UARTStartTwoBytesTransmission(unsigned char First_Byte)
{
	// Enable the transmission interrupt to send the second byte when the first one has been sent
	UART_ENABLE_TRANSMISSION_INTERRUPT();
	
	// Start transmission
	txreg2 = First_Byte;
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
					UARTStartTwoBytesTransmission(Command_Answer[0]);
					break;
					
				case UART_COMMAND_GET_DISTANCE_SENSOR_VALUE:
					Word = DistanceSensorGetLastSampledDistance();
					Command_Answer[0] = Word >> 8;
					Command_Answer[1] = (unsigned char) Word;
					UARTStartTwoBytesTransmission(Command_Answer[0]);
					break;
				
				case UART_COMMAND_START_FIRMWARE_UPDATE:
					// Set the update flag to 0xFF
					FlashEraseBlock(UART_UPDATE_FLAG_ADDRESS);
					// Reboot the microcontroller
					asm reset;
					while (1); // Wait for the reset to happen
					
				case UART_COMMAND_GET_RUNNING_MODE:
					UARTStartOneByteTransmission(1);
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
		// Disable the transmission interrupt as it is the last byte to send
		UART_DISABLE_TRANSMISSION_INTERRUPT();
	
		// Send the command answer second byte
		txreg2 = Command_Answer[1];
	}
}