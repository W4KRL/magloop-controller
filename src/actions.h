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
#include "credentials.h"    // for LED colors

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
 * @brief Handles the scanning action for a motor based on button state and limit switch input.
 * 
 * This function manages the motor's movement by checking the state of a button and a limit switch.
 * If the button is pressed and the limit switch is triggered, the motor is activated in the specified
 * direction and speed. If the button is released, the motor is stopped.
 * 
 * @param btnIndx Index of the button being checked.
 * @param moveDirection Direction of motor movement (e.g., forward or reverse).
 * @param speed Speed at which the motor should move.
 * @param limitSwitch Reference to a Bounce object representing the limit switch state.
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

  // Check if the limit switches are triggered
  if (limitSwitchUp.rose()) // Limit switch UP triggered
  {
    setMotorSpeed(NO_MOTION, IDLE);              // Stop the motor
    buttonStates[BTN_SCAN_UP].depressed = false; // Set Scan Up button to undepressed
    updateButtonState(BTN_SCAN_UP);              // Send websocket message
    updateLedState(LED_UP, LED_COLOR_RED);             // Set up limit LED to red
  } // if (limitSwitchUp.rose())

  if (limitSwitchDown.rose()) // Limit switch DOWN triggered
  {
    setMotorSpeed(NO_MOTION, IDLE);                // Stop the motor
    buttonStates[BTN_SCAN_DOWN].depressed = false; // Set Scan Down button to undepressed
    updateButtonState(BTN_SCAN_DOWN);              // Send websocket message
    updateLedState(LED_DOWN, LED_COLOR_RED);             // Set down limit LED to red
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