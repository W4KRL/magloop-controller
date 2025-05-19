
#include "actions.h" // for actionScan(), actionJog(), processLimitSwitches()

#include <Arduino.h>       // Required for basic Arduino functions
#include "buttonHandler.h" // for webSocket buttonStates[] and updateButtonState()
#include "h_bridge.h"      // for setMotorSpeed()
#include "configuration.h" // for LED colors
#include "ledControl.h"    // for colors
#include "scpiControl.h"   // for motor speed and direction

//! Instantiate DigitalSignalDetector objects for limit switches
DigitalSignalDetector limitSwitchUp(LIMIT_UP_PIN);
DigitalSignalDetector limitSwitchDown(LIMIT_DOWN_PIN);

void actionScan(int btnIndx, int moveDirection, int speed, DigitalSignalDetector &limitSwitch)
{
  if (buttonStates[btnIndx].depressed)
  {
    setMotorSpeed(NO_MOTION, NO_MOTION);
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
      // run the motor for jogDuration
    }
    setMotorSpeed(NO_MOTION, NO_MOTION);
  }
}

void processLimitSwitches()
{
  // Update the state of the limit switches
  limitSwitchUp.update();
  limitSwitchDown.update();

  // Check if the limit switches were triggered
  if (limitSwitchUp.rose()) // Limit switch UP was triggered
  {
    setMotorSpeed(NO_MOTION, NO_MOTION);                  // Stop the motor
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
    setMotorSpeed(NO_MOTION, NO_MOTION);                    // Stop the motor
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