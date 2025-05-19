#include "swrCalc.h"

#include <Arduino.h>
#include "configuration.h" // for SWR pin definitions
#include "webSocket.h"	   // for notifyClients()

//! ADC pins are defined in configuration.h

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
}
