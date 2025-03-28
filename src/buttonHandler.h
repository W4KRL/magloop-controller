//! buttonHandler.h
// 2025-03-16 improved interlocking of btn 3 and 4
// 2025-03-18 comments expanded, actions renamed from action1, etc.
// 2025-03-19 added inactiveButtonColor
// 2025-03-20 added isJogActionAllowed()
// 2025-03-23 revised button colors
// 2025-03-28 changed to notifyClients, added initButtonStates()

#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <Arduino.h>
#include "actions.h"
#include "debug.h"
#include "ledControl.h"
#include "h_bridge.h"

void actionScanUp();                // latching button, pressed toggles .depressed
void actionScanDown();              // latching button
void actionJogUp(String &action);   // momentary button, action is "pressed" or "released"
void actionJogDown(String &action); // momentary button

//! Structure to hold button state information
struct ButtonState
{
  String id;
  bool depressed;
  String color;
};

//! Array to store the initial state of each button
//! Each entry includes: buttonId, initial state (depressed or not), and depressedColor (ON)
//! button 0 is not used - it's just there as an index marker
// button 1 and 2 are latching buttons
// button 3 and 4 are momentary buttons
ButtonState buttonStates[] = {
    {"btn0", false, UNPRESSED}, // not used
    {"btn1", false, SCAN_UP},   // scan up
    {"btn2", false, SCAN_DOWN}, // scan down
    {"btn3", false, JOG_UP},    // jog up
    {"btn4", false, JOG_DOWN}}; // jog down

void initButtonStates()
{
  for (int i = 0; i < sizeof(buttonStates) / sizeof(buttonStates[0]); i++)
  {
    notifyClients(buttonStates[i].id + "~" + buttonStates[i].color);
  }
} // initButtonStates()

//! Send a button state update to all connected clients
void updateButtonState(const String &buttonId)
{
  for (auto &button : buttonStates)
  {
    if (button.id == buttonId)
    {
      String message = "buttonState~" + buttonId + "~";
      message += button.depressed ? "true~" + button.color : "false~" + (String)UNPRESSED;
      notifyClients(message);
      break;
    }
  }
} // updateButtonState()

//! Helper function to check if jog actions are allowed
bool isJogActionAllowed()
{
  return !buttonStates[1].depressed && !buttonStates[2].depressed;
}

//! Call the action responses for all buttons
void buttonHandler(String &buttonId, String &action)
{
  // interlocking for latching buttons
  if ((buttonId == "btn1" && !buttonStates[2].depressed))
  {
    actionScanUp();
  }
  else if ((buttonId == "btn2" && !buttonStates[1].depressed))
  {
    actionScanDown();
  }
  else if (buttonId == "btn3" && isJogActionAllowed())
  {
    actionJogUp(action);
  }
  else if (buttonId == "btn4" && isJogActionAllowed())
  {
    actionJogDown(action);
  }
} // buttonHandler()

#endif
