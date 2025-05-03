//! scpiControl.h
//! 2025-04-03 revised message parsing for SCPI commands
//! 2025-04-04 revised SCPI command structure, remove stream class
//! 2025-04-11 removed SYStem tree

/*
  SCPI Commands:
  *CLS
    Clears the user preference settings

  *IDN?
    Returns the instrument identification

  *RST
    Resets the instrument to default state, retaining user settings.

  Help
  Help?
    Returns the list of available SCPI commands.

  CONtrol:All?
    Returns all motor control mode settings.

  CONtrol:Duration <value>
    Sets the jog mode duration in milliseconds.

  CONtrol:Jogspeed <value>
    Sets jog speed (low range) as a percentage (0–100).

  CONtrol:Pressduration <value>
    Sets th jog mode button long press duration in milliseconds.

  CONtrol:Repeatinterval <value>
    Sets the jog mode button repeat interval in milliseconds.

  CONtrol:Scanspeed <value>
    Sets scan speed (high range) as a percentage (0–100).

  SYStem:Dump?
    Returns the system state.

  SYStem:Environment?
    Returns the enclosure temperature and humidity.

  SYStem:Voltage?
    Returns the system supply voltage.

  SYStem:Webserver?
    Returns web server state:
    [IP address, port number, signal strength (RSSI), connected clients]
*/

#ifndef SCPICONTROL_H
#define SCPICONTROL_H

#include <Arduino.h>             // Required for platformIO
#include "credentials.h"         // for SCPI identification
#include "debug_magloop.h"       // DEBUG_PRINTF()
#include <Vrekrer_scpi_parser.h> // https://github.com/Vrekrer/Vrekrer_scpi_parser
#include <StreamString.h>        // for StreamString class in processSCPIcommand()
#include <ESPAsyncWebServer.h>   // https://github.com/ESP32Async/ESPAsyncWebServer for StreamString class
#include <Preferences.h>         // Store controller settings in flash with LittleFS
#include <Wire.h>                // for I2C communication
#include <SHT2x.h>               // for HTU21D temperature and humidity sensor
#include <WiFi.h>                // for WiFi functions like localIP() and RSSI()
#include "webSocket.h"

extern AsyncWebSocket ws;
;                                // Instantiations
SCPI_Parser scpi;                //   SCPI parser
Preferences preferences;         //   Preferences storage
;                                // Declare global variables for Preferences:
int speedScan;                   //   motor high speed % for scan
int speedJog;                    //   motor low speed % for fine tune
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
  speedScan = preferences.getInt("speedHigh", 100);           // motor high speed % for scan
  speedJog = preferences.getInt("speedLow", 50);              // motor low speed % for fine tune
  pressDuration = preferences.getInt("pressDuration", 300);   // long press duration ms
  repeatInterval = preferences.getInt("repeatInterval", 200); // repeat interval ms
  jogDuration = preferences.getInt("jogDuration", 100);       // jog duration ms
} // restorePreferences()

//! SCPI Command Functions
void instrumentClearStorage(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // reset to default parameters, processor not restarted
  preferences.clear();
  restorePreferences();
  interface.print("Parameters reset");
} // deviceClearStorage()

void instrumentIdentify(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // required by SCPI spec for device identification
  int bufferLength = 100;
  char buffer[bufferLength];
  snprintf(buffer, bufferLength, "%s, %s, %s, %s", MAKER, MODEL, SERIAL_NUMBER, VERSION);
  interface.print(buffer);
} // deviceIdentity()

void instrumentReset(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // processor restarted, system parameters retained
  interface.print("Restarting.");
  delay(1000);
  ESP.restart();
} // deviceReset()

void getSystemEnvironment(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // get the temperature and humidity from the HTU21D sensor
  envSensor.read();                         // read the sensor data
  float tempC = envSensor.getTemperature(); // HTU21D temperature
  float tempF = 1.8 * tempC + 32;
  float humidity = envSensor.getHumidity(); // HTU21D humidity
  interface.printf("Temperature: %.1f°C (%d°F)\n", tempC, (int)round(tempF));
  interface.printf("Humidity: %d%%\n", (int)round(humidity));
} // getEnvironment()

