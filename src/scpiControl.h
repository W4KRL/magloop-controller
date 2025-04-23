//! scpiControl.h
//! 2025-04-03 revised message parsing for SCPI commands
//! 2025-04-04 revised SCPI command structure, remove stream class
//! 2025-04-11 removed SYStem tree

/*
  SCPI Commands:
  *CLS
    Clears the preference settings

  *IDN?
    Gets the instrument identification

  *RST
    Restarts processor, settings unaltered

  Help
  Help?
    Get the SCPI command list

  ENVironment?
    Get the enclosure temperature and humidity

  Dump?
    Gets the system state

  Highspeed <value>
    Sets the motor high speed scan speed in percentage

  Lowspeed <value>
    Sets the motor slow speed jog in percentage

  Press <value>
    Sets the motor jog time in milliseconds

  Repeat <value> ! NOT IN USE !
    Sets the jog button long press repeat interval in milliseconds

*/

#ifndef SCPICONTROL_H
#define SCPICONTROL_H

#include <Arduino.h>             // Required for platformIO
#include "credentials.h"         // for SCPI identification
#include "debug.h"               // DEBUG_PRINTF()
#include <Vrekrer_scpi_parser.h> // https://github.com/Vrekrer/Vrekrer_scpi_parser
#include <StreamString.h>        // for StreamString class in processSCPIcommand()
#include <ESPAsyncWebServer.h>   // https://github.com/ESP32Async/ESPAsyncWebServer for StreamString class
#include <Preferences.h>         // Store controller settings in flash with LittleFS
#include <Wire.h>                // for I2C communication
#include <SHT2x.h>               // for HTU21D temperature and humidity sensor
;                                // Instantiations
SCPI_Parser scpi;                //   SCPI parser
Preferences preferences;         //   Preferences storage
;                                // Declare global variables for Preferences:
int speedHigh;                   //   motor high speed % for scan
int speedLow;                    //   motor low speed % for fine tune
int pressDuration;               //   long button press duration ms
int jogDuration;                 //   motor jog duration ms
int repeatInterval;              //   jog repeat interval ms
extern Bounce limitSwitchUp;
extern Bounce limitSwitchDown;

void notifyClients(const String &message); // webSocket.h prototype

//! Instantiate temperature and humidity sensor object
SHT2x envSensor; // HTU21D temperature and humidity sensor

String processSCPIcommand(String scpiCommand)
{
  // process the incoming SCPI command
  // scpi.Execute() returns true if a command was executed
  // interface is the stream to send the response to
  // StreamString inherits from ESPAsyncWebServer::Stream

  char scpiCommandBuf[scpiCommand.length() + 1];                   // Create a buffer for the command
  scpiCommand.toCharArray(scpiCommandBuf, sizeof(scpiCommandBuf)); // Copy to buffer
  StreamString responseStream;                                     // Create a string to capture the response
  String scpiResponse;                                             // Initialize response string
  scpi.Execute(scpiCommandBuf, responseStream);                    // Execute SCPI command
  scpiResponse = "scp~" + responseStream;                          // format for JavaScript client
  return scpiResponse;                                             // return the response string
} // processSCPIcommand()

void restorePreferences()
{
  speedHigh = preferences.getInt("speedHigh", 100);           // motor high speed % for scan
  speedLow = preferences.getInt("speedLow", 50);              // motor low speed % for fine tune
  pressDuration = preferences.getInt("pressDuration", 300);   // long press duration ms
  repeatInterval = preferences.getInt("repeatInterval", 200); // repeat interval ms
  jogDuration = preferences.getInt("jogDuration", 100);       // jog duration ms
} // restorePreferences()

//! SCPI Command Functions
void deviceClearStorage(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // reset to default parameters, processor not restarted
  preferences.clear();
  restorePreferences();
  interface.print("Parameters reset");
} // deviceClearStorage()

void deviceIdentity(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // required by SCPI spec for device identification
  int bufferLength = 100;
  char buffer[bufferLength];
  snprintf(buffer, bufferLength, "%s, %s, %s, %s", MAKER, MODEL, SERIAL_NUMBER, VERSION);
  interface.print(buffer);
} // deviceIdentity()

void deviceReset(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // processor restarted, system parameters retained
  interface.print("Restarting.");
  delay(1000);
  ESP.restart();
} // deviceReset()

void getEnvironment(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // get the temperature and humidity from the HTU21D sensor
  envSensor.read(); // read the sensor data
  float tempC = envSensor.getTemperature(); // HTU21D temperature
  float tempF = 1.8 * tempC + 32;
  float humidity = envSensor.getHumidity(); // HTU21D humidity
  interface.printf("Temperature: %.1f °C (%.1f °F)\n", tempC, tempF);
  interface.printf("Humidity: %.1f%%\n", humidity);
} // getEnvironment()

