//! actions.h
//! 2024-05-09 change to DigitalSignalDetector.h from Bounce2.h

#ifndef ACTIONS_H
#define ACTIONS_H

#include <Arduino.h>               // Required for basic Arduino functions
#include "buttonHandler.h"         // for webSocket buttonStates[] and updateButtonState()
#include "h_bridge.h"              // for setMotorSpeed()
#include "credentials.h"           // for LED colors
#include "DigitalSignalDetector.h" // for limit switch detection

//! Instantiate DigitalSignalDetector objects for limit switches
DigitalSignalDetector limitSwitchUp(LIMIT_UP_PIN);
DigitalSignalDetector limitSwitchDown(LIMIT_DOWN_PIN);

/**
 * @brief Handles the scanning action for a motor based on button state and limit switch input.
 *
 * This function manages the motor's movement in a specified direction and speed
 * when a button is pressed, and stops the motor when the button is released.
 * It also ensures that the motor does not move if the limit switch is triggered.
 *
 * @param btnIndx The index of the button being monitored.
 * @param moveDirection The direction in which the motor should move.
 *                       Use predefined constants for direction (e.g., MOVE_UP,
 *                       MOVE_DOWN, NO_MOTION).
 * @param speed The speed at which the motor should move.
 *              Use predefined constants or values for speed.
 * @param limitSwitch A reference to a DigitalSignalDetector object representing the limit switch.
 *                    The limit switch prevents motor movement when triggered.
 */
void actionScan(int btnIndx, int moveDirection, int speed, DigitalSignalDetector &limitSwitch)
{
  if (buttonStates[btnIndx].depressed)
  {
    setMotorSpeed(NO_MOTION, IDLE);
    buttonStates[btnIndx].depressed = false;
    updateButtonState(btnIndx);
  }
  else
  {
    if (limitSwitch.read() == LOW)
    {
      setMotorSpeed(speed, moveDirection);
      buttonStates[btnIndx].depressed = true;
      updateButtonState(btnIndx);
    }
    else
    {
      buttonStates[btnIndx].color = BTN_DISABLED_COLOR;
      updateButtonState(btnIndx);
    }
  }
}

/**
 * @brief Handles the jog action for a motor based on button input and limit switch state.
 *
 * @param btnIdx Index of the button triggering the jog action.
 * @param moveDirection Direction of motor movement (e.g., forward or reverse).
 * @param speed Speed at which the motor should jog.
 * @param limitSwitch Reference to a DigitalSignalDetector object representing the limit switch.
 * @param action String indicating the button action ("pressed" or other states).
 *
 * This function checks the state of the limit switch and performs a jog action
 * if the button is pressed and the limit switch allows movement. The motor jogs
 * for a predefined duration (`jogDuration`) and then stops. If the button action
 * is not "pressed", the motor is stopped, and the button state is updated.
 */
void actionJog(int btnIndx, int moveDirection, int speed, DigitalSignalDetector &limitSwitch)
{
  if (limitSwitch.read() == LOW)
  {
    setMotorSpeed(speed, moveDirection);
    buttonStates[btnIndx].depressed = true;
    updateButtonState(btnIndx);
    const long endTime = millis() + jogDuration;
    while (millis() < endTime)
    {
      // wait for jogDuration to jog motor
    }
    setMotorSpeed(NO_MOTION, IDLE);
  }
}

/**
 * @brief Processes the state of the limit switches and updates the system accordingly.
 *
 * This function checks the state of the UP and DOWN limit switches, determines if they
 * have been triggered or cleared, and performs the necessary actions such as stopping
 * the motor, updating button states, sending websocket messages, and changing LED states.
 *
 * Actions performed:
 * - If a limit switch is triggered:
 *   - Stops the motor.
 *   - Sets the corresponding Scan button to undepressed.
 *   - Sends a websocket message to update the button state.
 *   - Sets the corresponding limit LED to red.
 * - If a limit switch is cleared:
 *   - Sets the corresponding limit LED to green.
 *
 * This function ensures that the system responds appropriately to changes in the state
 * of the limit switches, providing visual feedback through LEDs and maintaining proper
 * motor and button states.
 */
void processLimitSwitches()
{
  // Update the state of the limit switches
  limitSwitchUp.update();
  limitSwitchDown.update();

  // Check if the limit switches were triggered
  if (limitSwitchUp.rose()) // Limit switch UP was triggered
  {
    setMotorSpeed(NO_MOTION, IDLE);                       // Stop the motor
    buttonStates[BTN_SCAN_UP].depressed = false;          // Set Scan Up button to undepressed
    buttonStates[BTN_SCAN_UP].color = BTN_DISABLED_COLOR; // Set Scan button to disabled color
    updateButtonState(BTN_SCAN_UP);                       // Send websocket message
    buttonStates[BTN_JOG_UP].depressed = false;           // Set Jog Up button to undepressed
    buttonStates[BTN_JOG_UP].color = BTN_DISABLED_COLOR;  // Set Jog button to disabled color
    updateButtonState(BTN_JOG_UP);                        // Send websocket message
    updateLedState(LED_UP, LED_COLOR_RED);                // Set up limit LED to red
  } // if (limitSwitchUp.rose())

  if (limitSwitchDown.rose()) // Limit switch DOWN was triggered
  {
    setMotorSpeed(NO_MOTION, IDLE);                         // Stop the motor
    buttonStates[BTN_SCAN_DOWN].depressed = false;          // Set Scan Down button to undepressed
    buttonStates[BTN_SCAN_DOWN].color = BTN_DISABLED_COLOR; // Set Scan button to disabled color
    updateButtonState(BTN_SCAN_DOWN);                       // Send websocket message
    buttonStates[BTN_JOG_DOWN].depressed = false;           // Set Jog Down button to undepressed
    buttonStates[BTN_JOG_DOWN].color = BTN_DISABLED_COLOR;  // Set Jog button to disabled color
    updateButtonState(BTN_JOG_DOWN);                        // Send websocket message
    updateLedState(LED_DOWN, LED_COLOR_RED);                // Set down limit LED to red
  } // if (limitSwitchDown.rose())

  // Check if limit switches were cleared
  if (limitSwitchUp.fell()) // limit switch up cleared
  {
    updateLedState(LED_UP, LED_COLOR_GREEN); // Set limit up LED to green
  } // if (limitSwitchUp.fell())

  if (limitSwitchDown.fell()) // limit switch down cleared
  {
    updateLedState(LED_DOWN, LED_COLOR_GREEN); // Set limit down LED to green
  } // if (limitSwitchDown.fell())

} // processLimitSwitches()

//! create a random SWR value to demonstrate the gauge web socket
//! call this in loop()
String processSWR()
{
  float swrValue = 1 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (9.0));
  String swrValueString = "swr~" + String(swrValue, 2);
  return swrValueString;
}

#endif