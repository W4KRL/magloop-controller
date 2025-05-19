#include "h_bridge.h"

#include "configuration.h" // GPIO definitions

// enum Motion
// {
//     NO_MOTION = 0,
//     MOVE_UP = 1,
//     MOVE_DOWN = 2
// };

const int pwmFrequency = 1000; // frequency in Hz
const int pwmResolution = 8;   // resolution in bits

void h_bridgeBegin()
{
    // setup ESP32 PWM and connect to H-bridge inputs
    pinMode(HB_IN1_PIN, OUTPUT);
    pinMode(HB_IN2_PIN, OUTPUT);

    // Configure PWM settings for pin HB_IN1
    ledcAttach(HB_IN1_PIN, pwmFrequency, pwmResolution);
    ledcAttach(HB_IN2_PIN, pwmFrequency, pwmResolution);

    // Initialize pin states
    ledcWrite(HB_IN1_PIN, 255); // same as logic HIGH
    ledcWrite(HB_IN2_PIN, 255);
} // h-bridgeBegin()

void setMotorSpeed(int speed, int mode)
{
    //! H-Bridge outputs (fast stop inverted logic):
    //    as recommended by TI datasheet
    //    Forward mode IN1 = HIGH, IN1 =  PWM
    //    Reverse mode IN1 =  PWM, IN2 = HIGH
    //    Idle    mode IN1 = HIGH, IN2 = HIGH

    //! NOTE: duty is inverted for inverted logic
    //!   pwm duty = 0 for full output!
    int duty = map(speed, 0, 100, 255, 0);

    switch (mode)
    {
    case MOVE_UP:
        ledcWrite(HB_IN1_PIN, 255);  // logic HIGH
        ledcWrite(HB_IN2_PIN, duty); // inverted PWM
        break;
    case MOVE_DOWN:
        ledcWrite(HB_IN1_PIN, duty); // inverted PWM
        ledcWrite(HB_IN2_PIN, 255);  // logic HIGH
        break;
    case NO_MOTION:
        ledcWrite(HB_IN1_PIN, 255); // logic HIGH
        ledcWrite(HB_IN2_PIN, 255); // logic HIGH
        break;
    default:
        // direction is undefined - set to NO_MOTION
        ledcWrite(HB_IN1_PIN, 255); // logic HIGH
        ledcWrite(HB_IN2_PIN, 255); // logic HIGH
    }
} // setMotorSpeedDirect()
