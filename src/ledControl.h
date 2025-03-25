//! ledControl.h
//! LED control functions
// 2025-03-19
// removed AsyncTCP include
// revise colors add defines

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>
#include "credentials.h"
#include <ESPAsyncWebServer.h>

// Declare ws as an external variable defined elsewhere
extern AsyncWebSocket ws;

//! LED indices and colors
// see credentials.h for colors
#define LED_UP 0
#define LED_DOWN 1

struct LedState
{
  String id;
  String color;
};

//! changed 2025-02-15 XXX
LedState ledStates[] = {
    {"led1", LED_GREEN},
    {"led2", LED_GREEN}};

void updateLedState(const String &ledId, const String &color)
{
  for (auto &led : ledStates)
  {
    if (led.id == ledId)
    {
      led.color = color;
      break;
    }
  }
  // Broadcast the update to all clients
  DEBUG_PRINTF("%s: %s~%s", "WS msg sent", ledId, color);
  ws.textAll(ledId + "~" + color);
} // updateLedState()

#endif