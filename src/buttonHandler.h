//! @file buttonHandler.h
//! 2025-05-10 change to DigitalSignalDetector.h from Bounce2.h

#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <Arduino.h>       // for Arduino functions
#include "actions.h"       // for actionScan(), actionJog()
#include "configuration.h" // colors
// #include "ledControl.h"            // for LED colors
// #include "h_bridge.h"              // for setMotorSpeed()
// #include "DigitalSignalDetector.h" // for limit switch detection
// #include "debug_magloop.h"         // for debug print to Serial Monitor
// #include "webSocket.h"             // for notifyClients()
// #include "scpiControl.h" // for user parameters

//! Structure to hold button state information
struct ButtonState
{
  bool depressed;
  String color;
};

//! Array to store the initial state of each button
//! Each entry includes: buttonId, initial state (depressed or not), and depressedColor (ON)
// button 0 and 1 are latching buttons
// button 2 and 3 are momentary buttons
ButtonState buttonStates[] = {
    {false, BTN_SCAN_UP_COLOR},   // btn0 scan up
    {false, BTN_SCAN_DOWN_COLOR}, // btn1 scan down
    {false, BTN_JOG_UP_COLOR},    // btn2 jog up
    {false, BTN_JOG_DOWN_COLOR}}; // btn3 jog down

//! Button state constants
enum ButtonID
{
  BTN_SCAN_UP = 0,
  BTN_SCAN_DOWN = 1,
  BTN_JOG_UP = 2,
  BTN_JOG_DOWN = 3
} ButtonID;

void updateButtonState(int btnIndx);                       //! Send a button state update to all connected clients
void initButtonStates();                                   //! Send all button states to all clients
bool isJogActionAllowed();                                 //! Helper function to check if jog actions are allowed
void processButtonEvent(String &buttonId, String &action); //! Call the action responses for all buttons

#endif
