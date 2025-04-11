//! h_bridge.h
//! 2025-04-11
//! using fast stop inverted logic
// see TI datasheet 7.3.1
// https://www.ti.com/lit/ds/symlink/l298.pdf

// ESP32 GPIOs DOIT ESP32 DEVKIT V1
// tested with DRV8871 module

//! test limit switches before call to setMotorSpeed()

#ifndef H_BRIDGE_H
#define H_BRIDGE_H

#include <Arduino.h>     // for Arduino functions
#include "credentials.h" // GPIO definitions

#define pwmFrequency 1000 // frequency in Hz
#define pwmResolution 8   // resolution in bits
#define IDLE 0            // IDLE state
#define MOVE_UP 1         // forward motion
#define MOVE_DOWN 2       // reverse motion

void h_bridgeBegin()
{
    // setup ESP32 PWM and connect to H-bridge inputs
    pinMode(HB_IN1, OUTPUT);
    pinMode(HB_IN2, OUTPUT);

    // Configure PWM settings for pin HB_IN1
    ledcAttach(HB_IN1, pwmFrequency, pwmResolution);
    ledcAttach(HB_IN2, pwmFrequency, pwmResolution);

    // Initialize pin states
    ledcWrite(HB_IN1, 255); // same as logic HIGH
    ledcWrite(HB_IN2, 255);
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
        ledcWrite(HB_IN1, 255);  // logic HIGH
        ledcWrite(HB_IN2, duty); // inverted PWM
        break;
    case MOVE_DOWN:
        ledcWrite(HB_IN1, duty); // inverted PWM
        ledcWrite(HB_IN2, 255);  // logic HIGH
        break;
    case IDLE:
        ledcWrite(HB_IN1, 255); // logic HIGH
        ledcWrite(HB_IN2, 255); // logic HIGH
        break;
    default:
        // direction is undefined - set to idle
        ledcWrite(HB_IN1, 255); // logic HIGH
        ledcWrite(HB_IN2, 255); // logic HIGH
    }
} // setMotorSpeedDirect()

#endif // H_BRIDGE_H