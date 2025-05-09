//! @file DigitalSignalDetector.h
//! 2025-05-09

// by Perplexity AI & Karl W. Berger, W4KRL

#ifndef DIGITAL_SIGNAL_DETECTOR_H
#define DIGITAL_SIGNAL_DETECTOR_H

#include <Arduino.h>

class DigitalSignalDetector
{
private:
  int pin;
  bool currentState;
  bool previousState;

public:
  DigitalSignalDetector(int inputPin)
  {
    pin = inputPin;
    pinMode(pin, INPUT);
    currentState = static_cast<bool>(digitalRead(pin));
    previousState = currentState;
  }

  void update()
  {
    previousState = currentState;
    currentState = static_cast<bool>(digitalRead(pin));
  }

  bool rose() const
  {
    return (previousState == LOW && currentState == HIGH);
  }

  bool fell() const
  {
    return (previousState == HIGH && currentState == LOW);
  }

  bool read() const
  {
    return currentState;
  }
};

#endif // DIGITAL_SIGNAL_DETECTOR_H
