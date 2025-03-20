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
#define LED3 2
#define LED4 3
#define LED_UPon "lime"
#define LED_UPoff "red"
#define LED_DOWNon "lime"
#define LED_DOWNoff "red"
#define led3ON "skyBlue"
#define led3OFF "#202020"
#define led4ON "yellow"
#define led4OFF "#202020"

struct LedState
{
  String id;
  String color;
};

//! changed 2025-02-15 XXX
LedState ledStates[] = {
    {"led1", LED_UPon},
    {"led2", LED_DOWNoff},
    {"led3", led3OFF},
    {"led4", led4OFF}};

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
  DEBUG_PRINTF("%s: %s~%s","WS msg sent", ledId, color);
  ws.textAll(ledId + "~" + color);
} // updateLedState()

#endif