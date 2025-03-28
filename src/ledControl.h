//! ledControl.h
//! LED control functions
// 2025-03-19
// removed AsyncTCP include
// revise colors add defines
// 2025-03-28 changed to notifyClients(), added initLedStates()

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>
#include "credentials.h"
// #include <ESPAsyncWebServer.h>
#include "webSocket.h"

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
  notifyClients(ledId + "~" + color);
} // updateLedState()

void initLedStates()
{
  for (int i = 0; i < sizeof(ledStates) / sizeof(ledStates[0]); i++)
  {
    notifyClients(ledStates[i].id + "~" + ledStates[i].color);
  }
} // initLedStates()

#endif