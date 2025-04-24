//! wifiConnection.h
//! 2025-04-23 add comments

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

/**
 * @brief Connects to a Wi-Fi network using predefined SSID and password.
 *
 * This function attempts to establish a connection to a Wi-Fi network
 * using the credentials defined by WIFI_SSID and WIFI_PASSWORD. While
 * the connection is being established, the built-in LED toggles every
 * 250 milliseconds, and a dot ('.') is printed to the Serial monitor
 * to indicate progress. Once connected, the built-in LED is turned on,
 * and the assigned IP address is printed to the Serial monitor.
 *
 * @note Ensure that WIFI_SSID and WIFI_PASSWORD are defined before
 *       calling this function. The Serial monitor should also be
 *       initialized using Serial.begin() for output to be visible.
 */
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
/**
 * @brief Initializes the WiFi connection and configures the onboard LED.
 *
 * This function sets up the WiFi in station mode, disables persistent storage,
 * enables auto-reconnection, and disables WiFi sleep mode. It also configures
 * a static IP address and attempts to connect to the WiFi network using the
 * provided SSID and password. Additionally, it configures the onboard LED pin
 * and ensures it starts in the off state.
 *
 * @note Ensure that the following variables are defined and initialized before
 *       calling this function:
 *       - `WIFI_SSID`: The SSID of the WiFi network to connect to.
 *       - `localIP`: The static IP address to assign to the device.
 *       - `gateway`: The gateway address for the network.
 *       - `subnet`: The subnet mask for the network.
 *
 * @warning If the static IP configuration fails, the function will terminate
 *          without attempting to connect to the WiFi network.
 */
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
} // wifiBegin()

#endif