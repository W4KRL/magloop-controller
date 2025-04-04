// main.cpp
// MagLoop-Controller firmware for ESP32 DevKit board
//! 2025-04-03

// !!!Save this file!!!
// "C:\Users\KarlB\OneDrive\Documents\PlatformIO\Projects\magloop-controller"

/*
This code is for the ESP32 DevKit board, in PlatformIO with Arduino framework 3.x.
It uses the WebSocket protocol to communicate with the web interface.
The web interface is defined by html, JavaScript and CSS in flash memory.
The device employs Standard Commands for Programmable Instruments
(SCPI; often pronounced "skippy") for command and control.
The delimiter for web socket commands is "~" to avoid conflict
with SCPI commands using ':' and ';' as delimiters.

Interrupts detect the vacuum motor reaching limits of travel and stop the motor
if it does. The limit switches are normally closed (NC) reed switches.
The interrupts are triggered when the limit switch is opened (NO) by the motor.
*/

/*
Copyright (C) 2025 Karl W. Berger, W4KRL
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define DEBUG_MAGLOOP //! uncomment for debug output to Serial Monitor

#include <Arduino.h>        // required by PlatformIO
#include "scpiControl.h"    // for SCPI commands
#include "actions.h"        // responses to button commands & sensors
#include "buttonHandler.h"  // for button control from web sockets
#include "credentials.h"    // for WiFi credentials
#include <ESPTelnet.h>      // for Telnet client
#include "debug.h"          // for debug print to Serial Monitor
#include "h_bridge.h"       // for motor control
#include "ledControl.h"     // for LED control by web sockets
#include "scpiControl.h"    // for SCPI commands
#include "webSocket.h"      // set up webSocket
#include "wifiConnection.h" // local WiFi
#include <ArduinoOTA.h>     // for OTA updates

ESPTelnet telnet; // Telnet client for serial input/output

//! Additional libraries called in local headers:
/*
    ESPAsyncWebServer.h {webSocket.h} https://github.com/ESP32Async/AsyncTCP
    Vrekrer_scpi_parser.h {scpiControl.h} https://github.com/Vrekrer/Vrekrer_scpi_parser
    Preferences.h {scpiControl.h} builtin to Arduino compiler
    littleFS.h {webSocket.h} builtin to Arduino compiler
    wifi.h {wifiConnection.h} builtin to Arduino compiler
*/

//! *************** Setup function ******************

void setup()
{
  Serial.begin(115200); // start Serial Monitor
  Serial.println("\nMagLoop Controller v1.1\n");

  if (telnet.begin(23))
  {
    Serial.println("Telnet server started on port 23");
  }
  else
  {
    Serial.println("Failed to start Telnet server");
  }

  actionsBegin();   // initialize sensors, limit switches, and actions
  wifiBegin();      // connect to WiFi
  initOTA();        // Initialize OTA updates
  scpiBegin();      // initialize SCPI parser, define commands, load Preferences
  h_bridgeBegin();  // initialize h-bridge for motor control
  websocketBegin(); // initialize webSocket for bi-directional communication
} // setup()

//! **************** Loop function ******************
void loop()
{

  ArduinoOTA.handle(); // Check for OTA updates

  //! Reconnect to Wi-Fi if disconnected
  if (!WiFi.isConnected())
  {
    wifiBegin();
  }

  //! Perform web client cleanup
  /*
    The cleanupClients() function is called periodically to free up
    resources by removing disconnected or inactive clients from
    the server's client list. Otherwise, you might encounter
    issues like resource leaks or memory exhaustion over time,
    particularly if clients connect and disconnect frequently.
  */
  unsigned long cleanInterval = 5000; // 5 seconds
  static unsigned long cleanTime = millis() + cleanInterval;
  if (millis() > cleanTime)
  {
    ws.cleanupClients();
    cleanTime = millis() + cleanInterval;
  }

  //! Push random SWR value to all clients every 5 seconds
  unsigned long swrInterval = 5000;
  static unsigned long swrTime = millis() + swrInterval;
  if (millis() > swrTime)
  {
    notifyClients(processSWR()); // send to all clients
    swrTime = millis() + swrInterval;
  }

  //! Shut down motor if limits of travel are reached
  processLimitSwitches();

  telnet.loop();

  // send serial input to telnet as output
  if (Serial.available())
  {
    telnet.print(Serial.read());
  }

} // loop()
