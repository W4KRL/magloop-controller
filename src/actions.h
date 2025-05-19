//! @file actions.h
//! 2024-05-10 change to DigitalSignalDetector.h from Bounce2.h

#ifndef ACTIONS_H
#define ACTIONS_H

#include <Arduino.h>               // Required for basic Arduino functions
#include "DigitalSignalDetector.h" // for limit switch detection

extern DigitalSignalDetector limitSwitchUp;
extern DigitalSignalDetector limitSwitchDown;

void actionScan(int btnIndx, int moveDirection, int speed, DigitalSignalDetector &limitSwitch);
void actionJog(int btnIndx, int moveDirection, int speed, DigitalSignalDetector &limitSwitch);
void processLimitSwitches();
String processSWR();

#endif