//! scpiControl.h
//! 2025-05-08 revised SCPI control functions, added error handling, add VM voltage divider

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
 * - SYStem:Environment?: Returns the enclosure temperature and humidity.
 * - SYStem:LISt?: Returns the system state.
 * - SYStem:VOLTage?: Returns the system supply voltage.
 * - SYStem:WEBserver?: Returns web server state.
 *
 * @note The SCPI parser is implemented using the Vrekrer SCPI Parser library.
 *
 * @dependencies
 * - Arduino.h: Required for PlatformIO.
 * - credentials.h: For SCPI identification.
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

#define SCPI_MAX_COMMANDS 35    // Maximum number of SCPI commands
#define SCPI_MAX_TOKENS 30      // Maximum number of tokens in a command
#define SCPI_BUFFER_LENGTH 128  // From 64
#define SCPI_ARRAY_SYZE 6       // Maximum number of elements in an array
#define SCPI_HASH_TYPE uint16_t // Default value = uint8_t

#include <Arduino.h>     // Required for platformIO
#include "credentials.h" // for SCPI identification
// #include "debug_magloop.h"       // DEBUG_PRINTF()
#include <Vrekrer_scpi_parser.h> // https://github.com/Vrekrer/Vrekrer_scpi_parser
#include <StreamString.h>        // for StreamString class in processSCPIcommand()
// #include <ESPAsyncWebServer.h>   // https://github.com/ESP32Async/ESPAsyncWebServer for StreamString class
#include <Preferences.h> // Store controller settings in flash with LittleFS
#include <Wire.h>        // for I2C communication
#include <SHT2x.h>       // for HTU21D temperature and humidity sensor
// #include <WiFi.h>                // for WiFi functions like localIP() and RSSI()
// #include "webSocket.h"           // for SYStem:WEBserver? command, to get web server state

extern AsyncWebSocket ws;                  // for debug statements
void notifyClients(const String &message); // for debug statements
;                                          // Instantiations
SCPI_Parser scpi;                          //   SCPI parser
Preferences preferences;                   //   Preferences storage
;                                          // Declare global variables for Preferences:
int speedScan;                             //   motor high speed % for scan
int speedJog;                              //   motor low speed % for fine tune
int pressDuration;                         //   long button press duration ms
int jogDuration;                           //   motor jog duration ms
int repeatInterval;                        //   jog repeat interval ms

//! Instantiate temperature and humidity sensor object
SHT2x envSensor; // HTU21D temperature and humidity sensor

//! Route web socket message to SCPI Parser
/**
 * @brief Processes an incoming SCPI (Standard Commands for Programmable Instruments) command
 *        and returns the formatted response.
 *
 * This function takes an SCPI command as input, executes it, and captures the response.
 * The response is then formatted for use with a JavaScript client and returned as a string.
 *
 * @param scpiCommand A String containing the SCPI command to be processed.
 * @return A String containing the formatted SCPI response prefixed with "scp~".
 *
 * @note The function uses a temporary buffer to store the SCPI command and a StreamString
 *       object to capture the response from the SCPI execution.
 */
String processSCPIcommand(String scpiCommand)
{
  char scpiCommandBuf[scpiCommand.length() + 1];                   // Create a buffer for the command
  scpiCommand.toCharArray(scpiCommandBuf, sizeof(scpiCommandBuf)); // Copy to buffer
  StreamString responseStream;                                     // Create a string to capture the response
  String scpiResponse;                                             // Initialize response string
  scpi.Execute(scpiCommandBuf, responseStream);                    // Execute SCPI command
  scpiResponse = "scp~" + responseStream;                          // format for JavaScript client
  return scpiResponse;                                             // return the response string
} // processSCPIcommand()

