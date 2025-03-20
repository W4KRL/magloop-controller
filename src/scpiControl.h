//! scpiControl.h
// 2025-02-15
// eliminate setMotorDirectionDirect()
// fix typos

// Uses '~' as a delimiter for web socket commands
// to avoid conflict with SCPI commands using ':' and ';' as delimiters

/*
  SCPI Commands:
  *IDN?
    Gets the instrument identification string

  *RST
    Clears preferences, restores defaults, restarts preocessor

  HELP
  HELP?
    Get a list of SCPI command

  Motor:Direction <value>
    Sets the motor direction to <value>
    Valid values : Forward, Reverse, IDLEe

  Motor:Direction?
    Queries the current Motor mode

  Motor:Speed <value>
    Valid values : 0..100

  Motor:Speed?
    Queries the current Motor speed

  Motor:Condition?
    Queries the current motor's speed and direction
    Example response: Spd 30% Forward

  Example commands:

    mot:speed 60 - set motor speed to 60%
    mot:dir f set direction to forward
    mot:con? gets motor condition. Response is "Spd 60% Forward"

    commands may be cocatenated:
    mot:speed 60; mot:dir r
    sets speed to 60% reverse

  System:Dump?
    Lists the system state

  System:Highspeed <value>
    Sets the high speed scan speed

  System:Lowspeed <value>
    Sets the slow speed in percentage

  System:Press <value>
    Sets the button long press time in milliseconds

  System:Repeat <value>
    Sets the long press repeat interval in milliseconds

*/

#ifndef SCPICONTROL_H
#define SCPICONTROL_H

#include <Arduino.h>
#include "debug.h"
#include <Vrekrer_scpi_parser.h> // https://github.com/Vrekrer/Vrekrer_scpi_parser
#include "h_bridge.h"
#include <Preferences.h> // store controller settings in flash with LittleFS
;                        //
SCPI_Parser scpi;        // instantiate the parser
Preferences preferences; // instantiate the Preferences storage
;                        //
;                        // declare global variables for preferences
int speedHigh;           // motor high speed % for scan
int speedLow;            // motor low speed % for fine tune
int pressDuration;       // long button press duration ms
int jogDuration;         // motor jog duration ms
int repeatInterval;      // jog repeat interval ms

void restorePreferences()
{
  speedHigh = preferences.getInt("speedHigh", 100);           // motor high speed % for scan
  speedLow = preferences.getInt("speedLow", 50);              // motor low speed % for fine tune
  pressDuration = preferences.getInt("pressDuration", 300);   // long press duration ms
  repeatInterval = preferences.getInt("repeatInterval", 200); // repeat interval ms
  jogDuration = preferences.getInt("jogDuration", 100);       // jog duration ms
}

//! Functions to handle SCPI commands for web sockets

class CaptureStream : public Stream
{
  // written by Copilot AI
public:
  String captured;

  virtual size_t write(uint8_t c)
  {
    captured += (char)c;
    return 1;
  }

  virtual int available() { return 0; }
  virtual int read() { return -1; }
  virtual int peek() { return -1; }
};

