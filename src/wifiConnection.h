//! wifiConnection.h
//! 2025-04-11 change name of otaBegin()

#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include <WiFi.h>        // for WiFi
#include <ArduinoOTA.h>  // for OTA updates
#include "credentials.h" // for SSID, password, static IP
#include "actions.h"     // for setLED()

//! Initialize OTA
//! This function sets up the OTA (Over-The-Air) update process for the ESP32 device.
void otaBegin()
{
    ArduinoOTA.onStart([]()
                       {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("Start updating " + type); });
    ArduinoOTA.onEnd([]()
                     { Serial.println("\nUpdate Complete"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("Progress: %u%%\r", (progress * 100) / total); });
    ArduinoOTA.onError([](ota_error_t error)
                       {
Serial.printf("Error[%u]: ", error);
if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
    ArduinoOTA.begin();
    Serial.println("OTA Ready");
}

/**
 * @brief Sets the state of the built-in LED.
 *
 * This function updates the state of the built-in LED to the specified value.
 * It also updates the internal `ledBuiltIn` variable to reflect the current state.
 *
 * @param state A boolean value indicating the desired LED state:
 *              - `true` to turn the LED ON.
 *              - `false` to turn the LED OFF.
 */
void setLED_BUILTIN(bool state)
{
    ledBuiltIn = state;
    digitalWrite(LED_BUILTIN, ledBuiltIn);
}

/**
 * @brief Toggles the state of the built-in LED.
 *
 * This function inverts the current state of the built-in LED. If the LED is ON,
 * it will be turned OFF, and vice versa. The internal `ledState` variable is
 * updated to reflect the new state.
 */
void toggleLED_BUILTIN()
{
    ledBuiltIn = !ledBuiltIn;              // Invert the current state
    digitalWrite(LED_BUILTIN, ledBuiltIn); // Apply the new state
}

void wifiConnect()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        toggleLED_BUILTIN();
        delay(250);
        Serial.print(".");
    }
    setLED_BUILTIN(HIGH); // Turn on the LED when connected
    Serial.printf("\n%s: %s\n", "Connected to IP Address", WiFi.localIP().toString());
} // wifiConnect()

//! Connect to WiFi
void wifiBegin()
{
    // Configure onboard LED pin
    pinMode(LED_BUILTIN, OUTPUT);   // Set LED_BUILTIN as output
    digitalWrite(LED_BUILTIN, LOW); // Start with LED off

    WiFi.mode(WIFI_STA);
    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);
    WiFi.setSleep(false);
    Serial.printf("\n%s %s\n", "Connecting to", WIFI_SSID);
    // Set static IP configuration
    if (!WiFi.config(localIP, gateway, subnet))
    {
        Serial.println("Static IP Configuration Failed!");
        return;
    }
    wifiConnect(); // Connect to WiFi
    // Serial.println("WiFi connected");
    // Serial.print("IP address: ");
    // Serial.println(WiFi.localIP());
    // Serial.print("Signal strength (dBm): ");
    // Serial.println(WiFi.RSSI());
    // Serial.println("WiFi connection established.");
} // wifiBegin()

#endif