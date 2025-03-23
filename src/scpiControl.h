//! scpiControl.h
// 2025-03-23 removed motor commands, added ENVironment command

// Use '~' as a delimiter for web socket commands
// to avoid conflict with SCPI commands using ':' and ';' as delimiters

/*
  SCPI Commands:
  *CLS
    Clears the storage

  *IDN?
    Gets the instrument identification string

  *RST
    Clears preferences, restores defaults, restarts preocessor

  Help
  Help?
    Get a list of SCPI command

  ENVironment?
    Get the enclosure temperature and humidity

  SYStem:Dump?
    Lists the system state

  SYStem:Highspeed <value>
    Sets the high speed scan speed

  SYStem:Lowspeed <value>
    Sets the slow speed in percentage

  SYStem:Press <value>
    Sets the button long press time in milliseconds

  SYStem:Repeat <value>
    Sets the long press repeat interval in milliseconds

*/

#ifndef SCPICONTROL_H
#define SCPICONTROL_H

#include <Arduino.h>             // Required for platformio
#include "debug.h"               // DEBUG_PRINT, DEBUG_PRINTLN
#include <Vrekrer_scpi_parser.h> // https://github.com/Vrekrer/Vrekrer_scpi_parser
#include "h_bridge.h"            // Motor control
#include <Preferences.h>         // Store controller settings in flash with LittleFS
SCPI_Parser scpi;                // instantiate the parser
Preferences preferences;         // instantiate the Preferences storage
;                                // Declare global variables for preferences:
int speedHigh;                   // motor high speed % for scan
int speedLow;                    // motor low speed % for fine tune
int pressDuration;               // long button press duration ms
int jogDuration;                 // motor jog duration ms
int repeatInterval;              // jog repeat interval ms

void restorePreferences()
{
  speedHigh = preferences.getInt("speedHigh", 100);           // motor high speed % for scan
  speedLow = preferences.getInt("speedLow", 50);              // motor low speed % for fine tune
  pressDuration = preferences.getInt("pressDuration", 300);   // long press duration ms
  repeatInterval = preferences.getInt("repeatInterval", 200); // repeat interval ms
  jogDuration = preferences.getInt("jogDuration", 100);       // jog duration ms
}

//! Functions to handle SCPI commands for web sockets

/**
 * @class CaptureStream
 * @brief A custom implementation of the Stream class to capture SCPI command responses.
 *
 * This class is used to capture the output of SCPI commands executed by the SCPI parser.
 * Instead of sending the output to a hardware interface, it stores the output in a string
 * for further processing, such as sending it back to a client over a WebSocket.
 */
class CaptureStream : public Stream
{
  // written by Copilot AI
public:
  /// Captured output from the SCPI parser.
  String captured;

  /**
   * @brief Writes a single byte to the captured string.
   * @param c The byte to write.
   * @return Always returns 1 to indicate success.
   */
  virtual size_t write(uint8_t c)
  {
    captured += (char)c;
    return 1;
  }

  /**
   * @brief Indicates the number of bytes available for reading.
   * @return Always returns 0 as this stream is write-only.
   */
  virtual int available() { return 0; }

  /**
   * @brief Reads a byte from the stream.
   * @return Always returns -1 as this stream is write-only.
   */
  virtual int read() { return -1; }

  /**
   * @brief Peeks at the next byte in the stream without removing it.
   * @return Always returns -1 as this stream is write-only.
   */
  virtual int peek() { return -1; }
};

/**
 * @brief Processes an SCPI command received via WebSocket and sends the response back to the client.
 * 
 * @param client Pointer to the WebSocket client that sent the command.
 * @param command The SCPI command string received from the client.
 * 
 * This function captures the SCPI command response using a custom stream and sends it back to the client
 * if the response is not empty. It also logs the command and response for debugging purposes.
 */
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

void getEnvironment(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  // get the temperature and humidity
  interface.printf("Temperature: %i C\n", 25);
  interface.printf("Humidity: %i%%\n", 50);
} // getEnvironment()

void getHelp(SCPI_C commands, SCPI_P parameters, Stream &interface)
{
  interface.print("*IDN? device identity\n");
  interface.print("*CLS clears storage\n");
  interface.print("*RST resets processor, storage unaltered\n");
  interface.print("ENVironment?\n");
  interface.print("SYStem:DUMP?\n");
  interface.print("SYStem:HIGHspeed <80..100>\n");
  interface.print("SYStem:JOG <50..200>\n");
  interface.print("SYStem:LOWspeed <50..80>\n");
  interface.print("SYStem:PRESS <200..500>\n");
  interface.print("SYStem:REPeat <100..300>\n");
  interface.print("help? or help");
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
  scpi.RegisterCommand("ENVironment?", &getEnvironment);

  scpi.SetCommandTreeBase("SYStem");
  scpi.RegisterCommand(":DUMp", &getSystemDump);
  scpi.RegisterCommand(":DUMp?", &getSystemDump);
  scpi.RegisterCommand(":HIGHspeed", &setSystemHighspeed);
  scpi.RegisterCommand(":JOG", &setSystemJog);
  scpi.RegisterCommand(":LOWspeed", &setSystemLowspeed);
  scpi.RegisterCommand(":PREss", &setSystemPressDuration);
  scpi.RegisterCommand(":REPeat", &setSystemRepeatInterval);
} // scpiBegin()

#endif // SCPI_H