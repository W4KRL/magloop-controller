//! ledControl.h
//! LED control functions
//! 2025-04-02 improved/simplified ledStates

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>
#include "credentials.h" // for LED colors
#include "webSocket.h" // for notifyClients()

//! LED indices
#define LED_UP 0
#define LED_DOWN 1

//! Structure to store an LED id and color
struct LedState
{
  String id;
  String color;
};

//! Array to store the id and initial state of each LED
LedState ledStates[] = {
    {"1", LED_GREEN},  // LED_UP = 'led~1
    {"2", LED_GREEN}}; // LED_DOWN = 'led~2'

//! Update LED state on all connected clients
void updateLedState(int ledIndex, const String &color)
{
  ledStates[ledIndex].color = color; // Update the color in the array
  notifyClients("led~" + ledStates[ledIndex].id + "~" + color);
} // updateLedState()

//! Initialize all LED states for newly connected clients
void initLedStates()
{
  for (int i = 0; i < sizeof(ledStates) / sizeof(ledStates[0]); i++)
  {
    updateLedState(i, ledStates[i].color);
  }
} // initLedStates()

#endif