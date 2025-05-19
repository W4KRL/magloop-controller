//! @file scpiControl.h
//! 2025-05-10 revise SYS:ENV and others

/**
 * @file scpiControl.h
 * @brief SCPI (Standard Commands for Programmable Instruments) control functions for managing
 *        motor control, system state, and environmental data in an embedded system.
 *
 * This header file defines SCPI commands and their corresponding functions for controlling
 * and querying various aspects of the system, including motor speeds, durations, system
 * voltage, temperature, humidity, and web server state. It also provides functionality for
 * restoring user preferences and handling SCPI command parsing.
 *
 * @details
 * SCPI Commands:
 * - *CLS: Clears the user preference settings.
 * - *IDN?: Returns the instrument identification.
 * - *RST: Resets the instrument to default state, retaining user settings.
 * - HELP?: Returns the list of available SCPI commands.
 *
 * Control Commands:
 * - CONtrol:DURation <value>: Sets the jog mode duration in milliseconds.
 * - CONtrol:DURation?: Returns the jog mode duration in milliseconds.
 * - CONtrol:JOGspeed <value>: Sets the jog speed (low range) as a percentage (0–100).
 * - CONtrol:JOGspeed?: Returns the jog speed (low range) as a percentage (0–100).
 * - CONtrol:LISt?: Returns all motor control mode settings.
 * - CONtrol:PRESSduration <value>: Sets or returns the jog mode button long press duration in milliseconds.
 * - CONtrol:PRESSduration?: Returns the jog mode button long press duration in milliseconds.
 * - CONtrol:REPeatinterval <value>: Sets the jog mode button repeat interval in milliseconds.
 * - CONtrol:REPeatinterval?: Returns the jog mode button repeat interval in milliseconds.
 * - CONtrol:SCANspeed <value>: Sets or returns the scan speed (high range) as a percentage (0–100).
 * - CONtrol:SCANspeed?: Returns the scan speed (high range) as a percentage (0–100).
 *
 * System Commands:
 * - SYStem:ENVironment?: Returns the enclosure temperature and humidity.
 * - SYStem:LISt?: Returns the system state.
 * - SYStem:VOLTage?: Returns the system supply voltage.
 * - SYStem:WEBserver?: Returns web server state.
 *
 * @note The SCPI parser is implemented using the Vrekrer SCPI Parser library.
 *
 * @dependencies
 * - Arduino.h: Required for PlatformIO.
 * - configuration.h: For SCPI identification.
 * - debug_magloop.h: For debugging utilities.
 * - Vrekrer_scpi_parser.h: SCPI parser library.
 * - StreamString.h: For handling SCPI command responses.
 * - ESPAsyncWebServer.h: For web server and WebSocket communication.
 * - Preferences.h: For storing user preferences in flash memory.
 * - Wire.h: For I2C communication.
 * - SHT2x.h: For temperature and humidity sensor.
 * - WiFi.h: For WiFi-related functions.
 * - webSocket.h: For WebSocket communication.
 *
 * @global_variables
 * - speedScan: Motor high speed percentage for scanning.
 * - speedJog: Motor low speed percentage for fine-tuning.
 * - pressDuration: Long button press duration in milliseconds.
 * - jogDuration: Motor jog duration in milliseconds.
 * - repeatInterval: Jog repeat interval in milliseconds.
 *
 * @functions
 * - processSCPIcommand(): Routes incoming SCPI commands to the parser and returns the response.
 * - restorePreferences(): Restores user preferences from flash memory.
 * - scpiBegin(): Initializes SCPI parser, registers commands, and restores preferences.
 */

#ifndef SCPICONTROL_H
#define SCPICONTROL_H

#include <Arduino.h>			 // Required for platformIO
#include <Vrekrer_scpi_parser.h> // https://github.com/Vrekrer/Vrekrer_scpi_parser
								 // Declare global variables for Preferences:
extern int speedScan;			 //   motor high speed % for scan
extern int speedJog;			 //   motor low speed % for fine tune
extern int pressDuration;		 //   long button press duration ms
extern int jogDuration;			 //   motor jog duration ms
extern int repeatInterval;		 //   jog repeat interval ms

String processSCPIcommand(String scpiCommand);
void restorePreferences();
// void scpiErrorHandler(SCPI_C commands, SCPI_P parameters, Stream &interface);
void instrumentClearStorage(SCPI_C commands, SCPI_P parameters, Stream &interface);
void instrumentIdentify(SCPI_C commands, SCPI_P parameters, Stream &interface);
void instrumentReset(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getHelp(SCPI_C commands, SCPI_P parameters, Stream &interface);
void setControlDuration(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getControlDuration(SCPI_C commands, SCPI_P parameters, Stream &interface);
void setControlJog(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getControlJog(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getControlList(SCPI_C commands, SCPI_P parameters, Stream &interface);
void setControlPress(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getControlPress(SCPI_C commands, SCPI_P parameters, Stream &interface);
void setControlRepeat(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getControlRepeat(SCPI_C commands, SCPI_P parameters, Stream &interface);
void setControlScan(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getControlScan(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getSystemFirmware(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getSystemEnvironment(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getSystemVoltage(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getSystemWebserver(SCPI_C commands, SCPI_P parameters, Stream &interface);
void getSystemList(SCPI_C commands, SCPI_P parameters, Stream &interface);
void scpiBegin();

#endif // SCPICONTROL_H