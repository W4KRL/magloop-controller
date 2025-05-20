/**
 * @file main.cpp
 * @brief MagLoop-Controller firmware for ESP32 DevKit board.
 *
 * This firmware manages the operation of a magnetic loop antenna controller using an ESP32 DevKit board.
 * It provides WiFi connectivity, OTA updates, web socket communication, SCPI command parsing, motor control,
 * limit switch monitoring, SWR calculation, and LED control.
 *
 * Included Libraries:
 * - Arduino.h: Core Arduino functions.
 * - actions.h: Handles button commands and sensor responses.
 * - ArduinoOTA.h: Enables Over-The-Air firmware updates.
 * - buttonHandler.h: Manages button control via web sockets.
 * - configuration.h: Stores WiFi credentials and configuration.
 * - debug_magloop.h: Provides debug output to the Serial Monitor.
 * - DigitalSignalDetector.h: Handles limit switch control.
 * - h_bridge.h: Controls the motor via an H-bridge.
 * - ledControl.h: Manages LED indicators via web sockets.
 * - scpiControl.h: Parses and executes SCPI commands.
 * - swrCalc.h: Reads and calculates Standing Wave Ratio (SWR).
 * - webSocket.h: Sets up web socket communication.
 * - wifiConnection.h: Manages WiFi connection.
 *
 * Setup Function:
 * - Initializes serial communication, WiFi, OTA updates, web sockets, SCPI parser, and motor control.
 *
 * Loop Function:
 * - Maintains WiFi connection, handles OTA updates, cleans up web clients, processes limit switches,
 *   and updates SWR values for connected clients.
 *
 * @author Karl Berger
 * @date 2025-05-20
 */

#define DEBUG_MAGLOOP //! uncomment for debug output to Serial Monitor

#include <Arduino.h>               // required by PlatformIO
#include "actions.h"               // responses to button commands & sensors
#include "ArduinoOTA.h"            // for ArduinoOTA.handle() in loop()
#include "buttonHandler.h"         // for button control from web sockets
#include "configuration.h"         // for WiFi credentials
#include "debug_magloop.h"         // for debug print to Serial Monitor
#include "DigitalSignalDetector.h" // for limit switch control
#include "h_bridge.h"              // for motor control
#include "ledControl.h"            // for LED control by web sockets
#include "scpiControl.h"           // for SCPI commands
#include "swrCalc.h"               // for SWR read and calculate
#include "webSocket.h"             // set up webSocket
#include "wifiConnection.h"        // local WiFi#include <ArduinoOTA.h>     // for OTA updates

//! Additional libraries called in local headers:
/*
    ArduinoOTA.h {wifiConnection.h}       builtin to Arduino compiler
    ESPAsyncWebServer.h {webSocket.h}     https://github.com/ESP32Async/AsyncTCP
    littleFS.h {webSocket.h}              builtin to Arduino compiler
    Preferences.h {scpiControl.h}         builtin to Arduino compiler
    SHT2x.h {scpiControl.h}               https://github.com/RobTillaart/SHT2x
    Vrekrer_scpi_parser.h {scpiControl.h} https://github.com/Vrekrer/Vrekrer_scpi_parser
    wifi.h {wifiConnection.h}             builtin to Arduino compiler
    Wire.h {actions.h}                    builtin to Arduino compiler
*/

//! *************** Setup function ******************

void setup()
{
  Serial.begin(115200); // Initialize Serial Monitor at 115200 baud
  wifiBegin();          // Setup WiFi
  wifiConnect();        // Connect to WiFi
  otaBegin();           // Initialize Over-The-Air update service
  websocketBegin();     // Initialize webSocket for bi-directional communication with web UI
  scpiBegin();          // Initialize SCPI parser, define commands, load user Preferences
  h_bridgeBegin();      // Initialize h-bridge for motor control
} // setup()

//! **************** Loop function ******************

void loop()
{
  wifiConnect();          // Reconnect to Wi-Fi if disconnected
  ArduinoOTA.handle();    // Check for OTA updates
  websocketCleanup();     // Perform web client cleanup
  processLimitSwitches(); // Shut down motor if limits of travel are reached
  swrUpdate();            // Push random SWR value to all clients every 5 seconds
} // loop()
