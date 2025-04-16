//! actions.h
// 2025-03-23 restored updateButtonState(), improved limit clear detection
// 2025-03-28 changed to notifyClients() if no change
// 2025-04-11 changed to debounced limit switches

#ifndef ACTIONS_H
#define ACTIONS_H

#include <Arduino.h>       // Required for basic Arduino functions like pinMode, digitalWrite, millis, etc., used throughout this file.
#include <Bounce2.h>       // for debouncing limit switches
#include "buttonHandler.h" // for webSocket buttonStates[] and updateButtonState()
#include "h_bridge.h"      // for setMotorSpeed()

//! LED color constants
#define LED_COLOR_GREEN LED_GREEN

//! LED color definitions
#define LED_COLOR_GREEN LED_GREEN

//! Instantiate Bounce objects for limit switches
Bounce limitSwitchUp = Bounce();   // Create a Bounce object for the UP limit switch
Bounce limitSwitchDown = Bounce(); // Create a Bounce object for the DOWN limit switch
#define DEBOUNCE_TIME 50           // Debounce time in milliseconds

//! Global variables
static bool ledBuiltIn = LOW; // Built-in LED LOW = OFF, HIGH = ON

//! Call in setup() to initialize the actions
/**
 * @brief Initializes the actions module by configuring the onboard LED and
 *        setting up the limit switches with debouncing.
 *
 * This function performs the following tasks:
 * - Configures the onboard LED pin (LED_BUILTIN) as an output and sets it to LOW (off).
 * - Configures the limit switch pins (LIMIT_UP and LIMIT_DOWN) as inputs with external pullups.
 * - Attaches the limit switches to debouncers and sets the debounce interval.
 *
 * Dependencies:
 * - The LED_BUILTIN macro must be defined for the onboard LED pin.
 * - The LIMIT_UP and LIMIT_DOWN macros must be defined for the limit switch pins.
 * - The DEBOUNCE_TIME macro must be defined for the debounce interval.
 */
void actionsBegin()
{
  // Configure onboard LED pin
  pinMode(LED_BUILTIN, OUTPUT);            // Set LED_BUILTIN as output
  digitalWrite(LED_BUILTIN, LOW);          // Start with LED off
  ;                                        // Assign limit switch up pin, attach debouncing
  pinMode(LIMIT_UP, INPUT);                // Use INPUT with external pullups
  limitSwitchUp.attach(LIMIT_UP);          // Attach limit switch to debouncer
  limitSwitchUp.interval(DEBOUNCE_TIME);   // Set debounce time
  ;                                        // Assign limit switch down pin, attach debouncing
  pinMode(LIMIT_DOWN, INPUT);              // Use INPUT with external pullups
  limitSwitchDown.attach(LIMIT_DOWN);      // Attach limit switch to debouncer
  limitSwitchDown.interval(DEBOUNCE_TIME); // Set debounce time
} // actionsBegin()

/**
 * @brief Sets the state of the built-in LED.
 *
 * This function updates the state of the built-in LED to the specified value.
 * It also updates the internal `ledBuiltIn` variable to reflect the current state.
 *
 * @param state A boolean value indicating the desired LED state:
 *              - `true` to turn the LED ON.
 *              - `false` to turn the LED OFF.
 */
void setLED_BUILTIN(bool state)
{
  ledBuiltIn = state;
  digitalWrite(LED_BUILTIN, ledBuiltIn);
}

/**
 * @brief Toggles the state of the built-in LED.
 *
 * This function inverts the current state of the built-in LED. If the LED is ON,
 * it will be turned OFF, and vice versa. The internal `ledState` variable is
 * updated to reflect the new state.
 */
void toggleLED_BUILTIN()
{
  ledBuiltIn = !ledBuiltIn;              // Invert the current state
  digitalWrite(LED_BUILTIN, ledBuiltIn); // Apply the new state
}

/**
 * @brief Handles motor control actions in response to button presses.
 *
 * The following functions manage motor movement and button states for specific actions:
 *
 * - `actionScanUp`: Toggles the motor to move upward at high speed when the UP button is pressed,
 *   and stops the motor when the button is toggled off or the UP limit switch is triggered.
 *
 * - `actionScanDown`: Toggles the motor to move downward at high speed when the DOWN button is pressed,
 *   and stops the motor when the button is toggled off or the DOWN limit switch is triggered.
 *
 * - `actionJogUp`: Moves the motor upward at low speed for a short duration when the UP jog button is pressed,
 *   and stops the motor when the button is released or the UP limit switch is triggered.
 *
 * - `actionJogDown`: Moves the motor downward at low speed for a short duration when the DOWN jog button is pressed,
 *   and stops the motor when the button is released or the DOWN limit switch is triggered.
 *
 * These functions ensure safe motor operation by checking limit switches and updating button states
 * and LED indicators accordingly.
 */

void actionScan(int btnIndx, int moveDirection, int speed, Bounce &limitSwitch)
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
  }
}

void actionJog(int btnIdx, int moveDirection, int speed, Bounce &limitSwitch, String &action)
{
  if (action == "pressed")
  {
    if (limitSwitch.read() == LOW)
    {
      setMotorSpeed(speed, moveDirection);
      buttonStates[btnIdx].depressed = true;
      updateButtonState(btnIdx);
      const long endTime = millis() + jogDuration;
      while (millis() < endTime)
      {
        // wait for jogDuration to jog motor
      }
      setMotorSpeed(NO_MOTION, IDLE);
    }
  }
  else
  {
    setMotorSpeed(NO_MOTION, IDLE);
    buttonStates[btnIdx].depressed = false;
    updateButtonState(btnIdx);
  }
}

/**
 * @brief Monitors the state of the limit switches and updates motor behavior and button states accordingly.
 *
 * This function should be called in the main loop. It checks the state of the UP and DOWN limit switches
 * to determine if the motor needs to stop or if the system state needs to be updated. When a limit switch
 * is triggered, the motor is stopped, the corresponding button state is reset, and the LED state is updated.
 *
 * Expected Behavior:
 * - If the UP limit switch is triggered, the motor stops moving forward, and the corresponding button and LED states are updated.
 * - If the DOWN limit switch is triggered, the motor stops moving in reverse, and the corresponding button and LED states are updated.
 * - If a limit switch is cleared, the corresponding LED state is updated to indicate the change.
 */

void processLimitSwitches()
{
  // Update the state of the limit switches
  limitSwitchUp.update();   // Update the state of the UP limit switch
  limitSwitchDown.update(); // Update the state of the DOWN limit switch

  // Check if the limit switches are triggered
  if (limitSwitchUp.rose()) // Limit switch UP triggered
  {
    setMotorSpeed(NO_MOTION, IDLE);              // Stop the motor
    buttonStates[BTN_SCAN_UP].depressed = false; // Set Scan Up button to undepressed
    updateButtonState(BTN_SCAN_UP);              // Send websocket message
    updateLedState(LED_UP, LED_RED);             // Set up limit LED to red
  } // if (limitSwitchUp.rose())

  if (limitSwitchDown.rose()) // Limit switch DOWN triggered
  {
    setMotorSpeed(NO_MOTION, IDLE);                // Stop the motor
    buttonStates[BTN_SCAN_DOWN].depressed = false; // Set Scan Down button to undepressed
    updateButtonState(BTN_SCAN_DOWN);              // Send websocket message
    updateLedState(LED_DOWN, LED_RED);             // Set down limit LED to red
  } // if (limitSwitchDown.rose())

  // Check if limit switches are cleared
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