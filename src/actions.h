/**
 * @file actions.h
 * @brief Declarations for action functions and limit switch handling in the magloop controller project.
 *
 * This header defines interfaces for scanning and jogging actions, as well as processing limit switches and SWR.
 * It also declares external references to limit switch detectors.
 *
 * Functions:
 * - actionScan: Performs a scanning action in a specified direction and speed, monitoring a limit switch.
 * - actionJog: Performs a jogging action in a specified direction and speed, monitoring a limit switch.
 * - processLimitSwitches: Handles the logic for processing the state of limit switches.
 *
 * External Variables:
 * - limitSwitchUp: Digital signal detector for the upper limit switch.
 * - limitSwitchDown: Digital signal detector for the lower limit switch.
 * 
 * @author Karl Berger
 * @date 2025-05-20
 */

#ifndef ACTIONS_H
#define ACTIONS_H

#include "DigitalSignalDetector.h" // for limitSwitch argument

extern DigitalSignalDetector limitSwitchUp;
extern DigitalSignalDetector limitSwitchDown;

void actionScan(int btnIndx, int moveDirection, int speed, DigitalSignalDetector &limitSwitch);
void actionJog(int btnIndx, int moveDirection, int speed, DigitalSignalDetector &limitSwitch);
void processLimitSwitches();

#endif