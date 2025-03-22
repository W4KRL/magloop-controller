//! buttonHandler.h
// 2025-03-16 improved interlocking of btn 3 and 4
// 2025-03-18 comments expanded, actions renamed from action1, etc.
// 2025-03-19 added inactiveButtonColor
// 2025-03-20 added isJogActionAllowed()

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
    {"btn0", false, "dimGray"}, // not used
    {"btn1", false, "skyblue"}, // scan up
    {"btn2", false, "lime"},    // scan down
    {"btn3", false, "red"},     // jog up
    {"btn4", false, "yellow"}}; // jog down

//! Color of all buttons when they are not pressed
String inactiveButtonColor = "dimGray";

//! Send a button state update to all connected clients
void updateButtonState(const String &buttonId)
{
  for (auto &button : buttonStates)
  {
    if (button.id == buttonId)
    {
      String message = "buttonState~" + buttonId + "~";
      message += button.depressed ? "true~" + button.color : "false~" + inactiveButtonColor;
      DEBUG_PRINTF("%s: %s", "WS msg sent", message.c_str());
      // Broadcast the update to all clients
      ws.textAll(message);
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
