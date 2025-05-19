//! @file swrCalc.h
// 2025-05-10 revise pin names and comments

//! placeholder for the SWR calculation function

#ifndef SWRCALC_H
#define SWRCALC_H

#include <Arduino.h>
#include "configuration.h" // for SWR pin definitions

//! ADC pins are defined in configuration.h

void swrCalcBegin();
float swrValue();

/*
update the SWR continuously at 100 ms intervals
send to the WebSocket only when the value has changed by more than 0.1

*/

#endif