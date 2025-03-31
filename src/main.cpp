// main.cpp
// MagLoop-Controller firmware for ESP32 DevKit board
// added preferences save/restore, list all libraries
// 2025-03-28 changes for notifyClients() and elimination of fileSystem.h

//! 2025-03-18 extensive changes for interupts, limit switches, actions

// !!!Save this file!!!
//! MagLoop-Controller

// This code is for the ESP32 DevKit board, in PlatformIO with the Arduino framework
// It uses the WebSocket protocol to communicate with the web interface.
// The web interface is defined by html, JavaScript and CSS in flash memory.
// The device employs Standard Commands for Programmable Instruments
// (SCPI; often pronounced "skippy").
// The delimeter for web socket commands is "~" to avoid conflict
// with SCPI commands using ':' and ';' as delimiters.

// Specifically, detect when the motor reaches its travel limits using interrupts,
// and stop the motor if it does. The limit switches are normally closed (NC) contacts.
// The interrupts are triggered when the limit switch is opened (NO) by the motor.

/*
  Add this to platformio.ini for SCPI parser & file system:
    monitor_filters = send_on_enter
    monitor_echo = yes
    monitor_eol = LF
    board_build.filesystem = littlefs

*/

/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#define DEBUG_MAGLOOP //! uncomment for debug output to Serial Monitor

#include <Arduino.h>       // required by PlatformIO
#include "scpiControl.h"   // for SCPI commands
#include "actions.h"       // responses to button commands & sensors
#include "buttonHandler.h" // for button control from web sockets
#include "credentials.h"   // for WiFi credentials
#include "debug.h"         // for debug print to Serial Monitor
#include "h_bridge.h"      // for motor control
#include "ledControl.h"    // for LED control by web sockets
// #include "scpiControl.h"    // for SCPI commands
#include "webSocket.h"      // set up webSocket
#include "wifiConnection.h" // local WiFi

//! Additional libraries called in local headers:
/*
    ESPAsyncWebServer.h {webSocket.h, ledControl.h} https://github.com/ESP32Async/AsyncTCP
    Vrekrer_scpi_parser.h {scpiControl.h} https://github.com/Vrekrer/Vrekrer_scpi_parser
    Preferences.h {scpiControl.h} builtin to Arduino compiler
*/

//! *************** Setup function ******************

void setup()
{
  Serial.begin(115200); // start Serial Monitor
  actionsBegin();       // initialize sensors, limit switches, and actions
  wifiBegin();          // connect to WiFi
  scpiBegin();          // initialize SCPI parser, define commands, load Preferences
  h_bridgeBegin();      // initialize h-bridge for motor control
  websocketBegin();     // initialize webSocket for bi-directional communication
} // setup()

//! **************** Loop function ******************
void loop()
{
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

} // loop()
