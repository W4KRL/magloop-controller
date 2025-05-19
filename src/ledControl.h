//! @file ledControl.h
//! LED control functions for webSocket clients
//! 2025-05-10 use enum LedIndex for LED indices

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>
// #include "DigitalSignalDetector.h" // for limit switch detection

// extern DigitalSignalDetector limitSwitchUp;
// extern DigitalSignalDetector limitSwitchDown;

//! LED indices
// enum LedIndex
// {
//     LED_UP = 0,
//     LED_DOWN = 1
// };

extern String ledColor[2];
extern int LED_UP;
extern int LED_DOWN;

void updateLedState(int ledIndex, const String &color);
void initLedStates();

#endif