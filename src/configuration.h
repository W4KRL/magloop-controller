//! @file configuration.h
//! 2025-05-10 update firmware version and dates, added voltage divider values

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h> // Include Arduino library for IPAddress

#define WIFI_SSID "DCMNET"
#define WIFI_PASSWORD "0F1A2D3E4D5G6L7O8R9Y"
//! See instructions in Notes.md for setting up static IP address
/**
 * @brief Defines the local IP address for the device.
 * 
 * This IP address is used to configure the network settings of the device.
 * Ensure that the IP address is within the subnet of your local network
 * and does not conflict with other devices on the network.
 * 
 * Example: IPAddress(192, 168, 0, 234) represents the static IP address
 * 192.168.0.234.
 */

 #define LOCAL_IP 192, 168, 0, 234 // Set your local IP address
 #define GATEWAY 192, 168, 0, 1   // Set your Gateway IP address
 #define SUBNET 255, 255, 255, 0  // Set your subnet mask
 
// IPAddress localIP(192, 168, 0, 234);
// // Set your Gateway IP address
// IPAddress gateway(192, 168, 0, 1);
// IPAddress subnet(255, 255, 255, 0);

// SCPI Identification
//! Do not include "~" in the string. The JavaScript client will parse it as a command.
#define MAKER "W4KRL"
#define MODEL "Mag Loop Controller"
#define SERIAL_NUMBER "S/N: 001"
#define VERSION "DevKit 2025-05-08"

//! Web Server File Dates
#define FW_DATE "2025-05-08"
#define HTML_DATE "2025-04-17"
#define SCRIPT_DATE "2025-04-17"
#define STYLES_DATE "2025-03-25"

//! GPIO Pin definitions for ESP32 DOIT DevKit V1
#define HB_IN1_PIN 23	  // PWM IN1 logic
#define HB_IN2_PIN 22	  // PWM IN2 logic
#define LIMIT_UP_PIN 18	  // upper limit switch interrupt
#define LIMIT_DOWN_PIN 19 // lower limit switch interrupt
#define SWR_RFL_PIN 35	  // SWR reflected ADC input
#define SWR_FWD_PIN 34	  // SWR forward ADC input
#define VM_PIN 32		  // VM voltage monitor ADC input
#define SDA_PIN 21		  // I2C SDA pin
#define SCL_PIN 4		  // I2C SCL pin
#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // Built-in LED pin
#endif

//! Voltage Monitor and ADC parameters
#define R2 47000.0	   // VM voltage divider upper resistor
#define R1 6800.0	   // VM voltage divider lower resistor
#define VM_VOLTAGE 3.3 // VM voltage divider reference voltage
#define ADC_MAX 4095.0 // ADC max value for 12-bit ADC

//! Web Button and LED colors
// used in ledControl.h & actions.h
#define LED_COLOR_GREEN "LimeGreen"
#define LED_COLOR_RED "Red"
// used in buttonControl.h
#define BTN_SCAN_UP_COLOR "RoyalBlue"
#define BTN_SCAN_DOWN_COLOR "Lime"
#define BTN_JOG_UP_COLOR "RoyalBlue"
#define BTN_JOG_DOWN_COLOR "Lime"
#define BTN_UNPRESSED_COLOR "DimGray"
#define BTN_DISABLED_COLOR "Cloud"

#endif

//! end of configuration.h