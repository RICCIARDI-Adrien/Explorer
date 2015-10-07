/** @file UART_Windows.c
 * @see UART.h for description.
 * @author Adrien RICCIARDI
 */
#ifdef WIN32 // This file will compile on Windows only
#include <stdio.h>
#include <windows.h>
#include "UART.h"
 
//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The serial port descriptor. */
static HANDLE COM_Handle;

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
int UARTOpen(char *String_Device_File_Name, unsigned int Baud_Rate)
{
	DCB COM_Parameters;
	COMMTIMEOUTS Timing_Parameters;
	char String_Full_Device_Name[128];

	// Access to the raw COM device
	sprintf(String_Full_Device_Name, "\\\\.\\%s", String_Device_File_Name);
	
	// Open the serial port and set all access rights
	COM_Handle = CreateFile(String_Full_Device_Name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (COM_Handle == INVALID_HANDLE_VALUE) return 1; // Error : can't access to the serial port
	
	// Configure port
	COM_Parameters.DCBlength = sizeof(DCB);
    COM_Parameters.fBinary = 1; // Must be set to 1 or Windows becomes angry
	COM_Parameters.fParity = 0; // No parity
	// Ignore modem signals
	COM_Parameters.fOutxCtsFlow = 0; 
	COM_Parameters.fOutxDsrFlow = 0;
	COM_Parameters.fDtrControl = DTR_CONTROL_DISABLE;
	COM_Parameters.fDsrSensitivity = 0;
	COM_Parameters.fTXContinueOnXoff = 0;
	COM_Parameters.fOutX = 0;
	COM_Parameters.fInX = 0;
	COM_Parameters.fErrorChar = 0;
	COM_Parameters.fNull = 0;
	COM_Parameters.fRtsControl = RTS_CONTROL_DISABLE;
	COM_Parameters.fAbortOnError = 0;
	COM_Parameters.fDummy2 = 0;
	COM_Parameters.wReserved = 0;
	COM_Parameters.XonLim = 0;
	COM_Parameters.XoffLim = 0;
	COM_Parameters.ByteSize = 8; // 8 bits of data
	COM_Parameters.Parity = NOPARITY; // Parity check disabled
	COM_Parameters.StopBits = ONESTOPBIT;
	COM_Parameters.XonChar = 0;
	COM_Parameters.XoffChar = 0;
	COM_Parameters.ErrorChar = 0;
	COM_Parameters.EofChar = 0;
	COM_Parameters.EvtChar = 0;
	COM_Parameters.wReserved1 = 0;
	
	// Set transmit and receive speed
	COM_Parameters.BaudRate = Baud_Rate;
	
	// Set new parameters
	SetCommState(COM_Handle, &COM_Parameters);
	
	// Make reads non blocking
	Timing_Parameters.ReadIntervalTimeout = MAXDWORD; // According to MSDN, make the ReadFile() function returns immediately
	Timing_Parameters.ReadTotalTimeoutMultiplier = 0;
	Timing_Parameters.ReadTotalTimeoutConstant = 0;
	Timing_Parameters.WriteTotalTimeoutMultiplier = 0;
	Timing_Parameters.WriteTotalTimeoutConstant = 0;
	SetCommTimeouts(COM_Handle, &Timing_Parameters);
	
	// No error
	return 0;
}

unsigned char UARTReadByte(void)
{
	unsigned char Byte;
	DWORD Number_Bytes_Read;
	
	do
	{
		ReadFile(COM_Handle, &Byte, 1, &Number_Bytes_Read, NULL);
	} while (Number_Bytes_Read == 0);
	return Byte;
}

void UARTReadBuffer(void *Pointer_Buffer, unsigned int Bytes_Count)
{
	unsigned char Byte, *Pointer_Buffer_Byte = Pointer_Buffer;
	DWORD Number_Bytes_Read;

	while (Bytes_Count > 0)
	{
		// Try to get a byte
		ReadFile(COM_Handle, &Byte, 1, &Number_Bytes_Read, NULL);

		if (Number_Bytes_Read > 0)
		{
			*Pointer_Buffer_Byte = Byte;
			Pointer_Buffer_Byte++;
			Bytes_Count--;
		}
	}
}

void UARTWriteByte(unsigned char Byte)
{
	DWORD Number_Bytes_Written;
	
	WriteFile(COM_Handle, &Byte, 1, &Number_Bytes_Written, NULL);
}

void UARTWriteBuffer(void *Pointer_Buffer, unsigned int Bytes_Count)
{
	DWORD Number_Bytes_Written;
	
	WriteFile(COM_Handle, Pointer_Buffer, Bytes_Count, &Number_Bytes_Written, NULL);
}

int UARTIsByteAvailable(unsigned char *Pointer_Available_Byte)
{
	DWORD Number_Bytes_Read;
	
	ReadFile(COM_Handle, Pointer_Available_Byte, 1, &Number_Bytes_Read, NULL);
	if (Number_Bytes_Read == 0) return 0;
	return 1;
}

void UARTClose(void)
{
	CloseHandle(COM_Handle);
}

#endif
	