//! @file DigitalSignalDetector.h
//! 2025-05-09

// by Perplexity AI & Karl W. Berger, W4KRL

/**
 * @class DigitalSignalDetector
 * @brief A class to detect digital signal changes (rising and falling edges) on a specified input pin.
 *
 * This class provides functionality to monitor a digital input pin and detect
 * transitions (rising and falling edges) as well as the current state of the pin.
 * 
 * @note Call the `update()` method in your main loop to refresh the state of the pin.
 *       Call the constructor with the pin number to initialize the pin mode in setup().
 */

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
