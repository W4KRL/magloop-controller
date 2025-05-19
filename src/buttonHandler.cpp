#include "buttonHandler.h"

#include <Arduino.h>               // for Arduino functions
#include "actions.h"               // for actionScan(), actionJog()
#include "ledControl.h"            // for LED colors
#include "h_bridge.h"              // for setMotorSpeed()
#include "DigitalSignalDetector.h" // for limit switch detection
#include "debug_magloop.h"         // for debug print to Serial Monitor
#include "webSocket.h"             // for notifyClients()
#include "scpiControl.h"           // for user parameters
#include "configuration.h"         // for colors and designations

//! Structure to hold button state information
// struct ButtonState
// {
//   bool depressed;
//   String color;
// };

// //! Array to store the initial state of each button
// //! Each entry includes: buttonId, initial state (depressed or not), and depressedColor (ON)
// // button 0 and 1 are latching buttons
// // button 2 and 3 are momentary buttons
// ButtonState buttonStates[] = {
//     {false, BTN_SCAN_UP_COLOR},   // btn0 scan up
//     {false, BTN_SCAN_DOWN_COLOR}, // btn1 scan down
//     {false, BTN_JOG_UP_COLOR},    // btn2 jog up
//     {false, BTN_JOG_DOWN_COLOR}}; // btn3 jog down

// //! Button state constants
// enum ButtonID
// {
//   BTN_SCAN_UP = 0,
//   BTN_SCAN_DOWN = 1,
//   BTN_JOG_UP = 2,
//   BTN_JOG_DOWN = 3
// } ButtonID;

//! Send a button state update to all connected clients
void updateButtonState(int btnIndx)
{
  // String id = buttonStates[btnIndx].id;
  ButtonState &button = buttonStates[btnIndx];     // Reference to the button state
  String message = "btn~" + String(btnIndx) + "~"; // Prefix with "btn~" for button messages
  message += button.depressed ? "true~" + button.color : "false~" + String(BTN_UNPRESSED_COLOR);
  DEBUG_PRINTF("%s", message.c_str());
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
  int buttonIndex = buttonChar - '0';   // Convert char to index (0-3)

  switch (buttonIndex)
  {
  case BTN_SCAN_UP:
    if (!buttonStates[BTN_SCAN_DOWN].depressed)
    {
      actionScan(BTN_SCAN_UP, MOVE_UP, speedScan, limitSwitchUp);
    }
    break;
  case BTN_SCAN_DOWN:
    if (!buttonStates[BTN_SCAN_UP].depressed)
    {
      actionScan(BTN_SCAN_DOWN, MOVE_DOWN, speedScan, limitSwitchDown);
    }
    break;
  case BTN_JOG_UP:
    if (action == "pressed" && isJogActionAllowed())
    {
      actionJog(BTN_JOG_UP, MOVE_UP, speedJog, limitSwitchUp);
    }
    else if (action == "released")
    {
      buttonStates[BTN_JOG_UP].depressed = false;
      updateButtonState(BTN_JOG_UP);
    }
    break;
  case BTN_JOG_DOWN:
    if (action == "pressed" && isJogActionAllowed())
    {
      actionJog(BTN_JOG_DOWN, MOVE_DOWN, speedJog, limitSwitchDown);
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