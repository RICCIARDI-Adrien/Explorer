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

/** Get the distance between the robot and something in front of it.
 * @param Pointer_Distance_Centimeters On output, contain the distance from the nearest object converted to cm.
 * @return 0 if the distance was successfully retrieved, TODO
 * @return 1 if a communication error occurred. TODO comme voltage
 */
int ProtocolGetSonarDistance(int *Pointer_Distance_Centimeters);

/** Update the robot firmware.
 * @param String_Firmware_Hex_File The Intel Hex file containing the firmware.
 * @return 0 if the firmware was successfully updated,
 * @return 1 if the provided firmware is bad,
 * @return 2 if a communication error occurred.
 */
int ProtocolUpdateFirmware(char *String_Firmware_Hex_File);

#endif