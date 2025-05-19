//! @file h_bridge.h
//! 2025-05-10 use enum Motion for direction, eliminate IDLE

//! uses fast stop inverted logic
// see TI datasheet 7.3.1
// https://www.ti.com/lit/ds/symlink/drv8871.pdf

#ifndef H_BRIDGE_H
#define H_BRIDGE_H

#include <Arduino.h>       // for Arduino functions

enum Motion
{
    NO_MOTION = 0,
    MOVE_UP = 1,
    MOVE_DOWN = 2
};

void h_bridgeBegin();
void setMotorSpeed(int speed, int mode);

#endif // H_BRIDGE_H