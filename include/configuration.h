/**
 * @file configuation.h
 * @brief Configuration file for Mag Loop Controller project.
 *
 * This file contains macro definitions and configuration parameters for the ESP32-based
 * Mag Loop Controller. It includes WiFi credentials, static IP settings, device identification
 * strings, web server file dates, GPIO pin assignments, voltage monitor parameters, and
 * UI color definitions. Use of 'inline' eliminates the need fror a separate .cpp file.
 *
 * Key sections:
 * - WiFi SSID and password for network connection.
 * - Static IP, gateway, and subnet mask definitions for network configuration.
 * - Device identification macros for SCPI and web server.
 * - File date macros for firmware and web assets.
 * - GPIO pin assignments for hardware interfacing (PWM, limit switches, ADC, I2C, LED).
 * - Voltage divider and ADC parameters for voltage monitoring.
 * - Color definitions for web UI buttons and LEDs.
 *
 * @note
 * - Update the WiFi credentials and static IP settings as needed for your local network.
 * - Ensure pin assignments match your ESP32 hardware.
 * - Do not include "~" in SCPI identification strings to avoid parsing issues in the JavaScript client.
 * - Refer to Notes.md for detailed setup instructions.
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H	

#include <Arduino.h> // for IPAddress

// WiFi Credentials
inline const char *WIFI_SSID = "DCMNET";
inline const char *WIFI_PASSWORD = "0F1A2D3E4D5G6L7O8R9Y";

// Static IP Settings
inline const IPAddress LOCAL_IP(192, 168, 0, 234);
inline const IPAddress GATEWAY(192, 168, 0, 1);
inline const IPAddress SUBNET(255, 255, 255, 0);

//! SCPI Identification
inline const char *MAKER = "W4KRL";
inline const char *MODEL = "Mag Loop Controller";
inline const char *SERIAL_NUMBER = "S/N: 001";
inline const char *VERSION = "DevKit 2025-05-08";

//! Web Server File Dates
inline const String FW_DATE = "2025-05-08";
inline const String HTML_DATE = "2025-04-17";
inline const String SCRIPT_DATE = "2025-04-17";
inline const String STYLES_DATE = "2025-03-25";

//! GPIO Pin definitions for ESP32 DOIT DevKit V1
inline const int HB_IN1_PIN = 23;	   // PWM IN1 logic
inline const int HB_IN2_PIN = 22;	   // PWM IN2 logic
inline const int LIMIT_UP_PIN = 18;    // upper limit switch interrupt
inline const int LIMIT_DOWN_PIN = 19;  // lower limit switch interrupt
inline const int SWR_RFL_PIN = 35;	   // SWR reflected ADC input
inline const int SWR_FWD_PIN = 34;	   // SWR forward ADC input
inline const int VM_PIN = 32;		   // VM voltage monitor ADC input
inline const int SDA_PIN = 21;		   // I2C SDA pin
inline const int SCL_PIN = 4;		   // I2C SCL pin
#ifndef LED_BUILTIN                   // some versions of ESP32 have LED_BUILTIN predefined
inline const int LED_BUILTIN = 2;	   // Built-in LED pin
#endif

//! Voltage Monitor and ADC parameters
inline const float R2 = 47000.0f;	   // VM voltage divider upper resistor
inline const float R1 = 6800.0f;	   // VM voltage divider lower resistor
inline const float VM_VOLTAGE = 3.3f;  // VM voltage divider reference voltage
inline const float ADC_MAX = 4095.0f;  // ADC max value for 12-bit ADC

//! Web UI LED Colors
inline const char* LED_COLOR_GREEN = "LimeGreen";
inline const char* LED_COLOR_RED = "Red";

//! Web UI Button Colors
inline const char* BTN_SCAN_UP_COLOR = "RoyalBlue";
inline const char* BTN_SCAN_DOWN_COLOR = "Lime";
inline const char* BTN_JOG_UP_COLOR = "RoyalBlue";
inline const char* BTN_JOG_DOWN_COLOR = "Lime";
inline const char* BTN_UNPRESSED_COLOR = "DimGray";
inline const char* BTN_DISABLED_COLOR = "Cloud";

#endif // CONFIGURATION_H

//! end of configuration.h