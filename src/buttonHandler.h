//! buttonHandler.h
//! 2025-04-16 combined up/down functions

#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <Arduino.h>
#include "actions.h"
#include "ledControl.h"
#include "h_bridge.h" // for setMotorSpeed()

// Declare limitSwitches as extern variables to be used in actions.h
extern Bounce limitSwitchUp;
extern Bounce limitSwitchDown;

// function prototypes from actions.h
void actionScan(int btnIndx, int moveDirection, int speed, Bounce &limitSwitch);
void actionJog(int btnIdx, int moveDirection, int speed, Bounce &limitSwitch);

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
    {"1", false, BTN_SCAN_UP_COLOR},   // scan up
    {"2", false, BTN_SCAN_DOWN_COLOR}, // scan down
    {"3", false, BTN_JOG_UP_COLOR},    // jog up
    {"4", false, BTN_JOG_DOWN_COLOR}}; // jog down

// ! Button state constants
#define BTN_SCAN_UP 0
#define BTN_SCAN_DOWN 1
#define BTN_JOG_UP 2
#define BTN_JOG_DOWN 3

//! Send a button state update to all connected clients
void updateButtonState(int btnIndx)
{
  String id = buttonStates[btnIndx].id;
  ButtonState &button = buttonStates[btnIndx]; // Reference to the button state
  String message = "btn~" + id + "~";          // Prefix with "btn~" for button messages
  message += button.depressed ? "true~" + button.color : "false~" + String(BTN_UNPRESSED_COLOR);
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
  return !buttonStates[BTN_SCAN_UP].depressed && !buttonStates[BTN_SCAN_DOWN].depressed;
}

//! Call the action responses for all buttons
void processButtonEvent(String &buttonId, String &action)
{
  char buttonChar = buttonId.charAt(0); // Extract the first character as a char
  switch (buttonChar)
  {
  case '1':
    if (!buttonStates[BTN_SCAN_DOWN].depressed)
    {
      actionScan(BTN_SCAN_UP, MOVE_UP, speedHigh, limitSwitchUp);
    }
    break;
  case '2':
    if (!buttonStates[BTN_SCAN_UP].depressed)
    {
      actionScan(BTN_SCAN_DOWN, MOVE_DOWN, speedHigh, limitSwitchDown);
    }
    break;
  case '3':
    if (action == "pressed" && isJogActionAllowed())
    {
      actionJog(BTN_JOG_UP, MOVE_UP, speedLow, limitSwitchUp);
    }
    else if (action == "released")
    {
      buttonStates[BTN_JOG_UP].depressed = false;
      updateButtonState(BTN_JOG_UP);
    }
    break;
  case '4':
    if (action == "pressed" && isJogActionAllowed())
    {
      actionJog(BTN_JOG_DOWN, MOVE_DOWN, speedLow, limitSwitchDown);
    }
    else if (action == "released")
    {
      buttonStates[BTN_JOG_DOWN].depressed = false;
      updateButtonState(BTN_JOG_DOWN);
    }
    break;
  default:
    return; // Invalid button ID, exit the function
  }
} // processButtonEvent()

#endif
