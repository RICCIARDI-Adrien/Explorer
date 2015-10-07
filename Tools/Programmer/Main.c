/** @file Main.c
 * Convert an Intel hex file into a binary representation of the MCU's memory and program it through a bootloader.
 * @author Adrien RICCIARDI
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Configuration.h"
#include "Hex_Parser.h"
#include "UART.h"

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** The protocol magic number. */
#define MAIN_PROTOCOL_MAGIC_NUMBER 0xA5
/** The "start update" command. */
#define MAIN_PROTOCOL_COMMAND_START_FIRMWARE_UPDATE 2
/** The bootloader acknowledges that it has received and flashed a block. */
#define MAIN_PROTOCOL_ACKNOWLEDGE 0x42
/** How many bytes can be sent in one time. */
#define MAIN_PROTOCOL_SEND_BUFFER_SIZE 64

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The target processor memory. */
static unsigned char Memory[CONFIGURATION_TARGET_PROCESSOR_MEMORY_SIZE];

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Parse the hex file to put all instructions in a microcontroller memory representation (the Memory array).
 * @param String_Firmware_File The firmware hex file.
 * @return The firmware size in bytes in case of success,
 * @return -1 if an error occurred.
 */
static int MainConvertFirmwareToBinary(char *String_Firmware_File)
{
	FILE *File_Hex;
	THexParserRecordType Record_Type;
	char String_Record_Line[HEX_PARSER_RECORD_MAXIMUM_DATA_SIZE * 2 + 10]; // Can contain a full record represented by hexadecimal characters
	int Current_Line_Number = 0, Data_Size, Load_Offset, Current_Address, Highest_Address = 0;
	unsigned char Data[HEX_PARSER_RECORD_MAXIMUM_DATA_SIZE];
	unsigned short Current_Address_High_Word = 0; // Set by an Extended Linear Address Record
	
	// Try to open the Hex file
	File_Hex = fopen(String_Firmware_File, "rb");
	if (File_Hex == NULL)
	{
		printf("Error : could not open the Hex file.\n");
		return -1;
	}
	
	// Initialize the memory representation with the erased flash value
	memset(Memory, 0xFF, sizeof(Memory));
	
	// Process all records
	while (fgets(String_Record_Line, sizeof(String_Record_Line), File_Hex) != NULL)
	{
		Current_Line_Number++;
		Debug("[main] Processing line %d\n", Current_Line_Number);
		
		// Get the record type
		Record_Type = HexParserParseLine(String_Record_Line, &Data_Size, &Load_Offset, Data);
		switch (Record_Type)
		{
			case HEX_PARSER_RECORD_TYPE_DATA:
				Debug("[main] Record type : data\n");
				
				// Compute the instruction address
				Current_Address = (Current_Address_High_Word << 16) | Load_Offset;
				Debug("[main] Instruction address : 0x%08X.\n", Current_Address);
				
				// Check address consistency (an address can't be lower than the base address)
				if (Current_Address < CONFIGURATION_FIRMWARE_BASE_ADDRESS)
				{
					printf("The instruction at line %d has an out of bounds address (0x%04X).\n", Current_Line_Number, Load_Offset);
					return -1;
				}
				
				// Put the instruction at the right memory place only if the instruction is in the firmware space (not the configuration registers location)
				if (Current_Address < CONFIGURATION_TARGET_PROCESSOR_MEMORY_SIZE) memcpy(&Memory[Current_Address], Data, Data_Size);
				
				// Find the highest address used in the program, so we can omit the remaining memory and shorten the output file (in order to shorten the flashing time)
				if ((Current_Address < CONFIGURATION_TARGET_PROCESSOR_MEMORY_SIZE) && Current_Address > Highest_Address) Highest_Address = Current_Address;
				break;
				
			case HEX_PARSER_RECORD_TYPE_END_OF_FILE:
				Debug("[main] Record type : end of file\n");
				break;

			// Allow to define the upper bits of a 32-bit Linear Base Address
			case HEX_PARSER_RECORD_TYPE_EXTENDED_LINEAR_ADDRESS:
				Debug("[main] Record type : extended linear address\n");

				Current_Address_High_Word = (Data[0] << 8) | Data[1];
				Debug("[main] Current address changed to 0x%08X.\n", Current_Address_High_Word << 16);
				break;

			default:
				printf("The line %d contains an unrecognized record (0x%02X). Aborting conversion.\n", Current_Line_Number, Record_Type);
				return -1;
		}
		Debug("\n");
	}
	
	// Return the firmware size
	return Highest_Address - CONFIGURATION_FIRMWARE_BASE_ADDRESS;
}

