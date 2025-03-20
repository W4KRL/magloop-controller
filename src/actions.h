//! actions.h
// 2025-03-18 refactoring, actions renamed from action1, etc.

#ifndef ACTIONS_H
#define ACTIONS_H

#include "buttonHandler.h"
extern int jogDuration, speedLow, speedHigh, repeatInterval, pressDuration;
#include "h_bridge.h"

//! Global variables
static bool ledBuiltIn = LOW;             // Current state of the built-in LED
volatile bool limitUpTriggered = false;   // Flag for limit switch interrupt
volatile bool limitDownTriggered = false; // Flag for limit switch interrupt

//! Interrupt Service Routines for limit switches
void IRAM_ATTR handleLimitUp()
{
  limitUpTriggered = true; // Set flag
}

void IRAM_ATTR handleLimitDown()
{
  limitDownTriggered = true; // Set flag
}

void actionsBegin()
//! call this in setup()
{
  pinMode(LED_BUILTIN, OUTPUT);   // Set LED_BUILTIN as output
  digitalWrite(LED_BUILTIN, LOW); // Start with LED off
  pinMode(LIMIT_UP, INPUT);       // Set the UP limit switch as input
  pinMode(LIMIT_DOWN, INPUT);     // Set the DOWN limit switch as input
  // attach limit switch inputs to interrupt hamdlers
  // switches are normally closed, so they are LOW when not triggered
  attachInterrupt(digitalPinToInterrupt(LIMIT_UP), handleLimitUp, RISING);
  attachInterrupt(digitalPinToInterrupt(LIMIT_DOWN), handleLimitDown, RISING);
}

/**
 * @brief Sets the state of the built-in LED.
 *
 * This function updates the state of the built-in LED to the specified value.
 * It also updates the internal `ledState` variable to reflect the current state.
 *
 * @param state A boolean value indicating the desired LED state:
 *              - `true` to turn the LED ON.
 *              - `false` to turn the LED OFF.
 */
void setLED(bool state)
{
  // Set the LED to the desired state
  // LOW turns the LED off, and HIGH turns it on
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
void toggleLED()
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
  if (buttonStates[1].depressed) // toggle off
  {
    setMotorSpeedDirect(0, IDLE);
    buttonStates[1].depressed = false;
    updateButtonState("btn1");
  }
  else
  {
    if (!digitalRead(LIMIT_UP))
    {
      setMotorSpeedDirect(speedHigh, MOVE_UP);
      buttonStates[1].depressed = true; // toggle on
      updateButtonState("btn1");
    }
  }
}

void actionScanDown()
{
  if (buttonStates[2].depressed) // button is ON, turn it OFF
  {
    setMotorSpeedDirect(0, IDLE);
    buttonStates[2].depressed = false;
    updateButtonState("btn2");
  }
  else
  {
    if (!digitalRead(LIMIT_DOWN))
    {
      setMotorSpeedDirect(speedHigh, MOVE_DOWN);
      buttonStates[2].depressed = true;
      updateButtonState("btn2");
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
      buttonStates[3].depressed = true;
      updateButtonState("btn3");
      updateLedState("led3", led3ON);
      const long endTime = millis() + jogDuration;
      while (millis() < endTime)
      {
        // wait for jogDuration to jog motor
      }
      setMotorSpeedDirect(0, IDLE);
    }
  }
  else if (action == "released")
  {
    setMotorSpeedDirect(0, IDLE);
    buttonStates[3].depressed = false;
    updateButtonState("btn3");
    updateLedState("led3", led3OFF);
  }
}

void actionJogDown(String &action)
{
  if (action == "pressed")
  {
    if (!digitalRead(LIMIT_DOWN))
    {
      setMotorSpeedDirect(speedLow, MOVE_DOWN);
      buttonStates[4].depressed = true;
      updateButtonState("btn4");
      updateLedState("led4", led4ON);
      const long endTime = millis() + jogDuration;
      while (millis() < endTime)
      {
        // wait for jogDuration to jog motor
      }
      setMotorSpeedDirect(0, IDLE);
    }
  }
  else if (action == "released")
  {
    setMotorSpeedDirect(0, IDLE);
    buttonStates[4].depressed = false;
    updateButtonState("btn4");
    updateLedState("led4", led4OFF);
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
  if (limitUpTriggered)
  {
    setMotorSpeedDirect(0, IDLE); // Stop the motor
    buttonStates[1].depressed = false;
    updateButtonState("btn1");
    updateLedState("led1", LED_UPoff); // Set to red
    limitUpTriggered = false;        // Reset flag
  }

  if (limitDownTriggered)
  {
    setMotorSpeedDirect(0, IDLE); // Stop the motor
    buttonStates[2].depressed = false;
    updateButtonState("btn2");
    updateLedState("led2", LED_DOWNoff); // Set to red
    limitDownTriggered = false;      // Reset flag
  }

  if (motorDir == MOVE_DOWN && !digitalRead(LIMIT_UP) && ledStates[LED_UP].color == LED_UPoff)
  {
    updateLedState("led1", LED_UPon); // Set to green
  }

  if (motorDir == MOVE_UP && !digitalRead(LIMIT_DOWN) && ledStates[LED_DOWN].color == LED_DOWNoff)
  {
    updateLedState("led2", LED_DOWNon); // Set to green
  }
}

// void processLimitSwitches()
// {
//   //! call this in loop()
//   static boolean wasLimitUp = false;
//   static boolean wasLimitDown = false;

// forward
//   if (digitalRead(LIMIT_UP) && !wasLimitUp)
//   {
//     setMotorSpeedDirect(0, IDLE);
//     buttonStates[1].depressed = false;
//     updateButtonState("btn1");
//     updateLedState("led1", led1OFF);
//     wasLimitUp = true;
//   }

//   if (!digitalRead(LIMIT_UP) && wasLimitUp)
//   {
//     updateLedState("led1", led1ON);
//     wasLimitUp = false;
//   }

// reverse
// was moving when it hit limit
//   if (digitalRead(LIMIT_DOWN) && !wasLimitDown)
//   {
//     setMotorSpeedDirect(0, IDLE);
//     buttonStates[2].depressed = false;
//     updateButtonState("btn2");
//     updateLedState("led2", led2OFF);
//     wasLimitDown = true;
//   }
// had been at limit, now is clear
//   if (!digitalRead(LIMIT_DOWN) && wasLimitDown)
//     if (!digitalRead(LIMIT_DOWN) && wasLimitDown) // had been at limit, now is clear
//     {
//       updateLedState("led2", led2ON);
//       wasLimitDown = false;
//     }
// }

//******************** ************ */
//! create a random SWR value to demonstrate the gauge web socket

float swrValue()
{
  float swrValue = 1 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (9.0));
  return swrValue;
}

void processSWR()
{
  //! call this in loop()
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 5000)
  {
    String swrValueString = "SWR~" + String(swrValue(), 2);
    ws.textAll(swrValueString);
    lastTime = millis();
  }
}

#endif