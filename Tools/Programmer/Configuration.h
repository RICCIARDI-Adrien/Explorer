/** @file Configuration.h
 * Contain the program configurable parameters.
 * @author Adrien RICCIARDI
 */
#ifndef H_CONFIGURATION_H
#define H_CONFIGURATION_H

//-------------------------------------------------------------------------------------------------
// Constants and macros
//-------------------------------------------------------------------------------------------------
/** The target processor memory size in bytes. */
#define CONFIGURATION_TARGET_PROCESSOR_MEMORY_SIZE 65536
/** The target processor firmware base address. */
#define CONFIGURATION_FIRMWARE_BASE_ADDRESS 0x0400

#if CONFIGURATION_ENABLE_DEBUG == 1
	#define Debug(Format, ...) printf(Format, ##__VA_ARGS__)
#else
	#define Debug(Format, ...)
#endif

#endif
