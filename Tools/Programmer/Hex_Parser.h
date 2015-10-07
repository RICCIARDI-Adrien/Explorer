/** @file Hex_Parser.h
 * Extract useful values from an Intel Hex file record.
 * Only data and end-of-file records are supported.
 * @author Adrien RICCIARDI
 */
#ifndef H_HEX_PARSER_H
#define H_HEX_PARSER_H

//-------------------------------------------------------------------------------------------------
// Constants
//-------------------------------------------------------------------------------------------------
/** The maximum amount of data a record can contain. */
#define HEX_PARSER_RECORD_MAXIMUM_DATA_SIZE 255

//-------------------------------------------------------------------------------------------------
// Types
//-------------------------------------------------------------------------------------------------
/** All the existing record types. */
typedef enum
{
	HEX_PARSER_RECORD_TYPE_DATA,
	HEX_PARSER_RECORD_TYPE_END_OF_FILE,
	HEX_PARSER_RECORD_TYPE_EXTENDED_SEGMENT_ADDRESS,
	HEX_PARSER_RECORD_TYPE_START_SEGMENT_ADDRESS,
	HEX_PARSER_RECORD_TYPE_EXTENDED_LINEAR_ADDRESS,
	HEX_PARSER_RECORD_TYPE_START_LINEAR_ADDRESS,
	HEX_PARSER_RECORD_TYPE_UNKNOWN
} THexParserRecordType;

//-------------------------------------------------------------------------------------------------
// Function
//-------------------------------------------------------------------------------------------------
/** Parse a line from the Hex file.
 * @param String_Hexadecimal_Line A line read from the hex file.
 * @param Pointer_Data_Size On output, contain the data size.
 * @param Pointer_Load_Offset On output, contain the data load offset.
 * @param Pointer_Data On output, contain the record data converted to binary.
 * @return The record type.
 */
THexParserRecordType HexParserParseLine(char *String_Hexadecimal_Line, int *Pointer_Data_Size, int *Pointer_Load_Offset, unsigned char *Pointer_Data);

#endif
