//! ledControl.h
//! LED control functions
//! 2025-04-16 revised LED colors and names

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>
#include "credentials.h" // for LED colors
#include "webSocket.h"   // for notifyClients()

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
  ledColor[ledIndex] = color; // Update the color in the array
  notifyClients("led~" + String(ledIndex) + "~" + String(color)); // Prefix with "led~" for LED messages
} // updateLedState()

//! Initialize all LED states for newly connected clients
void initLedStates()
{
  for (int i = 0; i < sizeof(ledColor) / sizeof(ledColor[0]); i++)
  {
    updateLedState(i, ledColor[i]);
  }
} // initLedStates()

#endif