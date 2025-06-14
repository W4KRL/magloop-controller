/**
 * @file scpiControl.h
 * @brief SCPI (Standard Commands for Programmable Instruments) command handling for the magloop-controller project.
 *
 * This header declares the interface for SCPI command parsing, processing, and instrument control.
 * It provides forward declarations, global configuration variables, and function prototypes for
 * handling SCPI commands and user preferences.
 *
 * Global Variables:
 * - speedScan:        Motor high speed percentage for scanning.
 * - speedJog:         Motor low speed percentage for fine tuning.
 * - pressDuration:    Duration (ms) for a long button press.
 * - jogDuration:      Motor jog duration in milliseconds.
 * - repeatInterval:   Jog repeat interval in milliseconds.
 *
 * Functions:
 * - processSCPIcommand:         Processes a raw SCPI command string.
 * - restorePreferences:         Restores user preferences to default or saved values.
 * - scpiErrorHandler:           Handles SCPI command errors.
 * - instrumentClearStorage:     Clears instrument storage.
 * - instrumentIdentify:         Returns instrument identification.
 * - instrumentReset:            Resets the instrument.
 * - getHelp:                    Provides help information.
 * - set/getControlDuration:     Sets or gets the control duration parameter.
 * - set/getControlJog:          Sets or gets the jog parameter.
 * - getControlList:             Retrieves a list of control parameters.
 * - set/getControlPress:        Sets or gets the press duration parameter.
 * - set/getControlRepeat:       Sets or gets the repeat interval parameter.
 * - set/getControlScan:         Sets or gets the scan speed parameter.
 * - getSystemFirmware:          Retrieves firmware information.
 * - getSystemEnvironment:       Retrieves environment information.
 * - getSystemVoltage:           Retrieves system voltage.
 * - getSystemWebserver:         Retrieves webserver status.
 * - getSystemList:              Retrieves a list of system parameters.
 * - scpiBegin:                  Initializes SCPI command processing.
 *
 * Types:
 * - SCPI_Commands:      Structure representing SCPI command metadata.
 * - SCPI_Parameters:    Structure representing SCPI command parameters.
 * - SCPI_Parser:        Class responsible for parsing SCPI commands.
 *
 * @author Karl Berrger
 * @date 2025-05-20
 */

#ifndef SCPICONTROL_H
#define SCPICONTROL_H

#include <Arduino.h> // Required for platformIO

// Forward declarations:
struct SCPI_Commands;			// Forward declaration
struct SCPI_Parameters;			// Forward declaration
using SCPI_C = SCPI_Commands;	// Alias for brevity
using SCPI_P = SCPI_Parameters; // Alias for brevity
class SCPI_Parser;				// <-- Critical addition

extern SCPI_Parser scpi; // Declaration

// Declare global variables for user Preferences:
extern int speedScan;	   //   motor high speed % for scan
extern int speedJog;	   //   motor low speed % for fine tune
extern int pressDuration;  //   long button press duration ms
extern int jogDuration;	   //   motor jog duration ms
extern int repeatInterval; //   jog repeat interval ms

String processSCPIcommand(String scpiCommand);
void restorePreferences();
void scpiErrorHandler(SCPI_C commands, SCPI_P parameters, Stream &interface);
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