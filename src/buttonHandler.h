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
#include "ledControl.h"

// function prototypes from actions.h
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
    {"1", false, SCAN_UP},   // scan up
    {"2", false, SCAN_DOWN}, // scan down
    {"3", false, JOG_UP},    // jog up
    {"4", false, JOG_DOWN}}; // jog down

//! Send a button state update to all connected clients
void updateButtonState(int btnIndx)
{
  String id = buttonStates[btnIndx].id;
  ButtonState &button = buttonStates[btnIndx]; // Reference to the button state
  String message = "btn~" + id + "~";
  message += button.depressed ? "true~" + button.color : "false~" + String(UNPRESSED);
  notifyClients(message);
} // updateButtonState()

void initButtonStates()
{
  for (int i = 0; i < sizeof(buttonStates) / sizeof(buttonStates[0]); i++)
  {
    updateButtonState(i); // Send initial states to all clients
  }
} // initButtonStates()

//! Helper function to check if jog actions are allowed
bool isJogActionAllowed()
{
  return true; // Allow jog actions by default
  // return !buttonStates[0].depressed && !buttonStates[1].depressed;
}

//! Call the action responses for all buttons
void buttonHandler(String &buttonId, String &action)
{
  Serial.printf("Button rcvd: %s, Action: %s\n", buttonId.c_str(), action.c_str());
  // interlocking for latching buttons
  if ((buttonId == "btn1" && !buttonStates[1].depressed))
  {
    actionScanUp();
  }
  else if ((buttonId == "btn2" && !buttonStates[0].depressed))
  {
    actionScanDown();
  }
  else if (buttonId == "btn3" && isJogActionAllowed())
  {
    actionJogUp(action);
    Serial.printf("Btn3 rcvd: %s, Action: %s\n", buttonId.c_str(), action.c_str());
  }
  else if (buttonId == "btn4" && isJogActionAllowed())
  {
    actionJogDown(action);
  }
} // buttonHandler()

#endif
