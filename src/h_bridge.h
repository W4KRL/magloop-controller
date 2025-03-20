//! h_bridge.h
//! 2025-03-10
//! using fast stop inverted logic
// see TI datasheet 7.3.1
// https://www.ti.com/lit/ds/symlink/l298.pdf

// ESP32 GPIOs DOIT ESP32 DEVKIT V1
// tested with DRV8871 module

#ifndef H_BRIDGE_H
#define H_BRIDGE_H

#include <Arduino.h>
#include "credentials.h" // GPIO definitions

#define pwmFrequency 1000 // frequency in Hz
#define pwmResolution 8   // resolution in bits
#define IDLE 0            // IDLE state
#define MOVE_UP 1         // forward motion
#define MOVE_DOWN 2       // reverse motion

String motorDirString[] = {"Idle", "Move Up", "Move Down"};

int motorDir = IDLE; // motor direction
int motorSpeed = 0; // motor velocity

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

void setMotorSpeedDirect(int speed, int direction)
{
    motorDir = direction; // bag to let limit switches know motor direction
    //! H-Bridge outputs (fast stop inverted logic):
    //    as recommended by TI datasheet
    //    Forward mode IN1 = HIGH, IN1 =  PWM
    //    Reverse mode IN1 =  PWM, IN2 = HIGH
    //    IDLEe    mode IN1 = HIGH, IN2 = HIGH

    //! NOTE: duty is inverted for inverted logic
    //!   pwm duty = 0 for full output!
    int duty = map(speed, 0, 100, 255, 0);

    //! enable output to motor if it is not at a limit
    switch (direction)
    {
    case MOVE_UP:
        if (!digitalRead(LIMIT_UP)) // not at upper limit
        {
            ledcWrite(HB_IN1, 255); // logic HIGH
            ledcWrite(HB_IN2, duty);
        }
        break;
    case MOVE_DOWN:
        if (!digitalRead(LIMIT_DOWN)) // not at lower limit
        {
            ledcWrite(HB_IN1, duty);
            ledcWrite(HB_IN2, 255); // logic HIGH
        }
        break;
    default:
        // direction is IDLE or undefined
        ledcWrite(HB_IN1, 255); // logic HIGH
        ledcWrite(HB_IN2, 255); // logic HIGH
    }
} // setMotorSpeedDirect()

#endif // H_BRIDGE_H