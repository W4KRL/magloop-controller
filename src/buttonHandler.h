/**
 * @file buttonHandler.h
 * @brief Handles button state management and event processing for the magloop controller.
 *
 * This header defines the structures, enumerations, and function prototypes required to manage
 * the state of control buttons (latching and momentary) and to process their events. It provides
 * mechanisms to update button states, initialize them, and determine if certain actions (like jog)
 * are allowed based on current button states.
 *
 * Dependencies:
 * - Arduino.h: For Arduino-specific functions and types.
 * - actions.h: For actionScan() and actionJog() function declarations.
 * - configuration.h: For button color definitions.
 *
 * Structures:
 * - ButtonState: Holds the depressed state and color for each button.
 *
 * Enumerations:
 * - ButtonID: Identifiers for each button (scan up/down, jog up/down).
 *
 * Functions:
 * - updateButtonState(int btnIndx): Updates and broadcasts the state of a specific button.
 * - initButtonStates(): Initializes and broadcasts the state of all buttons.
 * - isJogActionAllowed(): Checks if jog actions are currently permitted.
 * - processButtonEvent(String &buttonId, String &action): Processes button events and triggers corresponding actions.
 */

#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <Arduino.h>       // for Arduino functions
#include "actions.h"       // for actionScan(), actionJog()
#include "configuration.h" // colors

/**
 * @brief Structure representing a button state and its associated color.
 *
 * @var depressed Indicates whether the button is currently pressed (true) or not (false).
 * @var color The color associated with the button, represented as a String.
 */
struct ButtonState
{
  bool depressed;
  String color;
};

/**
 * @brief Array of ButtonState structures representing the state and color of each button.
 *
 * The array contains the following buttons:
 * - btn0: Scan Up button, initialized as not pressed, with BTN_SCAN_UP_COLOR.
 * - btn1: Scan Down button, initialized as not pressed, with BTN_SCAN_DOWN_COLOR.
 * - btn2: Jog Up button, initialized as not pressed, with BTN_JOG_UP_COLOR.
 * - btn3: Jog Down button, initialized as not pressed, with BTN_JOG_DOWN_COLOR.
 */
ButtonState buttonStates[] = {
    {false, BTN_SCAN_UP_COLOR},   // btn0 scan up
    {false, BTN_SCAN_DOWN_COLOR}, // btn1 scan down
    {false, BTN_JOG_UP_COLOR},    // btn2 jog up
    {false, BTN_JOG_DOWN_COLOR}}; // btn3 jog down

/**
 * @enum ButtonID
 * @brief Identifies the different buttons available on the controller.
 *
 * Enumerates the possible button actions:
 * - BTN_SCAN_UP:    Button to initiate scanning upwards.
 * - BTN_SCAN_DOWN:  Button to initiate scanning downwards.
 * - BTN_JOG_UP:     Button to jog (move incrementally) upwards.
 * - BTN_JOG_DOWN:   Button to jog (move incrementally) downwards.
 */
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
