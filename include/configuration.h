/**
 * @file configuration.h
 * @brief Header file for configuration settings of the magloop-controller project.
 *
 * This file contains configuration definitions and macros used throughout the project.
 * Add your configuration parameters and settings to configuation.cpp.
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h> // for IPAddress

//! Add your configuration parameters and settings to configuation.cpp.

//! Statis IP address
extern const IPAddress LOCAL_IP;
extern const IPAddress GATEWAY;
extern const IPAddress SUBNET;

//! WiFi credentials
extern const char *WIFI_SSID;
extern const char *WIFI_PASSWORD;
extern const char *HOSTNAME;

//! SCPI Identification
extern const char *MAKER;
extern const char *MODEL;
extern const char *SERIAL_NUMBER;
extern const char *VERSION;

//! Web Server File Dates
extern const String FW_DATE;
extern const String HTML_DATE;
extern const String SCRIPT_DATE;
extern const String STYLES_DATE;

//! GPIO Pins for ESP32 DevKit V1
extern const int HB_IN1_PIN;	 // PWM IN1 logic
extern const int HB_IN2_PIN;	 // PWM IN2 logic
extern const int LIMIT_UP_PIN;	 // upper limit switch interrupt
extern const int LIMIT_DOWN_PIN; // lower limit switch interrupt
extern const int SWR_RFL_PIN;	 // SWR reflected ADC input
extern const int SWR_FWD_PIN;	 // SWR forward ADC input
extern const int VM_PIN;		 // VM voltage monitor ADC input
extern const int SDA_PIN;		 // I2C SDA pin
extern const int SCL_PIN;		 // I2C SCL pin
#ifndef LED_BUILTIN				 // some versions of ESP32 have LED_BUILTIN predefined
extern const int LED_BUILTIN;	 // Built-in LED pin
#endif

//! Voltage Monitor and ADC parameters
extern const float R2;		   // VM voltage divider upper resistor
extern const float R1;		   // VM voltage divider lower resistor
extern const float VM_VOLTAGE; // VM voltage divider reference voltage
extern const float ADC_MAX;	   // ADC max value for 12-bit ADC

//! Web UI limit LED colors
extern const char *LED_COLOR_GREEN;
extern const char *LED_COLOR_RED;

//! Web UI button colors
extern const char *BTN_SCAN_UP_COLOR;
extern const char *BTN_SCAN_DOWN_COLOR;
extern const char *BTN_JOG_UP_COLOR;
extern const char *BTN_JOG_DOWN_COLOR;
extern const char *BTN_UNPRESSED_COLOR;
extern const char *BTN_DISABLED_COLOR;

#endif // CONFIGURATION_H
//! end of configuration.h