/** @file Protocol.c
 * @see Protocol.h for description.
 * @author Adrien RICCIARDI
 */
#include <Serial_Port.h>
#include <stdlib.h> // Needed by atexit()
#include <unistd.h> // Needed by usleep()
#include "Configuration.h"
#include "Hex_Parser.h"
#include "Protocol.h"

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** The protocol magic number. */
#define PROTOCOL_MAGIC_NUMBER 0xA5
/** The bootloader acknowledges that it has received and flashed a block. */
#define PROTOCOL_ACKNOWLEDGE 0x42

/** How many bytes can be sent in one time. */
#define PROTOCOL_SEND_BUFFER_SIZE 64

//-------------------------------------------------------------------------------------------------
// Private types
//-------------------------------------------------------------------------------------------------
/** All existing commands. */
typedef enum
{
	PROTOCOL_COMMAND_GET_BATTERY_VOLTAGE,
	PROTOCOL_COMMAND_GET_DISTANCE_SENSOR_VALUE
} TProtocolCommand;

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The opened serial port identifier. */
static TSerialPortID Protocol_Serial_Port_ID;

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Close the UART on program exit. */
static void ProtocolExitCloseSerialPort(void)
{
	SerialPortClose(Protocol_Serial_Port_ID);
}

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
int ProtocolInitialize(char *String_Serial_Port_File)
{
	// Try to open the serial port
	if (SerialPortOpen(String_Serial_Port_File, 115200, &Protocol_Serial_Port_ID) == 0)
	{
		atexit(ProtocolExitCloseSerialPort);
		return 0;
	}

	return 1;
}

float ProtocolGetBatteryVoltage(void)
{
	int Raw_Voltage;
	
	// Send the command
	Debug("[%s] Sending magic number...\n", __func__);
	SerialPortWriteByte(Protocol_Serial_Port_ID, PROTOCOL_MAGIC_NUMBER);
	Debug("[%s] Sending command...\n", __func__);
	SerialPortWriteByte(Protocol_Serial_Port_ID, PROTOCOL_COMMAND_GET_BATTERY_VOLTAGE);
	
	// Receive the raw voltage
	Debug("[%s] Waiting for answer...\n", __func__);
	Raw_Voltage = (SerialPortReadByte(Protocol_Serial_Port_ID) << 8) | SerialPortReadByte(Protocol_Serial_Port_ID);
	Debug("[%s] Raw voltage : %d.\n", __func__, Raw_Voltage);
	
	// Convert the voltage to volts
	return (15.f * Raw_Voltage) / 1023.f;
}

int ProtocolGetSonarDistance(void)
{
	int Raw_Distance;
	
	// Send the command
	Debug("[%s] Sending magic number...\n", __func__);
	SerialPortWriteByte(Protocol_Serial_Port_ID, PROTOCOL_MAGIC_NUMBER);
	Debug("[%s] Sending command...\n", __func__);
	SerialPortWriteByte(Protocol_Serial_Port_ID, PROTOCOL_COMMAND_GET_DISTANCE_SENSOR_VALUE);
	
	// Receive the raw distance
	Debug("[%s] Waiting for answer...\n", __func__);
	Raw_Distance = (SerialPortReadByte(Protocol_Serial_Port_ID) << 8) | SerialPortReadByte(Protocol_Serial_Port_ID);
	Debug("[%s] Measured time : %d ms.\n", __func__, Raw_Distance);
	
	// Convert it to centimeters
	return Raw_Distance / 58;
}

int ProtocolUpdateFirmware(char *String_Firmware_Hex_File)
{
	static unsigned char Microcontroller_Memory[CONFIGURATION_TARGET_PROCESSOR_MEMORY_SIZE];
	unsigned char Byte, *Pointer_Memory;
	int Firmware_Size, Bytes_To_Send_Count;
	
	// Convert the Hex file into something usable
	Debug("[%s] Converting the Hex file to binary...\n", __func__);
	Firmware_Size = HexParserConvertHexToBinary(String_Firmware_Hex_File, CONFIGURATION_TARGET_PROCESSOR_MEMORY_SIZE, CONFIGURATION_FIRMWARE_BASE_ADDRESS, Microcontroller_Memory);
	if (Firmware_Size < 0)
	{
		printf("Error : failed to convert the hex file to binary.\n");
		return 1;
	}
	Debug("[%s] Conversion succeeded.\n", __func__);
	
	// Start sending instructions from the firmware beginning
	Pointer_Memory = &Microcontroller_Memory[CONFIGURATION_FIRMWARE_BASE_ADDRESS];
	
	// Wait for the microcontroller's bootloader "ready" code
	printf("Waiting for the bootloader code...\n");
	do
	{
		Byte = SerialPortReadByte(Protocol_Serial_Port_ID);
		Debug("[%s] Received byte : 0x%02X.\n", __func__, Byte);
	} while (Byte != PROTOCOL_MAGIC_NUMBER);
	
	// Send the same code to the bootloader to enter programming mode
	SerialPortWriteByte(Protocol_Serial_Port_ID, PROTOCOL_MAGIC_NUMBER);
	
	// Send the firmware size
	Debug("[%s] Sending the firmware size...\n", __func__);
	SerialPortWriteByte(Protocol_Serial_Port_ID, Firmware_Size >> 8);
	SerialPortWriteByte(Protocol_Serial_Port_ID, (unsigned char) Firmware_Size);
	
	// Send the instructions
	while (Firmware_Size > 0)
	{
		Debug("[%s] Total remaining of bytes to send : %d.\n", __func__, Firmware_Size);
		// Compute the amount of bytes to send
		if (Firmware_Size >= PROTOCOL_SEND_BUFFER_SIZE) Bytes_To_Send_Count = PROTOCOL_SEND_BUFFER_SIZE;
		else Bytes_To_Send_Count = Firmware_Size;
		
		// Send a block
		SerialPortWriteBuffer(Protocol_Serial_Port_ID, Pointer_Memory, Bytes_To_Send_Count);
		Pointer_Memory += Bytes_To_Send_Count;
		Firmware_Size -= Bytes_To_Send_Count;
		
		// Wait for the bootloader to acknowledge
		Byte = SerialPortReadByte(Protocol_Serial_Port_ID);
		if (Byte != PROTOCOL_ACKNOWLEDGE)
		{
			printf("Error : the bootloader acknowledged value was 0x%02X instead of 0x%02X.\n", Byte, PROTOCOL_ACKNOWLEDGE);
			return 2;
		}
	}
	printf("Firmware successfully updated.\n");
	
	return 0;
}