/**
 * @brief Restores user preferences from persistent storage.
 *
 * This function retrieves various user-configurable settings from
 * persistent storage (e.g., preferences) and assigns them to the
 * corresponding variables. These settings include motor speeds,
 * press duration, repeat interval, and jog duration.
 *
 * The following preferences are restored:
 * - `speedHigh`: Motor high speed percentage for scanning (default: 100%).
 * - `speedLow`: Motor low speed percentage for fine-tuning (default: 50%).
 * - `pressDuration`: Duration of a long press in milliseconds (default: 300 ms).
 * - `repeatInterval`: Interval between repeated actions in milliseconds (default: 200 ms).
 * - `jogDuration`: Duration of a jog action in milliseconds (default: 100 ms).
 */
void restorePreferences()
{
  speedScan = preferences.getInt("speedHigh", 100);           // motor high speed % for scan
  speedJog = preferences.getInt("speedLow", 50);              // motor low speed % for fine tune
  pressDuration = preferences.getInt("pressDuration", 300);   // long press duration ms
  repeatInterval = preferences.getInt("repeatInterval", 200); // repeat interval ms
  jogDuration = preferences.getInt("jogDuration", 100);       // jog duration ms
} // restorePreferences()

//! SCPI Command Functions

void scpiErrorHandler(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("ERROR: Unrecognized command: ");
  for (int i = 0; i < commands.Size(); i++)
  {
    if (i > 0)
      interface.print(":");
    interface.print(commands[i]);
  }
  interface.println();
} // scpiErrorHandler()

void instrumentClearStorage(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // reset to default parameters, processor not restarted
  preferences.clear();
  restorePreferences();
  interface.print("User settings cleared");
} // instrumentClearStorage()

void instrumentIdentify(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // required by SCPI spec for device identification
  int bufferLength = 100;
  char buffer[bufferLength];
  snprintf(buffer, bufferLength, "%s, %s, %s, %s", MAKER, MODEL, SERIAL_NUMBER, VERSION);
  interface.print(buffer);
} // instrumentIdentity()

void instrumentReset(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  static unsigned long resetStartTime = 0; // Track the start time

  interface.print("Restarting....\n");
  if (millis() - resetStartTime >= 2000)
  {
    ESP.restart(); // Restart the processor
  }
} // instrumentReset()

void getHelp(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("---Help\n");
  interface.print("*IDN? device identity\n");
  interface.print("*CLS clears user settings\n");
  interface.print("*RST reset device, user settings unchanged\n");
  interface.print("HELP? or Help get this help\n");
  interface.print("CON:DURation <50..200> jog duration in ms\n");
  interface.print("CON:DURation? get jog duration in ms\n");
  interface.print("CON:LISt? get all control settings\n");
  interface.print("CON:JOG <50..80> set jog speed in %\n");
  interface.print("CON:JOG? get jog speed in %\n");
  interface.print("CON:PREss button long press <200..500>ms\n");
  interface.print("CON:PREss? get long press duration in ms\n");
  interface.print("CON:REPeat button interval <100..300>ms\n");
  interface.print("CON:REPeat? get repeat interval in ms\n");
  interface.print("CON:SCAn <80..100> set scan speed in %\n");
  interface.print("CON:SCAn? get scan speed in %\n");
  interface.print("SYS:ERRor? get last SCPI error\n");
  interface.print("SYS:LISt? get system state\n");
  interface.print("SYS:TEMPerature? get encl. temp & humidity\n");
  interface.print("SYS:VOLTage? get supply voltage\n");
  interface.print("SYS:WEBserver? get web server state\n");
  interface.print("---End Help---");
} // getHelp()

//! CONtrol commands

void setControlDuration(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    jogDuration = constrain(String(parameters[0]).toInt(), 50, 200); // Ensure jogDuration is within 50-200 ms
    preferences.putInt("jogDuration", jogDuration);
    interface.printf("%s: %i ms", "Jog duration", jogDuration);
  }
  else
  {
    interface.printf("Jog duration value is missing\n");
  }
} // sedtControlDuration()

void getControlDuration(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.printf("%s: %i ms", "Jog duration", jogDuration);
} // getControlDuration()

