/**
 * @file h_bridge.h
 * @brief Header file for H-Bridge motor control functions.
 *
 * This file provides the interface for controlling a motor using an H-Bridge driver,
 * specifically referencing the TI DRV8871 datasheet (section 7.3.1).
 *
 * - Uses fast stop with inverted logic.
 * - Defines the Motion enum for specifying motor direction.
 * - Eliminates the IDLE state in favor of NO_MOTION.
 *
 * Functions:
 *   - h_bridgeBegin(): Initializes the H-Bridge hardware.
 *   - setMotorSpeed(int speed, int mode): Sets the motor speed and direction.
 *
 * @author Karl Berger
 * @date 2025-05-19
 */

#ifndef H_BRIDGE_H
#define H_BRIDGE_H

#include <Arduino.h> // for Arduino functions

extern const int NO_MOTION;
extern const int MOVE_UP;
extern const int MOVE_DOWN;

void h_bridgeBegin();
void setMotorSpeed(int speed, int mode);

#endif // H_BRIDGE_H