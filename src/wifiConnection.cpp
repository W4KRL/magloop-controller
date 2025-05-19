
/**
 * @file wifiConnection.cpp
 * @brief Implements Wi-Fi connection and OTA update functionality for the magloop-controller project.
 *
 * This file provides functions to initialize Wi-Fi connectivity, configure static IP,
 * handle built-in LED status indication, and enable Arduino OTA (Over-The-Air) updates.
 * It uses the ESP32 WiFi and ArduinoOTA libraries, and relies on configuration parameters
 * defined in "configuration.h" for SSID, password, and network settings.
 *
 * Features:
 * - Wi-Fi connection with status LED feedback.
 * - Static IP configuration.
 * - Built-in LED control and toggling.
 * - Arduino OTA update support with progress and error reporting.
 *
 * Dependencies:
 * - Arduino.h
 * - WiFi.h
 * - ArduinoOTA.h
 * - configuration.h
 */
#include "wifiConnection.h" // Wi-Fi connection header

#include <Arduino.h>	   // for PlatformIO
#include <WiFi.h>		   // for WiFi
#include <ArduinoOTA.h>	   // for OTA updates
#include "configuration.h" // for SSID, password, static IP

void otaBegin()
{
	ArduinoOTA.begin();
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

//! Global variables
static bool ledBuiltIn = LOW; // Built-in LED LOW = OFF, HIGH = ON

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
	ledBuiltIn = !ledBuiltIn;			   // Invert the current state
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

void wifiBegin()
{
	// Configure onboard LED pin
	pinMode(LED_BUILTIN, OUTPUT);	// Set LED_BUILTIN as output
	digitalWrite(LED_BUILTIN, LOW); // Start with LED off

	WiFi.mode(WIFI_STA);
	WiFi.persistent(false);
	WiFi.setAutoReconnect(true);
	WiFi.setSleep(false);
	Serial.printf("\n%s %s\n", "Connecting to", WIFI_SSID);
	// Set static IP configuration
	if (!WiFi.config(LOCAL_IP, GATEWAY, SUBNET))
	{
		Serial.println("Static IP Configuration Failed!");
		return;
	}
	wifiConnect(); // Connect to WiFi
} // wifiBegin()