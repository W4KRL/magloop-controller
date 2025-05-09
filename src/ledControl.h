//! ledControl.h
//! LED control functions for webSocket clients
//! 2025-05-08 moved external functions to ledControl.h

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>
#include "credentials.h"           // for LED colors
#include "webSocket.h"             // for notifyClients()
#include "actions.h"               // for limitSwitchUp, limitSwitchDown
#include "DigitalSignalDetector.h" // for limit switch detection

extern DigitalSignalDetector limitSwitchUp;
extern DigitalSignalDetector limitSwitchDown;

//! LED indices
#define LED_UP 0
#define LED_DOWN 1

//! Array to store the initial state of each LED
String ledColor[2] = {
    {LED_COLOR_GREEN},  // LED_UP = 'led~0
    {LED_COLOR_GREEN}}; // LED_DOWN = 'led~1'

//! Update LED state on all connected clients
void updateLedState(int ledIndex, const String &color)
{
  ledColor[ledIndex] = color;                                       // Update the color in the array
  String message = "led~" + String(ledIndex) + "~" + String(color); // Create the message
  DEBUG_PRINTF("%s", message.c_str());                              // Debug print
  notifyClients(message);
} // updateLedState()

//! Initialize all LED states for newly connected clients
/**
 * @brief Initializes the LED states based on the status of the limit switches.
 *
 * This function checks the state of `limitSwitchUp` and `limitSwitchDown`
 * to determine the appropriate color for the corresponding LEDs. If a limit
 * switch is triggered, the associated LED is set to red; otherwise, it is set
 * to green. After determining the colors, the function updates the state of
 * all LEDs accordingly.
 *
 * LED States:
 * - LED_UP: Red if `limitSwitchUp` is triggered, Green otherwise.
 * - LED_DOWN: Red if `limitSwitchDown` is triggered, Green otherwise.
 *
 * The function iterates through all LEDs in the `ledColor` array and updates
 * their states using the `updateLedState` function.
 */
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

#endif