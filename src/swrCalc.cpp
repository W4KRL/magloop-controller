/**
 * @file swrCalc.cpp
 * @brief Functions for SWR (Standing Wave Ratio) calculation and update for the magloop-controller project.
 *
 * This file provides functions to initialize ADC pins for SWR measurement, calculate the SWR value
 * based on analog readings, and periodically update and notify clients with the SWR value.
 *
 * Functions:
 * - swrCalcBegin(): Initializes the ADC pins for forward and reflected power measurement.
 * - swrValue(): Reads the forward and reflected power from ADC pins and calculates the SWR.
 * - swrUpdate(): Periodically generates a random SWR value (for testing) and notifies connected clients.
 *
 * Dependencies:
 * - Arduino.h: For pinMode, analogRead, millis, etc.
 * - configuration.h: For SWR pin definitions (SWR_FWD_PIN, SWR_RFL_PIN).
 * - webSocket.h: For notifyClients() function to send updates to clients.
 *
 * @author Karl Berger
 * @date 2025-05-20
 */

#include "swrCalc.h"

#include <Arduino.h>	   // for Arduino functions
#include "configuration.h" // for SWR pin definitions
#include "webSocket.h"	   // for notifyClients()

void swrCalcBegin()
{
	// setup the ADC pins
	// call in setup()
	pinMode(SWR_FWD_PIN, INPUT);
	pinMode(SWR_RFL_PIN, INPUT);
} // swrCalcBegin()

float swrValue()
{
	// read the forward and reverse power
	int fwdPower = analogRead(SWR_FWD_PIN);
	int revPower = analogRead(SWR_RFL_PIN);

	// calculate the SWR
	float swr = (fwdPower + revPower) / (fwdPower - revPower);

	return swr;
} // swrValue()

void swrUpdate()
{
	unsigned long swrInterval = 5000;
	static unsigned long swrTime = millis() + swrInterval;
	if (millis() > swrTime)
	{
		float swrValue = 1 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (9.0));
		String swrValueString = "swr~" + String(swrValue, 2);
		notifyClients(swrValueString); // send to all clients
		swrTime = millis() + swrInterval;
	}
} // swrUpdate()
