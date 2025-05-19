/**
 * @file configuation.cpp
 * @brief Configuration file for Mag Loop Controller project.
 *
 * This file contains macro definitions and configuration parameters for the ESP32-based
 * Mag Loop Controller. It includes WiFi credentials, static IP settings, device identification
 * strings, web server file dates, GPIO pin assignments, voltage monitor parameters, and
 * UI color definitions.
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

#include "configuration.h"

#include <Arduino.h> // Include Arduino library for IPAddress

const char *WIFI_SSID = "DCMNET";
const char *WIFI_PASSWORD = "0F1A2D3E4D5G6L7O8R9Y";

/**
 * @brief Defines the local IP address for the device.
 *
 * This IP address is used to configure the network settings of the device.
 * Ensure that the IP address is within the subnet of your local network
 * and does not conflict with other devices on the network.
 *
 * Example: IPAddress(192, 168, 0, 234) represents the static IP address
 * 192.168.0.234.
 *
 * @note See instructions in Notes.md for setting up static IP address
 */
const IPAddress LOCAL_IP(192, 168, 0, 234);
const IPAddress GATEWAY(192, 168, 0, 1);
const IPAddress SUBNET(255, 255, 255, 0);

// SCPI Identification
const String MAKER = "W4KRL";
const String MODEL = "Mag Loop Controller";
const String SERIAL_NUMBER = "S/N: 001";
const String VERSION = "DevKit 2025-05-08";

//! Web Server File Dates
const String FW_DATE = "2025-05-08";
const String HTML_DATE = "2025-04-17";
const String SCRIPT_DATE = "2025-04-17";
const String STYLES_DATE = "2025-03-25";

//! GPIO Pin definitions for ESP32 DOIT DevKit V1
const int HB_IN1_PIN = 23;	   // PWM IN1 logic
const int HB_IN2_PIN = 22;	   // PWM IN2 logic
const int LIMIT_UP_PIN = 18;   // upper limit switch interrupt
const int LIMIT_DOWN_PIN = 19; // lower limit switch interrupt
const int SWR_RFL_PIN = 35;	   // SWR reflected ADC input
const int SWR_FWD_PIN = 34;	   // SWR forward ADC input
const int VM_PIN = 32;		   // VM voltage monitor ADC input
const int SDA_PIN = 21;		   // I2C SDA pin
const int SCL_PIN = 4;		   // I2C SCL pin
#ifndef LED_BUILTIN			   // some versions of ESP32 have LED_BUILTIN predefined
const int LED_BUILTIN = 2;	   // Built-in LED pin
#endif

//! Voltage Monitor and ADC parameters
const float R2 = 47000.0f;	   // VM voltage divider upper resistor
const float R1 = 6800.0f;	   // VM voltage divider lower resistor
const float VM_VOLTAGE = 3.3f; // VM voltage divider reference voltage
const float ADC_MAX = 4095.0f; // ADC max value for 12-bit ADC

// Web UI LED Colors
const char* LED_COLOR_GREEN = "LimeGreen";
const char* LED_COLOR_RED = "Red";

// Web UI Button Colors
const char* BTN_SCAN_UP_COLOR = "RoyalBlue";
const char* BTN_SCAN_DOWN_COLOR = "Lime";
const char* BTN_JOG_UP_COLOR = "RoyalBlue";
const char* BTN_JOG_DOWN_COLOR = "Lime";
const char* BTN_UNPRESSED_COLOR = "DimGray";
const char* BTN_DISABLED_COLOR = "Cloud";