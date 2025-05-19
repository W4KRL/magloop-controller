
/**
 * @file buttonHandler.cpp
 * @brief Handles button state management and actions for the magloop controller.
 *
 * This file manages the state of physical or virtual buttons, updates their states,
 * communicates changes to connected clients, and triggers appropriate actions such as
 * scanning or jogging the motor. It also ensures that button actions are mutually exclusive
 * where necessary (e.g., jog actions are only allowed if scan buttons are not pressed).
 *
 * Key functionalities:
 * - Maintains an array of button states, including their pressed status and associated colors.
 * - Provides functions to update and broadcast button states to clients.
 * - Processes button events and triggers corresponding actions (scan or jog).
 * - Ensures safe operation by checking button dependencies (e.g., jog actions are blocked during scan).
 *
 * Dependencies:
 * - Arduino core for basic functions.
 * - Custom modules for actions, LED control, H-bridge motor control, limit switch detection,
 *   debugging, WebSocket communication, SCPI control, and configuration.
 *
 * @author Karl Berger
 * @date 2025-05-19
 */

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

//! Array to store the initial state of each button
//! Each entry includes: buttonId, initial state (depressed or not), and depressedColor (ON)
ButtonState buttonStates[] = {
    {false, BTN_SCAN_UP_COLOR},   // btn0 scan up
    {false, BTN_SCAN_DOWN_COLOR}, // btn1 scan down
    {false, BTN_JOG_UP_COLOR},    // btn2 jog up
    {false, BTN_JOG_DOWN_COLOR}}; // btn3 jog down

//! Button state constants
const int BTN_SCAN_UP = 0;
const int BTN_SCAN_DOWN = 1;
const int BTN_JOG_UP = 2;
const int BTN_JOG_DOWN = 3;

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