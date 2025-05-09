//! @file DigitalSignalDetector.h
//! 2025-05-09

// by Perplexity AI & Karl W. Berger, W4KRL

#ifndef DIGITAL_SIGNAL_DETECTOR_H
#define DIGITAL_SIGNAL_DETECTOR_H

#include <Arduino.h>

/**
 * @class DigitalSignalDetector
 * @brief A class to detect digital signal changes (rising and falling edges) on a specified input pin.
 *
 * This class provides functionality to monitor a digital input pin and detect
 * transitions such as rising edges (LOW to HIGH) and falling edges (HIGH to LOW).
 * No debouncing is performed in this class, so it is assumed that the input signal is
 * clean and stable. The input pin should be connected to a digital signal source.
 *
 * @note This class is designed for use with Arduino or similar platforms.
 */
class DigitalSignalDetector
{
private:
  int pin;               ///< The pin number to monitor for digital signals.
  bool currentState;     ///< The current state of the digital signal (HIGH or LOW).
  bool previousState;    ///< The previous state of the digital signal (HIGH or LOW).

public:
  /**
   * @brief Constructor to initialize the DigitalSignalDetector.
   * 
   * @param inputPin The pin number to monitor for digital signals.
   * 
   * This constructor sets up the specified pin as an input and initializes
   * the current and previous states based on the initial reading of the pin.
   */
  DigitalSignalDetector(int inputPin);

  /**
   * @brief Updates the current and previous states of the digital signal.
   * 
   * This method should be called periodically to refresh the state of the
   * digital signal and detect any transitions.
   */
  void update();

  /**
   * @brief Checks if a rising edge (LOW to HIGH transition) occurred.
   * 
   * @return true if a rising edge was detected, false otherwise.
   */
  bool rose() const;

  /**
   * @brief Checks if a falling edge (HIGH to LOW transition) occurred.
   * 
   * @return true if a falling edge was detected, false otherwise.
   */
  bool fell() const;

  /**
   * @brief Reads the current state of the digital signal.
   * 
   * @return true if the signal is HIGH, false if it is LOW.
   */
  bool read() const;
};

#endif // DIGITAL_SIGNAL_DETECTOR_H