void setControlJog(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    speedJog = constrain(String(parameters[0]).toInt(), 50, speedScan);
    preferences.putInt("speedLow", speedJog);
    interface.printf("%s: %i%%", "Jog speed", speedJog);
  }
  else
  {
    interface.printf("Scan speed value is missing\n");
  }
} // setControlJogSpeed()

void getControlJog(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.printf("%s: %i%%", "Jog speed", speedJog);
} // getControlJogSpeed()

void getControlList(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("---Control List\n");
  interface.printf("%-15s %i%%\n", "Scan speed", speedScan);
  interface.printf("%-15s %i%%\n", "Jog speed", speedJog);
  interface.printf("%-15s %i ms\n", "Jog duration", jogDuration);
  interface.printf("%-15s %i ms\n", "Press duration", pressDuration);
  interface.printf("%-15s %i ms\n", "Repeat interval", repeatInterval);
  interface.print("---End Control List---");
} // getControlList()

void setControlPress(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    pressDuration = String(parameters[0]).toInt();
    preferences.putInt("pressDuration", pressDuration);
    interface.printf("%s: %i ms", "Press duration", pressDuration);
  }
  else
  {
    interface.printf("Press duration value is missing\n");
  }
} // setControlPressDuration()

void getControlPress(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.printf("%s: %i ms", "Press duration", pressDuration);
} // getControlPressDuration()

void setControlRepeat(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    repeatInterval = String(parameters[0]).toInt();
    preferences.putInt("repeatInterval", repeatInterval);
    interface.printf("%s: %i ms", "Repeat Interval", repeatInterval);
  }
  else
  {
    interface.printf("Repeat interval value is missing\n");
  }
} // setControlRepeatInterval()

void getControlRepeat(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.printf("%s: %i ms", "Repeat Interval", repeatInterval);
} // getControlRepeatInterval()

void setControlScan(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0) // Check if parameters exist (SET mode)
  {
    speedScan = constrain(String(parameters[0]).toInt(), 50, 100); // Convert to int and constrain to range
    preferences.putInt("speedHigh", speedScan);
    interface.printf("%s: %i%%", "Scan speed", speedScan);
  }
  else // No parameters (GET mode)
  {
    interface.printf("Scan speed value is missing\n");
  }
} // setControlScan()

void getControlScan(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.printf("%s: %i%%", "Scan speed", speedScan);
} // getControlScan()

//! SYStem commands

void getSystemFirmware(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // get the firmware version
  char colFormat[20] = "%-15s %s\n";
  interface.printf("Version: %s\n", VERSION);
  interface.printf(colFormat, "Firmware", FW_DATE);
  interface.printf(colFormat, "index.html", HTML_DATE);
  interface.printf(colFormat, "script.js", SCRIPT_DATE);
  interface.printf(colFormat, "styles.css", STYLES_DATE);
} // getSystemFirmware()

void getSystemTemperature(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // get the temperature and humidity from the HTU21D sensor
  // if (!envSensor.begin()) // Initialize the HTU21D sensor
  // {
  //   notifyClients("scp~Error: HTU21D sensor.");
  //   return; // Exit if sensor initialization fails
  // }
  envSensor.read();                         // read the sensor data
  float tempC = envSensor.getTemperature(); // HTU21D temperature
  float tempF = 1.8 * tempC + 32;
  float humidity = envSensor.getHumidity(); // HTU21D humidity
  interface.printf("%-15s %.1f°C (%d°F)\n", "Temperature:", tempC, (int)round(tempF));
  interface.printf("%-15s %d%%\n", "Humidity:", (int)round(humidity));
} // getSystemEnvironment()

void getSystemVoltage(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // Get the supply voltage from the VM pin
  // Supply voltage is connected to resistor R2
  // The VM pin is connected to the voltage divider with R1 to ground
  // Compensate for non-linear ADC response
  int adc = analogRead(VM_PIN); // read the ADC value from the VM pin
  float voltage = 0;            // convert to voltage
  if (adc > 3000)
  {
    voltage = 0.0005 * adc + 1.0874;
  }
  else
  {
    voltage = 0.0008 * adc + 0.1372;
  }
  voltage = voltage * (R1 + R2) / R1; // convert to supply voltage
  interface.printf("%-15s %.1f V\n", "Voltage:", voltage);
} // getSystemVoltage()

