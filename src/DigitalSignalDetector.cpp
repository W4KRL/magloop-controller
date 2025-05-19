/**
 * @file DigitalSignalDetector.cpp
 * @brief Implementation of the DigitalSignalDetector class for detecting digital signal changes on a specified input pin.
 *
 * This class provides methods to monitor a digital input pin and detect rising and falling edges.
 * It maintains the current and previous state of the pin to determine if the signal has changed.
 *
 * @author Karl Berger
 * @date 2025-05-19
 */

#include "DigitalSignalDetector.h"

DigitalSignalDetector::DigitalSignalDetector(int inputPin)
    : pin(inputPin)
{
    pinMode(pin, INPUT);
    currentState = digitalRead(pin);
    previousState = currentState;
}

void DigitalSignalDetector::update()
{
    previousState = currentState;
    currentState = digitalRead(pin);
}

bool DigitalSignalDetector::rose() const
{
    return (previousState == LOW && currentState == HIGH);
}

bool DigitalSignalDetector::fell() const
{
    return (previousState == HIGH && currentState == LOW);
}

bool DigitalSignalDetector::read() const
{
    return currentState;
}
// end of file