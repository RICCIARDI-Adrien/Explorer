/** @file Protocol.c
 * @see Protocol.h for description.
 * @author Adrien RICCIARDI
 */
#include <unistd.h> // Needed by usleep()
#include "Configuration.h"
#include "Hex_Parser.h"
#include "Protocol.h"
#include "UART.h"

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
	PROTOCOL_COMMAND_GET_DISTANCE_SENSOR_VALUE,
	PROTOCOL_COMMAND_START_FIRMWARE_UPDATE,
	PROTOCOL_COMMAND_GET_RUNNING_MODE
} TProtocolCommand;

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
float ProtocolGetBatteryVoltage(void)
{
	int Raw_Voltage;
	
	// Send the command
	UARTWriteByte(PROTOCOL_MAGIC_NUMBER);
	UARTWriteByte(PROTOCOL_COMMAND_GET_BATTERY_VOLTAGE);
	
	// Receive the raw voltage
	Raw_Voltage = (UARTReadByte() << 8) | UARTReadByte();
	Debug("[%s] Raw voltage : %d.\n", __func__, Raw_Voltage);
	
	// Convert the voltage to volts
	return (15.f * Raw_Voltage) / 1023.f;
}

int ProtocolUpdateFirmware(char *String_Firmware_Hex_File)
{
	unsigned char Microcontroller_Memory[CONFIGURATION_TARGET_PROCESSOR_MEMORY_SIZE], Byte, *Pointer_Memory;
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
	
	// Reboot the microcontroller in bootloader mode if it is executing the firmware
	if (ProtocolGetRunningMode() == 1)
	{
		// Send the "start update" command to make the firmware reboot in bootloader mode
		Debug("[%s] Rebooting in bootloader mode...\n", __func__);
		UARTWriteByte(PROTOCOL_MAGIC_NUMBER);
		UARTWriteByte(PROTOCOL_COMMAND_START_FIRMWARE_UPDATE);
		usleep(200000); // Wait some time to let the microcontroller reboot
	}
	
	// Send the "start update" command another time to start the update procedure
	Debug("[%s] Starting the update procedure...\n", __func__);
	UARTWriteByte(PROTOCOL_MAGIC_NUMBER);
	UARTWriteByte(PROTOCOL_COMMAND_START_FIRMWARE_UPDATE);
	
	// Send the firmware size
	Debug("[%s] Sending the firmware size...\n", __func__);
	UARTWriteByte(Firmware_Size >> 8);
	UARTWriteByte((unsigned char) Firmware_Size);
	
	// Send the instructions
	while (Firmware_Size > 0)
	{
		Debug("[%s] Total remaining of bytes to send : %d.\n", __func__, Firmware_Size);
		// Compute the amount of bytes to send
		if (Firmware_Size >= PROTOCOL_SEND_BUFFER_SIZE) Bytes_To_Send_Count = PROTOCOL_SEND_BUFFER_SIZE;
		else Bytes_To_Send_Count = Firmware_Size;
		
		// Send a block
		UARTWriteBuffer(Pointer_Memory, Bytes_To_Send_Count);
		Pointer_Memory += Bytes_To_Send_Count;
		Firmware_Size -= Bytes_To_Send_Count;
		
		// Wait for the bootloader to acknowledge
		Byte = UARTReadByte();
		if (Byte != PROTOCOL_ACKNOWLEDGE)
		{
			printf("Error : the bootloader acknowledged value was 0x%02X instead of 0x%02X.\n", Byte, PROTOCOL_ACKNOWLEDGE);
			return 2;
		}
	}
	
	return 0;
}

int ProtocolGetRunningMode(void)
{
	// Send the command
	UARTWriteByte(PROTOCOL_MAGIC_NUMBER);
	UARTWriteByte(PROTOCOL_COMMAND_GET_RUNNING_MODE);
	
	return UARTReadByte();
}