void getSystemWebserver(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // get the web server state
  interface.printf("%-15s %s\n", "IP:", WiFi.localIP().toString().c_str());
  interface.printf("%-15s %i dBm\n", "RSSI:", WiFi.RSSI());
  interface.printf("%-15s %i\n", "Clients:", ws.count());
} // getSystemWebserver()

void getSystemList(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("---System List\n");
  getSystemTemperature(commands, parameters, interface); // get the enclosure temperature and humidity
  getSystemVoltage(commands, parameters, interface);     // get the supply voltage
  getSystemWebserver(commands, parameters, interface);   // get the web server state
  interface.print("---End System List---");
} // getSystemList()

void scpiBegin()
{
  preferences.begin("mag-loop", false); // false = open for read/write
  restorePreferences();

  // Configure I2C pins
  pinMode(SDA_PIN, INPUT);  // Set SDA pin to input with module pull-up resistor
  pinMode(SCL_PIN, INPUT);  // Set SCL pin to input with module pull-up resistor
  Wire.begin(SDA_PIN, SCL_PIN); // Initialize I2C with specified SDA and SCL pins

  if (!envSensor.begin()) // Initialize the HTU21D sensor
  {
    notifyClients("scp~Error: HTU21D sensor.");
  }

  //! SCPI Command Registration called in setup()
  scpi.hash_magic_number = 257; // set the hash magic number
  scpi.hash_magic_offset = 11;  // set the hash magic offset

  scpi.SetErrorHandler(scpiErrorHandler);                       // set the error handler
  scpi.RegisterCommand("*CLS", &instrumentClearStorage);        // clear the user preference settings stored in flash
  scpi.RegisterCommand("*IDN?", &instrumentIdentify);           // identify the device
  scpi.RegisterCommand("*RST", &instrumentReset);               // reset the device
  scpi.RegisterCommand("HELP?", &getHelp);                      // get the help text
  scpi.SetCommandTreeBase("CONtrol");                           // set the command tree base to CONtrol
  scpi.RegisterCommand(":DURation", &setControlDuration);       // set Duration of jog
  scpi.RegisterCommand(":DURation?", &getControlDuration);      // get Duration of jog
  scpi.RegisterCommand(":JOG", &setControlJog);                 // set Jog speed
  scpi.RegisterCommand(":JOG?", &getControlJog);                // get Jog speed
  scpi.RegisterCommand(":LISt?", &getControlList);              // get all control settings
  scpi.RegisterCommand(":PRESS", &setControlPress);             // long button press duration not implemented
  scpi.RegisterCommand(":PRESS?", &getControlPress);            // long button press duration not implemented
  scpi.RegisterCommand(":REPeat", &setControlRepeat);           // REPeat interval not implemented
  scpi.RegisterCommand(":REPeat?", &getControlRepeat);          // REPeat interval not implemented
  scpi.RegisterCommand(":SCAN", &setControlScan);               // set Scan speed
  scpi.RegisterCommand(":SCAN?", &getControlScan);              // get Scan speed
  scpi.SetCommandTreeBase("SYStem");                            // set the command tree base to SYStem
  scpi.RegisterCommand(":FIRMware?", &getSystemFirmware);       // get the system firmware list
  scpi.RegisterCommand(":LISt?", &getSystemList);               // get the system status list
  scpi.RegisterCommand(":TEMPerature?", &getSystemTemperature); // get the temp/humid inside enclosure
  scpi.RegisterCommand(":VOLTage?", &getSystemVoltage);         // get the system Voltage
  scpi.RegisterCommand(":WEBserver?", &getSystemWebserver);     // get the web server status

  // scpi.PrintDebugInfo(Serial); // print the debug info to the serial monitor
} // scpiBegin()

#endif // SCPI_H