void getHelp(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("*IDN? device identity\n");
  interface.print("*CLS clears settings\n");
  interface.print("*RST reset MUC, settings unaltered\n");
  interface.print("SYS:Environment? get encl. temp & humidity\n");
  interface.print("SYS:Dump? get settings\n");
  interface.print("SYS:Scan <80..100> set scan speed in %\n");
  interface.print("SYS:Duration <50..200> oj duration in ms\n");
  interface.print("SYS:Jogspeed <50..80> set jog speed in %\n");
  interface.print("PRESS button long press <200..500>ms\n");
  interface.print("REPeat button <100..300>ms\n");
  interface.print("Help? or Help get this help\n");
  interface.print("---End Help---");
} // getHelp()

// system commands
void getSystemDump(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("System Dump\n");
  char colFormat[20] = "%-15s %s\n";
  interface.printf(colFormat, "Firmware", FW_DATE);
  interface.printf(colFormat, "index.html", HTML_DATE);
  interface.printf(colFormat, "script.js", SCRIPT_DATE);
  interface.printf(colFormat, "styles.css", STYLES_DATE);
  interface.printf("%-15s %i%%\n", "Scan speed", speedScan);
  interface.printf("%-15s %i%%\n", "Jog speed", speedJog);
  interface.printf("%-15s %i ms\n", "Jog duration", jogDuration);
  interface.printf("%-15s %i ms\n", "Press duration", pressDuration);
  interface.printf("%-15s %i ms\n", "Repeat interval", repeatInterval);
  interface.print("---End Dump---");
} // getDump()

void getSystemVoltage(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // get the supply voltage and RSSI
  interface.print("Supply voltage: ");
  // interface.printf("%i mV\n", ESP.getVcc());
} // getMeasure()

void getSystemWebserver(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // get the web server state
  interface.printf("IP:%s, %i dBm, %i client\n", WiFi.localIP().toString().c_str(), WiFi.RSSI(), ws.count());
} // getWebserver()

void setControlScanSpeed(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    speedScan = String(parameters[0]).toInt();
    interface.printf("%s: %i%%", "High speed", speedScan);
    preferences.putInt("speedHigh", speedScan);
  }
  else
  {
    interface.print("High speed missing");
  }
} // setHighspeed()

void setControlDuration(SCPI_C commands, SCPI_P parameters, Stream &interface)
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

void setControlJogSpeed(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    speedJog = String(parameters[0]).toInt();
    preferences.putInt("speedLow", speedJog);
    interface.printf("%s: %i%%", "Low speed", speedJog);
  }
  else
  {
    interface.print("Low speed missing");
  }
} // setLowspeed()

void setControlPressDuration(SCPI_C commands, SCPI_P parameters, Stream &interface)
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

void setControlRepeatInterval(SCPI_C commands, SCPI_P parameters, Stream &interface)
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
  scpi.RegisterCommand("*CLS", &instrumentClearStorage);
  scpi.RegisterCommand("*IDN?", &instrumentIdentify);
  scpi.RegisterCommand("*RST", &instrumentReset);
  scpi.RegisterCommand("DUMp", &getSystemDump);
  scpi.RegisterCommand("DUMp?", &getSystemDump);
  scpi.RegisterCommand("Help", &getHelp);
  scpi.RegisterCommand("Help?", &getHelp);
  scpi.SetCommandTreeBase("CONtrol");                           // set the command tree base to CONtrol
  scpi.RegisterCommand(":Scanspeed", &setControlScanSpeed);     // SCAN speed
  scpi.RegisterCommand(":Duration", &setControlDuration);       // DURation of JOG
  scpi.RegisterCommand(":Jogspeed", &setControlJogSpeed);       // JOG speed
  scpi.RegisterCommand(":Repeat", &setControlRepeatInterval);   // REPeat interval not implemented
  scpi.RegisterCommand(":Press", &setControlPressDuration);     // long button press duration not implemented
  scpi.SetCommandTreeBase("SYStem");                            // set the command tree base to SYStem
  scpi.RegisterCommand(":Environment?", &getSystemEnvironment); // set the command tree base to SYStem
  scpi.RegisterCommand(":Voltage?", &getSystemEnvironment);     // for testing
  scpi.RegisterCommand(":Webserver?", &getSystemWebserver);     // for testing
} // scpiBegin()

#endif // SCPI_H