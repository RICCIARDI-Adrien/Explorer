/** Hex_Parser.c
 * @see Hex_Parser.h for description.
 * @author Adrien RICCIARDI
 */
#include <stdio.h>
#include "Configuration.h"
#include "Hex_Parser.h"

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Convert an hexadecimal character (a nibble) to its binary value.
 * @return The character binary value.
 */
static inline unsigned char ConvertHexadecimalCharacterToNibble(char Character)
{
	// Cast the character in upper case if needed
	if ((Character >= 'a') && (Character <= 'f')) Character -= 32;
	
	// Convert the character
	if (Character <= '9') Character -= '0'; // The character is a digit
	else Character = Character - 'A' + 10; // The character is a letter

	return Character;
}

/** Convert an hexadecimal byte represented by two characters into the corresponding binary value.
 * @param First_Character The number's high nibble.
 * @param Second_Character The number's low nibble.
 * @return The corresponding binary value.
 */
static unsigned char ConvertHexadecimalNumberToByte(char First_Character, char Second_Character)
{
	unsigned char Value;
	
	Value = (ConvertHexadecimalCharacterToNibble(First_Character) << 4) & 0xF0;
	Value |= ConvertHexadecimalCharacterToNibble(Second_Character) & 0x0F;
	return Value;
}

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
THexParserRecordType HexParserParseLine(char *String_Hexadecimal_Line, int *Pointer_Data_Size, int *Pointer_Load_Offset, unsigned char *Pointer_Data)
{
	THexParserRecordType Record_Type;
	int i;
	
	Debug("[HexParserInitialize] Parsing line %s", String_Hexadecimal_Line); // No new line at the end of this string as the read line adds it
	
	// Bypass the record mark (':')
	String_Hexadecimal_Line++;
	
	// Extract the record size
	*Pointer_Data_Size = ConvertHexadecimalNumberToByte(String_Hexadecimal_Line[0], String_Hexadecimal_Line[1]);
	String_Hexadecimal_Line += 2;
	Debug("[HexParserInitialize] Record data size = %d\n", *Pointer_Data_Size);
	
	// Extract the load offset
	*Pointer_Load_Offset = ConvertHexadecimalNumberToByte(String_Hexadecimal_Line[0], String_Hexadecimal_Line[1]) << 8;
	*Pointer_Load_Offset |= ConvertHexadecimalNumberToByte(String_Hexadecimal_Line[2], String_Hexadecimal_Line[3]);
	String_Hexadecimal_Line += 4;
	Debug("[HexParserInitialize] Record load offset = 0x%04X\n", *Pointer_Load_Offset);
	
	// Extract the record type
	Record_Type = ConvertHexadecimalNumberToByte(String_Hexadecimal_Line[0], String_Hexadecimal_Line[1]);
	String_Hexadecimal_Line += 2;
	Debug("[HexParserInitialize] Record type = %d\n", Record_Type);
	
	// Convert the data if there are provided
	Debug("[HexParserInitialize] Data : ");
	for (i = 0; i < *Pointer_Data_Size; i++)
	{
		*Pointer_Data = ConvertHexadecimalNumberToByte(String_Hexadecimal_Line[0], String_Hexadecimal_Line[1]);
		Debug("0x%02X ", *Pointer_Data);
		Pointer_Data++;
		String_Hexadecimal_Line += 2;
	}
	Debug("\n");
	
	return Record_Type;	
}