void getHelp(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("*IDN? device identity\n");
  interface.print("*CLS clears settings\n");
  interface.print("*RST reset MUC, settings unaltered\n");
  interface.print("ENVironment? get encl. temp & humidity\n");
  interface.print("DUMp? or DUMp get settings\n");
  interface.print("HIGHspeed set scan speed <80..100>%\n");
  interface.print("JOGduration <50..200>ms\n");
  interface.print("LOWspeed set jog speed <50..80>%\n");
  interface.print("PRESS button long press <200..500>ms\n");
  interface.print("REPeat button <100..300>ms\n");
  interface.print("Help? or Help get this help\n");
  interface.print("---End Help---");
} // getHelp()

// system commands
void getDump(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("System Dump\n");
  char colFormat[20] = "%-15s %s\n";
  interface.printf(colFormat, "Firmware", FW_DATE);
  interface.printf(colFormat, "index.html", HTML_DATE);
  interface.printf(colFormat, "script.js", SCRIPT_DATE);
  interface.printf(colFormat, "styles.css", STYLES_DATE);
  interface.printf("%-15s %i%%\n", "High speed", speedHigh);
  interface.printf("%-15s %i%%\n", "Low speed", speedLow);
  interface.printf("%-15s %i ms\n", "Jog duration", jogDuration);
  interface.printf("%-15s %i ms\n", "Press duration", pressDuration);
  interface.printf("%-15s %i ms\n", "Repeat interval", repeatInterval);
  interface.print("---End Dump---");
} // getDump()

void setHighspeed(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    speedHigh = String(parameters[0]).toInt();
    interface.printf("%s: %i%%", "High speed", speedHigh);
    preferences.putInt("speedHigh", speedHigh);
  }
  else
  {
    interface.print("High speed missing");
  }
} // setHighspeed()

void setJog(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    jogDuration = String(parameters[0]).toInt();
    preferences.putInt("jogDuration", jogDuration);
    interface.printf("%s: %i ms", "Jog duration", jogDuration);
  }
  else
  {
    interface.print("Jog duration missing");
  }
} // setJog()

void setLowspeed(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    speedLow = String(parameters[0]).toInt();
    preferences.putInt("speedLow", speedLow);
    interface.printf("%s: %i%%", "Low speed", speedLow);
  }
  else
  {
    interface.print("Low speed missing");
  }
} // setLowspeed()

void setPressDuration(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    pressDuration = String(parameters[0]).toInt();
    preferences.putInt("pressDuration", pressDuration);
    interface.printf("%s: %i ms", "Press duration", pressDuration);
  }
  else
  {
    interface.print("Press duration missing");
  }
} // setPressDuration()

void setRepeatInterval(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    repeatInterval = String(parameters[0]).toInt();
    preferences.putInt("repeatInterval", repeatInterval);
    interface.printf("%s: %i ms", "Repeat Interval", repeatInterval);
  }
  else
  {
    interface.print("Repeat interval missing");
  }
} // seSystemRepeatInterval()

void scpiBegin()
{
  preferences.begin("mag-loop", false); // false = open for read/write
  restorePreferences();

  // Configure I2C pins
  pinMode(SDA, INPUT);  // Set SDA pin to input with module pull-up resistor
  pinMode(SCL, INPUT);  // Set SCL pin to input with module pull-up resistor
  Wire.begin(SDA, SCL); // Initialize I2C with specified SDA and SCL pins
  envSensor.begin();    // Initialize the HTU21D sensor

  //! SCPI Command Registration in setup()
  // define the scpi command structure
  // capitalized letters may be used as abbreviations
  // command case is irrelevant
  // formal SCPI commands should have a command base and a subcommand
  // e.g. "HIGHspeed" or "LOWspeed"
  scpi.RegisterCommand("*CLS", &deviceClearStorage);
  scpi.RegisterCommand("*IDN?", &deviceIdentity);
  scpi.RegisterCommand("*RST", &deviceReset);
  scpi.RegisterCommand("DUMp", &getDump);
  scpi.RegisterCommand("DUMp?", &getDump);
  scpi.RegisterCommand("ENVironment?", &getEnvironment);
  scpi.RegisterCommand("Help", &getHelp);
  scpi.RegisterCommand("Help?", &getHelp);
  scpi.RegisterCommand("HIGHspeed", &setHighspeed);   // SCAN speed
  scpi.RegisterCommand("JOG", &setJog);               // DURation of JOG
  scpi.RegisterCommand("LOWspeed", &setLowspeed);     // JOG speed
  scpi.RegisterCommand("REPeat", &setRepeatInterval); // REPeat interval not implemented
  scpi.RegisterCommand("PREss", &setPressDuration);   // long button press duration not implemented
  scpi.RegisterCommand("RSSI?", &getEnvironment);     // for testing
  scpi.RegisterCommand("VOLTage?", &getEnvironment);  // for testing
} // scpiBegin()

#endif // SCPI_H