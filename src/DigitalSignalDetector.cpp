//! @file DigitalSignalDetector.cpp
//! 2025-05-15
// by Perplexity AI & Karl W. Berger, W4KRL

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