//! actions.h
// 2025-03-23 restored updateButtonState(), improved limit clear detection
// 2025-03-28 changed to notifyClients() if no change

#ifndef ACTIONS_H
#define ACTIONS_H

#include "buttonHandler.h"
#include "h_bridge.h"       // for setMotorSpeedDirect()

//! Global variables
static bool ledBuiltIn = LOW;               // Built-in LED LOW = OFF, HIGH = ON
volatile bool isLimitUpTriggered = false;   // Flag for limit switch interrupt
volatile bool isLimitDownTriggered = false; // Flag for limit switch interrupt

//! Interrupt Service Routines (ISRs)
// Triggered when capacitor has hit upper frequency limit
void IRAM_ATTR handleLimitUpTriggered()
{
  isLimitUpTriggered = true;
}

// Triggered when capacitor has hit lower frequency limit
void IRAM_ATTR handleLimitDownTriggered()
{
  isLimitDownTriggered = true;
}

//! Call in setup() to initialize the actions
void actionsBegin()
{
  // Configure LED pin
  pinMode(LED_BUILTIN, OUTPUT);   // Set LED_BUILTIN as output
  digitalWrite(LED_BUILTIN, LOW); // Start with LED off

  // Configure limit switch pins
  pinMode(LIMIT_UP, INPUT_PULLUP);   // Use INPUT_PULLUP for stable signals
  pinMode(LIMIT_DOWN, INPUT_PULLUP); // Use INPUT_PULLUP for stable signals

  // Attach interrupt handlers for limit switches
  // Limit switches are normally closed, triggering interrupts when opened (RISING)
  attachInterrupt(digitalPinToInterrupt(LIMIT_UP), handleLimitUpTriggered, RISING);
  attachInterrupt(digitalPinToInterrupt(LIMIT_DOWN), handleLimitDownTriggered, RISING);
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

void actionScanUp()
{
  if (buttonStates[BTN1].depressed) // toggle off
  {
    setMotorSpeedDirect(0, IDLE);
    buttonStates[BTN1].depressed = false;
    updateButtonState(BTN1);
  }
  else
  {
    if (!digitalRead(LIMIT_UP))
    {
      setMotorSpeedDirect(speedHigh, MOVE_UP);
      buttonStates[BTN1].depressed = true; // toggle on
      updateButtonState(BTN1);
    }
  }
}

void actionScanDown()
{
  if (buttonStates[BTN2].depressed)
  {
    setMotorSpeedDirect(0, IDLE);
    buttonStates[BTN2].depressed = false;
    updateButtonState(BTN2);
  }
  else
  {
    if (!digitalRead(LIMIT_DOWN))
    {
      setMotorSpeedDirect(speedHigh, MOVE_DOWN);
      buttonStates[BTN2].depressed = true;
      updateButtonState(BTN2);
    }
  }
}

void actionJogUp(String &action)
{
  if (action == "pressed")
  {
    if (!digitalRead(LIMIT_UP))
    {
      setMotorSpeedDirect(speedLow, MOVE_UP);
      buttonStates[BTN3].depressed = true;
      updateButtonState(BTN3);
      const long endTime = millis() + jogDuration;
      while (millis() < endTime)
      {
        // wait for jogDuration to jog motor
      }
      setMotorSpeedDirect(0, IDLE);
    }
  }
  else
  {
    setMotorSpeedDirect(0, IDLE);
    buttonStates[BTN3].depressed = false;
    updateButtonState(BTN3);
  }
}

void actionJogDown(String &action)
{
  if (action == "pressed")
  {
    if (!digitalRead(LIMIT_DOWN))
    {
      setMotorSpeedDirect(speedLow, MOVE_DOWN);
      buttonStates[BTN4].depressed = true;
      updateButtonState(BTN4);
      const long endTime = millis() + jogDuration;
      while (millis() < endTime)
      {
        // wait for jogDuration to jog motor
      }
      setMotorSpeedDirect(0, IDLE);
    }
  }
  else
  {
    setMotorSpeedDirect(0, IDLE);
    buttonStates[BTN4].depressed = false;
    updateButtonState(BTN4);
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
  if (isLimitUpTriggered)
  {
    setMotorSpeedDirect(0, IDLE);      // Stop the motor
    buttonStates[BTN1].depressed = false; // Reset the Scan Up button state
    updateButtonState(BTN1);         // Send websocket message
    updateLedState(LED_UP, LED_RED);   // Set to red
    isLimitUpTriggered = false;        // Reset flag to prevent repeated action
  } // if (isLimitUpTriggered)

  if (isLimitDownTriggered)
  {
    setMotorSpeedDirect(0, IDLE); // Stop the motor
    buttonStates[BTN2].depressed = false;
    updateButtonState(BTN2);
    updateLedState(LED_DOWN, LED_RED); // Set to red
    isLimitDownTriggered = false;    // Reset flag
  } // if (isLimitDownTriggered)

  // test if limit switches are restored
  if (!digitalRead(LIMIT_UP) && ledStates[LED_UP].color == LED_RED)
  {
    Serial.println("Limit switch UP cleared");
    updateLedState(LED_UP, LED_GREEN); // Set to green
  }

  if (!digitalRead(LIMIT_DOWN) && ledStates[LED_DOWN].color == LED_RED)
  {
    Serial.println("Limit switch DOWN cleared");
    updateLedState(LED_DOWN, LED_GREEN); // Set to green
  }
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