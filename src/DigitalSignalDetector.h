

#ifndef DIGITAL_SIGNAL_DETECTOR_H
#define DIGITAL_SIGNAL_DETECTOR_H

#include <Arduino.h>

/**
 * @class DigitalSignalDetector
 * @brief Detects rising and falling edges on a digital input pin.
 *
 * The DigitalSignalDetector class monitors a specified digital input pin for state changes.
 * It detects transitions (rising and falling edges) and provides methods to query the current
 * state and whether an edge has occurred since the last update.
 *
 * Usage:
 *   - Instantiate with the desired pin number.
 *   - Call update() in the main loop to refresh the pin state.
 *   - Use rose(), fell(), and read() to query pin transitions and state.
 *
 * @note Call update() frequently (e.g., in the main loop) to ensure accurate edge detection.
 *
 * @author Karl Berger
 * @date 2025-05-19
 */
class DigitalSignalDetector
{
private:
    int pin;
    bool currentState;
    bool previousState;

public:
    explicit DigitalSignalDetector(int inputPin);

    void update();

    bool rose() const;
    bool fell() const;
    bool read() const;
};

#endif // DIGITAL_SIGNAL_DETECTOR_H
// end of file  
