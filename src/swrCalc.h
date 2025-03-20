//! swrCalc.h
// 02/06/2025   

//! placeholder for the SWR calculation function

#ifndef SWRCALC_H
#define SWRCALC_H

#include <Arduino.h>

// ADC pins
#define SWR_FWD 34
#define SWR_REV 35

void swrCalcBegin()
{
  // setup the ADC pins
  pinMode(SWR_FWD, INPUT);
  pinMode(SWR_REV, INPUT);
} // swrCalcBegin()

float swrValue()
{
  // read the forward and reverse power
  int fwdPower = analogRead(SWR_FWD);
  int revPower = analogRead(SWR_REV);

  // calculate the SWR
  float swr = (fwdPower + revPower) / (fwdPower - revPower);

  return swr;
} // swrValue()

/*
update the SWR continuously at 100 ms intervals
send to the WebSocket only when the value has changed by more than 0.1

*/

#endif