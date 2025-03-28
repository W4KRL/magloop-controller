// main.cpp
// 2025-03-19 save this file
// added preferences save/restore, list all libraries
// moved code out of loop()
// 2025-03-28 changes fornotifyClients() and elimination of fileSystem.h

//! 2025-03-18 extensive changes for interupts, limit switches, actions
//! leds not going grey when limit is restored

// !!!Save this file!!!
//! MagLoop-Controller

// This code is for the ESP32 DevKit board, in PlatformIO with the Arduino framework
// It uses the WebSocket protocol to communicate with the web interface.
// The web interface is defined by html, JavaScript and CSS in flash memory.
// The device employs Standard Commands for Programmable Instruments
// (SCPI; often pronounced "skippy").
// The delimeter for web socket commands is "~" to avoid conflict
// with SCPI commands using ':' and ';' as delimiters.

// ToDo: Find a gauge that can have logarithmic scale
// ToDo: Make LEDs and buttons indicate motor limit reached
// ToDo: Move loop test of limit switches to h_bridge.h or actions.h
// ToDo: Use interrupts to handle limit switches.
// Specifically, detect when the motor reaches its travel limits using interrupts,
// and turn the "Command" button red to indicate the motor cannot move further.

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

#include <Arduino.h>        // required by PlatformIO

#include "actions.h"        // responses to button commands & sensors
#include "buttonHandler.h"  // for button control from web sockets
#include "credentials.h"    // for WiFi credentials
#include "debug.h"          // for debug print to Serial Monitor
// #include "fileSystem.h"     // LittleFS for html/js/css
#include "h_bridge.h"       // for motor control
#include "ledControl.h"     // for LED control by web sockets
#include "webSocket.h"      // set up webSocket
#include "scpiControl.h"    // for SCPI commands
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
  actionsBegin();       // initialize sensors and limit switches
  wifiBegin();          // connect to WiFi
  scpiBegin();          // initialize SCPI parser, define commands, load Preferences
  h_bridgeBegin();      // initialize h-bridge for motor control
  websocketBegin();     // initialize webSocket for bi-directional communication
} // setup()

//! **************** Loop function ******************
void loop()
{
  // reconnect to WiFi if disconnected
  if (!WiFi.isConnected())
  {
    wifiBegin();
  }

  //! Perform web client cleanup every second
  unsigned long cleanInterval = 1000; // 1 second
  static unsigned long endTime = millis() + cleanInterval;
  if (millis() > endTime)
  {
    ws.cleanupClients();
    endTime = millis() + cleanInterval;
  }

  //! push random SWR value to all clients every 5 seconds
  processSWR();

  //! shut down motor if limits of travel are reached
  processLimitSwitches();

} // loop()
