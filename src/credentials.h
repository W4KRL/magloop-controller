//! credentials.h
//! 2025-02-18
//! 2025-03-24 added static IP address

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#define WIFI_SSID "DCMNET"
#define WIFI_PASSWORD "0F1A2D3E4D5G6L7O8R9Y"
// Set your Static IP address & hostname
//! See instructions in Notes.md for setting up static IP address
IPAddress localIP(192, 168, 0, 234);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// SCPI Identification
//! Do not include "~" in the string. The JavaScript client will parse it as a command.
#define MAKER "W4KRL"
#define MODEL "Mag Loop Controller"
#define SERIAL_NUMBER "S/N: 001"
#define VERSION "DevKit 2025-04-03"

//! Web Server File Dates
#define FW_DATE "2025-04-03"
#define HTML_DATE "2025-04-03"
#define SCRIPT_DATE "2025-04-03"
#define STYLES_DATE "2025-03-25"

//! GPIO Pin definitions
#define HB_IN1 23	  // PWM IN1 logic
#define HB_IN2 22	  // PWM IN2 logic
#define LIMIT_UP 19	  // upper limit switch interrupt
#define LIMIT_DOWN 18 // lower limit switch interrupt
#define SWR_RFL 35	  // SWR reflected ADC input
#define SWR_FWD 34	  // SWR forward ADC input
#define VM 32		  // VM voltage monitor ADC input

//! Web Button and LED colors
// used in ledControl.h & actions.h
#define LED_GREEN "LimeGreen"
#define LED_RED "Red"
// used in buttonControl.h
#define SCAN_UP "RoyalBlue"
#define SCAN_DOWN "LimeGreen"
#define JOG_UP "DodgerBlue"
#define JOG_DOWN "Lime"
#define UNPRESSED "DimGray"

#endif

//! end of credentials.h