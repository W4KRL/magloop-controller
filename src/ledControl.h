//! ledControl.h
//! LED control functions
// 2025-03-19
// removed AsyncTCP include
// revise colors add defines

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// Declare ws as an external variable defined elsewhere
extern AsyncWebSocket ws;

//! LED indices and ON/OFF colors
#define LED_UP 0
#define LED_DOWN 1
#define LED_UP_GREEN "lime"
#define LED_UP_RED "red"
#define LED_DOWN_GREEN "lime"
#define LED_DOWN_RED "red"

struct LedState
{
  String id;
  String color;
};

//! changed 2025-02-15 XXX
LedState ledStates[] = {
    {"led1", LED_UP_GREEN},
    {"led2", LED_DOWN_GREEN}};

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