/** Send the memory array content to the bootloader.
 * @param Firmware_Size The firmware size in bytes.
 * @return 0 if the firmware was successfully updated,
 * @return -1 if an error occurred.
 */
static int MainUpdateFirmware(int Firmware_Size)
{
	int Bytes_To_Send_Count;
	unsigned char Byte, *Pointer_Memory = &Memory[CONFIGURATION_FIRMWARE_BASE_ADDRESS]; // Start sending instructions from the firmware beginning
	
	// Send the "start update" command to make the firmware reboot in bootloader mode
	/*Debug("[MainUpdateFirmware] Rebooting in bootloader mode...\n");
	UARTWriteByte(MAIN_PROTOCOL_MAGIC_NUMBER);
	UARTWriteByte(MAIN_PROTOCOL_COMMAND_START_FIRMWARE_UPDATE);
	usleep(200000); // Wait some time to let the MCU reboot*/
	
	// Send the "start update" command another time to start the update procedure
	Debug("[MainUpdateFirmware] Starting the update procedure...\n");
	UARTWriteByte(MAIN_PROTOCOL_MAGIC_NUMBER);
	UARTWriteByte(MAIN_PROTOCOL_COMMAND_START_FIRMWARE_UPDATE);
	
	// Send the firmware size
	Debug("[MainUpdateFirmware] Sending the firmware size...\n");
	UARTWriteByte(Firmware_Size >> 8);
	UARTWriteByte((unsigned char) Firmware_Size);
	
	// Send the instructions
	while (Firmware_Size > 0)
	{
		Debug("[MainUpdateFirmware] Total remaining of bytes to send : %d.\n", Firmware_Size);
		// Compute the amount of bytes to send
		if (Firmware_Size >= MAIN_PROTOCOL_SEND_BUFFER_SIZE) Bytes_To_Send_Count = MAIN_PROTOCOL_SEND_BUFFER_SIZE;
		else Bytes_To_Send_Count = Firmware_Size;
		
		// Send a block
		UARTWriteBuffer(Pointer_Memory, Bytes_To_Send_Count);
		Pointer_Memory += Bytes_To_Send_Count;
		Firmware_Size -= Bytes_To_Send_Count;
		
		// Wait for the bootloader to acknowledge
		Byte = UARTReadByte();
		if (Byte != MAIN_PROTOCOL_ACKNOWLEDGE)
		{
			printf("Error : the bootloader acknowledged value was 0x%02X instead of 0x%02X.\n", Byte, MAIN_PROTOCOL_ACKNOWLEDGE);
			return -1;
		}
	}
	
	return 0;
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int Firmware_Size;
	char *String_Serial_Port_File, *String_Hex_File;
	
	// Check parameters
	if (argc != 3)
	{
		printf("Update the Explorer robot firmware.\nUsage : %s Serial_Port Hex_File.\n", argv[0]);
		return EXIT_FAILURE;
	}
	String_Serial_Port_File = argv[1];
	String_Hex_File = argv[2];
	
	// Try to open the serial port
	if (UARTOpen(String_Serial_Port_File, 115200) != 0)
	{
		printf("Error : failed to open the serial port '%s'.\n", String_Serial_Port_File);
		return EXIT_FAILURE;
	}
	
	// Convert the hex file to binary
	printf("Starting conversion...\n");
	Firmware_Size = MainConvertFirmwareToBinary(String_Hex_File);
	if (Firmware_Size < 0) return EXIT_FAILURE;
	printf("Conversion terminated. Firmware size : %d bytes.\n", Firmware_Size);
	
	// Send the firmware to the bootloader
	printf("Updating the firmware...\n");
	MainUpdateFirmware(Firmware_Size);
	printf("Firmware successfully updated.\n");
	
	return EXIT_SUCCESS;
}
