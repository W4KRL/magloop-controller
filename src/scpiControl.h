//! scpiControl.h
//! 2025-04-03 revised message parsing for SCPI commands
//! 2025-04-04 revised SCPI command structure, remove stream class

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

  SYStem:Dump?
    Gets the system state

  SYStem:Highspeed <value>
    Sets the motor high speed scan speed in percentage

  SYStem:Lowspeed <value>
    Sets the motor slow speed jog in percentage

  SYStem:Press <value>
    Sets the motor jog time in milliseconds

  SYStem:Repeat <value> ! NOT IN USE !
    Sets the jog button long press repeat interval in milliseconds

*/

#ifndef SCPICONTROL_H
#define SCPICONTROL_H

#include <Arduino.h>             // Required for platformIO
#include "credentials.h"         // for SCPI identification
#include "debug.h"               // DEBUG_PRINT, DEBUG_PRINTLN
#include <Vrekrer_scpi_parser.h> // https://github.com/Vrekrer/Vrekrer_scpi_parser
#include <Preferences.h>         // Store controller settings in flash with LittleFS
;                                // Instantiations
SCPI_Parser scpi;                //   SCPI parser
Preferences preferences;         //   Preferences storage
;                                // Declare global variables for Preferences:
int speedHigh;                   //   motor high speed % for scan
int speedLow;                    //   motor low speed % for fine tune
int pressDuration;               //   long button press duration ms
int jogDuration;                 //   motor jog duration ms
int repeatInterval;              //   jog repeat interval ms

void restorePreferences()
{
  speedHigh = preferences.getInt("speedHigh", 100);           // motor high speed % for scan
  speedLow = preferences.getInt("speedLow", 50);              // motor low speed % for fine tune
  pressDuration = preferences.getInt("pressDuration", 300);   // long press duration ms
  repeatInterval = preferences.getInt("repeatInterval", 200); // repeat interval ms
  jogDuration = preferences.getInt("jogDuration", 100);       // jog duration ms
}

//! SCPI Command Functions
void deviceClearStorage(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // reset to default parameters, processor not restarted
  preferences.clear();
  restorePreferences();
  interface.print("Parameters reset");
}

void deviceIdentity(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // required by SCPI spec for device identification
  int bufferLength = 100;
  char buffer[bufferLength];
  snprintf(buffer, bufferLength, "%s, %s, %s, %s", MAKER, MODEL, SERIAL_NUMBER, VERSION);
  interface.print(buffer);
} // Identify()

void deviceReset(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // processor restarted, system parameters retained
  interface.print("Restarting.");
  ESP.restart();
}

void getEnvironment(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // get the temperature and humidity
  interface.printf("Temperature: %i C\n", 25);
  interface.printf("Humidity: %i%%\n", 50);
} // getEnvironment()

void getHelp(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("*IDN? device identity\n");
  interface.print("*CLS clears settings\n");
  interface.print("*RST resets processor, settings unaltered\n");
  interface.print("ENVironment? get enclosure temp & humidity\n");
  interface.print("SYStem:DUMP? get system settings\n");
  interface.print("SYStem:HIGHspeed set scan speed <80..100>%\n");
  interface.print("SYStem:JOGduration <50..200>ms\n");
  interface.print("SYStem:LOWspeed set jog speed <50..80>%\n");
  interface.print("SYStem:PRESS <200..500>ms\n");
  interface.print("SYStem:REPeat <100..300>ms\n");
  interface.print("Help? or Help");
} // getHelp()

// system commands
void getSystemDump(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("System Dump\n");
  interface.printf("%s:\t%s\n", "Firmware", FW_DATE);
  interface.printf("%s:\t%s\n", "index.html", HTML_DATE);
  interface.printf("%s:\t%s\n", "script.js", SCRIPT_DATE);
  interface.printf("%s:\t%s\n", "styles.css", STYLES_DATE);
  interface.printf("%s:\t%i%%\n", "High speed", speedHigh);
  interface.printf("%s:\t%i%%\n", "Low speed", speedLow);
  interface.printf("%s:\t%i ms\n", "Jog duration", jogDuration);
  interface.printf("%s:\t%i ms\n", "Press duration", pressDuration);
  interface.printf("%s:\t%i ms\n", "Repeat intrvl", repeatInterval);
  interface.print("---End Dump---");
} // SystemDump()

void setSystemHighspeed(SCPI_C commands, SCPI_P parameters, Stream &interface)
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
} // setSystemHighspeed()

void setSystemJog(SCPI_C commands, SCPI_P parameters, Stream &interface)
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
} // setSystemHighspeed()

void setSystemLowspeed(SCPI_C commands, SCPI_P parameters, Stream &interface)
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
} // setSystemLowspeed()

void setSystemPressDuration(SCPI_C commands, SCPI_P parameters, Stream &interface)
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
} // setButtonLongpress()

void setSystemRepeatInterval(SCPI_C commands, SCPI_P parameters, Stream &interface)
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
} // setButtonRepeat()

void scpiBegin()
{
  preferences.begin("mag-loop", false); // false = open for read/write
  restorePreferences();
  //! SCPI Command Registration in setup()
  // define the scpi command structure
  // capitalized letters may be used as abbreviations
  // command case is irrelevant
  // formal SCPI commands should have a command base and a subcommand
  // e.g. "SYStem:HIGHspeed" or "SYStem:LOWspeed"
  scpi.RegisterCommand("*CLS", &deviceClearStorage);
  scpi.RegisterCommand("*IDN?", &deviceIdentity);
  scpi.RegisterCommand("*RST", &deviceReset);
  scpi.RegisterCommand("DUMp", &getSystemDump);
  scpi.RegisterCommand("DUMp?", &getSystemDump);
  scpi.RegisterCommand("ENVironment?", &getEnvironment);
  scpi.RegisterCommand("HELP", &getHelp);
  scpi.RegisterCommand("HELP?", &getHelp);
  scpi.RegisterCommand("HIGHspeed", &setSystemHighspeed);// SCAN speed
  scpi.RegisterCommand("JOG", &setSystemJog); // DURation of JOG
  scpi.RegisterCommand("LOWspeed", &setSystemLowspeed); // JOG speed
  scpi.RegisterCommand("REPeat", &setSystemRepeatInterval); // REPeat interval not implemented
  scpi.RegisterCommand("PREss", &setSystemPressDuration); // long button press duration not implemented
  scpi.RegisterCommand("RSSI?", &getEnvironment); // for testing
  scpi.RegisterCommand("VOLTage?", &getEnvironment); // for testing
} // scpiBegin()

#endif // SCPI_H