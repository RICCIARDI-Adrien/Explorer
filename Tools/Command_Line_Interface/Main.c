/** @file Main.c
 * Convert an Intel hex file into a binary representation of the MCU's memory and program it through a bootloader.
 * @author Adrien RICCIARDI
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Configuration.h"
#include "Protocol.h"
#include "UART.h"

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int Return_Value = EXIT_SUCCESS;
	char *String_Serial_Port_File, *String_Command, *String_Hex_File;
	
	// Check parameters
	if (argc < 3)
	{
		printf("Usage : %s Serial_Port Command [Parameters]\n"
			"Available commands :\n"
			"   -d : get the sonar distance from the nearest object\n"
			"   -v : get the battery voltage\n"
			"   -u Hex_File : update the robot firmware from an Intel Hex file\n" , argv[0]);
		return EXIT_FAILURE;
	}
	String_Serial_Port_File = argv[1];
	String_Command = argv[2];
	
	// Try to open the serial port
	if (UARTOpen(String_Serial_Port_File, 115200) != 0)
	{
		printf("Error : failed to open the serial port '%s'.\n", String_Serial_Port_File);
		return EXIT_FAILURE;
	}
	
	// Select the right command
	if (strcmp(String_Command, "-d") == 0) printf("Distance to the nearest object : %d cm\n", ProtocolGetSonarDistance());
	else if (strcmp(String_Command, "-v") == 0) printf("Battery voltage : %0.3f V\n", ProtocolGetBatteryVoltage());
	else if (strcmp(String_Command, "-u") == 0)
	{
		// Get the Hex file parameter
		if (argc < 4)
		{
			printf("Error : you must provide an Hex file path with the -u command.\n");
			Return_Value = EXIT_FAILURE;
			goto Exit;
		}
		String_Hex_File = argv[3];
		
		// Try to update the firmware
		if (ProtocolUpdateFirmware(String_Hex_File) != 0)
		{
			Return_Value = EXIT_FAILURE;
			goto Exit;
		}
	}
	else
	{
		printf("Error : unknown command.\n");
		Return_Value = EXIT_FAILURE;
	}

Exit:
	UARTClose();
	return Return_Value;
}
