/**
 * @file ledControl.cpp
 * @brief Implements LED control logic for the magloop controller, including state updates and client notifications via web sockets.
 *
 * This file provides functions to manage the state and color of two LEDs (UP and DOWN) based on limit switch readings.
 * It updates all connected clients with the current LED state using web sockets.
 *
 * - LED_UP and LED_DOWN represent the indices for the two LEDs.
 * - ledColor[] stores the current color of each LED.
 * - updateLedState() updates the color of a specific LED and notifies all clients.
 * - initLedStates() initializes the LED states based on the limit switch readings and updates all clients.
 *
 * Dependencies:
 * - Arduino.h: Arduino core functions.
 * - configuration.h: LED color definitions.
 * - webSocket.h: notifyClients() for client communication.
 * - actions.h: limitSwitchUp and limitSwitchDown objects.
 * - debug_magloop.h: Debug print macros.
 * - DigitalSignalDetector.h: Digital signal detection utilities.
 */

#include "ledControl.h" // for LED control by web sockets

#include "configuration.h" // for LED colors
#include "webSocket.h"     // for notifyClients()
#include "actions.h"       // for limitSwitchUp, limitSwitchDown
#include "debug_magloop.h" // for debug print to Serial Monitor
#include "DigitalSignalDetector.h"

int LED_UP = 0;
int LED_DOWN = 1;

//! Array to store the initial state of each LED
String ledColor[2] = {
    LED_COLOR_GREEN,  // LED_UP = 'led~0
    LED_COLOR_GREEN}; // LED_DOWN = 'led~1'

//! Update LED state on all connected clients
void updateLedState(int ledIndex, const String &color)
{
  ledColor[ledIndex] = color;                                       // Update the color in the array
  String message = "led~" + String(ledIndex) + "~" + String(color); // Create the message
  DEBUG_PRINTF("%s", message.c_str());                              // Debug print
  notifyClients(message);
} // updateLedState()

void initLedStates()
{
  if (limitSwitchUp.read()) // Limit switch up triggered
  {
    ledColor[LED_UP] = LED_COLOR_RED; // Set up limit LED to red
  }
  else
  {
    ledColor[LED_UP] = LED_COLOR_GREEN; // Set up limit LED to green
  }
  if (limitSwitchDown.read()) // Limit switch down triggered
  {
    ledColor[LED_DOWN] = LED_COLOR_RED; // Set down limit LED to red
  }
  else
  {
    ledColor[LED_DOWN] = LED_COLOR_GREEN; // Set down limit LED to green
  }
  for (int i = 0; i < sizeof(ledColor) / sizeof(ledColor[0]); i++)
  {
    updateLedState(i, ledColor[i]);
  }
} // initLedStates()