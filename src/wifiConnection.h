//! wifiConnection.h
//! 2025-03-15 removed static IP due to router conflicts

#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include <WiFi.h> // for WiFi
#include <ArduinoOTA.h> // for OTA updates
// #include "credentials.h" // for SSID, password, static IP
// #include "actions.h"     // for setLED()

void initOTA()
{
    // Initialize OTA
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

//! Connect to WiFi
void wifiBegin()
{
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