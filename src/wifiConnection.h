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
    Serial.printf("\n%s %s\n", "Connecting to", WIFI_SSID);
        // Set static IP configuration
        if (!WiFi.config(localIP, gateway, subnet))
        {
            Serial.println("Static IP Configuration Failed!");
            return;
        }
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        toggleLED_BUILTIN();
        delay(250);
        Serial.print(".");
    }
    setLED_BUILTIN(HIGH); // Turn on the LED when connected
    Serial.printf("\n%s: %s\n", "Connected to IP Address", WiFi.localIP().toString());
} // wifiBegin()

#endif