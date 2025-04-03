//! buttonHandler.h
//! 2025-04-03 revised message parsing for button events

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
// button 1 and 2 are latching buttons
// button 3 and 4 are momentary buttons
ButtonState buttonStates[] = {
    {"1", false, SCAN_UP},   // scan up
    {"2", false, SCAN_DOWN}, // scan down
    {"3", false, JOG_UP},    // jog up
    {"4", false, JOG_DOWN}}; // jog down

// ! Button state constants    
#define BTN1 0
#define BTN2 1
#define BTN3 2
#define BTN4 3

//! Send a button state update to all connected clients
void updateButtonState(int btnIndx)
{
  String id = buttonStates[btnIndx].id;
  ButtonState &button = buttonStates[btnIndx]; // Reference to the button state
  String message = "btn~" + id + "~";
  message += button.depressed ? "true~" + button.color : "false~" + String(UNPRESSED);
  notifyClients(message);
} // updateButtonState()

//! Send all button states to all clients
void initButtonStates()
{
  for (int i = 0; i < sizeof(buttonStates) / sizeof(buttonStates[0]); i++)
  {
    updateButtonState(i);
  }
} // initButtonStates()

//! Helper function to check if jog actions are allowed
bool isJogActionAllowed()
{
  return !buttonStates[BTN1].depressed && !buttonStates[1].depressed;
}

//! Call the action responses for all buttons
void processButtonEvent(String &buttonId, String &action)
{
  Serial.printf("Button rcvd: %s, Action: %s\n", buttonId.c_str(), action.c_str());
  // interlocking for latching buttons
  if ((buttonId == "1" && !buttonStates[BTN2].depressed))
  {
    actionScanUp();
  }
  else if ((buttonId == "2" && !buttonStates[BTN1].depressed))
  {
    actionScanDown();
  }
  // momentary buttons
  else if (buttonId == "3" && action == "pressed" && isJogActionAllowed())
  {
    actionJogUp(action);
  }
  else if (buttonId == "3" && action == "released")
  {
    buttonStates[BTN3].depressed = false;
    updateButtonState(BTN3);
  }
  else if (buttonId == "4" && action == "pressed" && isJogActionAllowed())
  {
    actionJogDown(action);
  }
  else if (buttonId == "4" && action == "released")
  {
    buttonStates[BTN4].depressed = false;
    updateButtonState(BTN4);
  }
  else if (buttonId == "3" && isJogActionAllowed())
  {
    actionJogUp(action);
  }
  else if (buttonId == "4" && isJogActionAllowed())
  {
    actionJogDown(action);
  }
} // buttonHandler()

#endif
