/**
 * @file ledControl.h
 * @brief Provides LED control functions for webSocket clients.
 *
 * This header defines functions and variables for controlling LEDs,
 * including updating their state and initializing them. It also
 * declares external variables for LED color and indices.
 *
 * @note As of 2025-05-10, the use of enum LedIndex for LED indices is suggested.
 *
 * @author Karl Berger
 * @date 2025-05-10
 */

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>

extern String ledColor[2];
extern int LED_UP;
extern int LED_DOWN;

void updateLedState(int ledIndex, const String &color);
void initLedStates();

#endif