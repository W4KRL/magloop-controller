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
 * Functions:
 * - updateButtonState(int btnIndx): Updates and broadcasts the state of a specific button.
 * - initButtonStates(): Initializes and broadcasts the state of all buttons.
 * - isJogActionAllowed(): Checks if jog actions are currently permitted.
 * - processButtonEvent(String &buttonId, String &action): Processes button events and triggers corresponding actions.
 *
 * @author Karl Berger
 * @date 2025-05-20
 */

#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <Arduino.h>       // for String
#include "actions.h"       // for motor control actions
#include "configuration.h" // for button colors

// Structure representing a button state and its associated color
struct ButtonState
{
  bool depressed;
  String color;
};

extern ButtonState buttonStates[];
extern const int BTN_SCAN_UP;
extern const int BTN_SCAN_DOWN;
extern const int BTN_JOG_UP;
extern const int BTN_JOG_DOWN;

void updateButtonState(int btnIndx);
void initButtonStates();
bool isJogActionAllowed();
void processButtonEvent(String &buttonId, String &action);

#endif