void processSCPICommand(AsyncWebSocketClient *client, String command)
{
  // written by Copilot AI
  // enables integration of websockets with SCPI parser
  CaptureStream captureStream;

  char *commandCopy = new char[command.length() + 1];
  strcpy(commandCopy, command.c_str());
  DEBUG_PRINTF("%s: %s", "SCPI command", commandCopy);
  scpi.Execute(commandCopy, captureStream);
  delete[] commandCopy;

  if (captureStream.captured.length() > 0)
  {
    String response = "SCPI~" + captureStream.captured;
    DEBUG_PRINTF("%s: %s", "SCPI response", response.c_str());
    client->text(response);
  }
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

void getHelp(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("*IDN? device identity\n");
  interface.print("*CLS clears storage\n");
  interface.print("*RST resets processor, storage unaltered\n");
  interface.print("MOTor:CONdition?\n");
  interface.print("MOTor:DIRection <I|F|R>\n");
  interface.print("MOTor:DIRection?\n");
  interface.print("MOTor:SPEeed <50..100>\n");
  interface.print("MOTor:SPEeed?\n");
  interface.print("SYStem:DUMP?\n");
  interface.print("SYStem:HIGHspeed <80..100>\n");
  interface.print("SYStem:JOG <50..200>\n");
  interface.print("SYStem:LOWspeed <50..80>\n");
  interface.print("SYStem:PRESS <200..500>\n");
  interface.print("SYStem:REPeat <100..300>\n");
  interface.print("help? or help");
} // getHelp()

void setMotorSpeed(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  if (parameters.Size() > 0)
  {
    motorSpeed = String(parameters[0]).toInt();
    motorSpeed = constrain(motorSpeed, 0, 100);
    interface.printf("%s: %i%%", "Speed", motorSpeed);
    setMotorSpeedDirect(motorSpeed, motorDir);
  }
  else
  {
    interface.print("Speed missing");
  }
} // setMotorSpeed()

void getMotorSpeed(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.printf("%s: %i%%", "Speed", motorSpeed);
} // getMotorSpeed()

void setMotorDirection(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  String first_parameter = parameters.First();
  first_parameter.toUpperCase();

  if (first_parameter.startsWith("F"))
  {
    motorDir = MOVE_UP;
  }
  else if (first_parameter.startsWith("R"))
  {
    motorDir = MOVE_DOWN;
  }
  else
  { // anything except F or R
    // consider set duty = 0
    motorDir = IDLE;
  }
  setMotorSpeedDirect(motorSpeed, motorDir);
  interface.printf("%s: %s", "Dir: ", motorDirString[motorDir]);
} // setMotorDirection()

void getMotorDirection(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.printf("%s: %s", "Dir: ", motorDirString[motorDir]);
} // getMotorDirection()

void getMotorCondition(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.printf("%s: %i%% %s: %s", "SPD", motorSpeed, "DIR", motorDirString[motorDir]);
} // getMotorCondition()

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

void setSystemPressduration(SCPI_C commands, SCPI_P parameters, Stream &interface)
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
  //! SCPI Command Registration in setup()
  // define the scpi command structure
  // capitalized letters may be used as abbreviations
  // command case is irrelevant

  preferences.begin("mag-loop", false); // false = open for read/write

  restorePreferences();

  scpi.RegisterCommand("*CLS", &deviceClearStorage);
  scpi.RegisterCommand("*IDN?", &deviceIdentity);
  scpi.RegisterCommand("*RST", &deviceReset);
  scpi.RegisterCommand("HELP", &getHelp);
  scpi.RegisterCommand("HELP?", &getHelp);

  scpi.SetCommandTreeBase("MOTor");
  scpi.RegisterCommand(":DIRection", &setMotorDirection);
  scpi.RegisterCommand(":DIRection?", &getMotorDirection);
  scpi.RegisterCommand(":CONdition?", &getMotorCondition);
  scpi.RegisterCommand(":SPD", &setMotorSpeed);
  scpi.RegisterCommand(":SPD?", &getMotorSpeed);
  scpi.RegisterCommand(":SPEed", &setMotorSpeed);
  scpi.RegisterCommand(":SPEed?", &getMotorSpeed);

  scpi.SetCommandTreeBase("SYStem");
  scpi.RegisterCommand(":DUMp", &getSystemDump);
  scpi.RegisterCommand(":DUMp?", &getSystemDump);
  scpi.RegisterCommand(":HIGHspeed", &setSystemHighspeed);
  scpi.RegisterCommand(":JOG", &setSystemJog);
  scpi.RegisterCommand(":LOWspeed", &setSystemLowspeed);
  scpi.RegisterCommand(":PREss", &setSystemPressduration);
  scpi.RegisterCommand(":REPeat", &setSystemRepeatInterval);
} // scpiBegin()

#endif // SCPI_H