/** @file Protocol.h
 * Communicate with the robot.
 * @author Adrien RICCIARDI
 */
#ifndef H_PROTOCOL_H
#define H_PROTOCOL_H

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Get the current battery voltage.
 * @return the battery voltage converted to volts.
 */
float ProtocolGetBatteryVoltage(void);

/** Get the distance between the robot and the nearest object in front of it.
 * @return the distance in centimeters.
 */
int ProtocolGetSonarDistance(void);

/** Update the robot firmware.
 * @param String_Firmware_Hex_File The Intel Hex file containing the firmware.
 * @return 0 if the firmware was successfully updated,
 * @return 1 if the provided firmware is bad,
 * @return 2 if a communication error occurred.
 */
int ProtocolUpdateFirmware(char *String_Firmware_Hex_File);

/** Get the microcontroller currently running software.
 * @return 0 if the microcontroller is running its bootloader,
 * @return 1 if the microcontroller is running its firmware.
 */
int ProtocolGetRunningMode(void);

#endif