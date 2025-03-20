//! wifiConnection.h
//! 2025-03-15 removed static IP due to router conflicts

#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include <WiFi.h>        // for WiFi
#include "credentials.h" // for SSID, password, static IP
#include "actions.h"     // for setLED()

//! Connect to WiFi
void wifiBegin()
{
    Serial.printf("\n%s %s\n", "Connecting to", SSID);
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        toggleLED();
        delay(250);
        Serial.print(".");
    }
    setLED(HIGH);
    Serial.printf("\n%s: %s\n", "Connected to IP Address", WiFi.localIP().toString());
} // wifiBegin()

#endif