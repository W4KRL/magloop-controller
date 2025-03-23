//! credentials.h
//! 2025-02-18
//! 2025-02-26 added static IP address

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#define SSID "DCMNET"
#define PASSWORD "0F1A2D3E4D5G6L7O8R9Y"
// Set your Static IP address & hostname
IPAddress local_IP(192, 168, 0, 234);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// SCPI Identification
//! Do not include "~" in the string. The JavaScript client will parse it as a command.
#define MAKER "W4KRL"
#define MODEL "Mag Loop Controller"
#define SERIAL_NUMBER "S/N: 001"
#define VERSION "DevKit 2025-03-14"

//! Web Server File Dates
#define FW_DATE "2025-03-23"
#define HTML_DATE "2025-03-20"
#define SCRIPT_DATE "2025-03-21"
#define STYLES_DATE "2025-02-15"

//! GPIO Pin definitions
#define HB_IN1 23	  // pin IN1 logic
#define HB_IN2 22	  // pin IN2 logic
#define LIMIT_UP 19	  // pin upper limit switch
#define LIMIT_DOWN 18 // pin lower limit switch
#define SWR_RFL 35	  // SWR reflected ADC input
#define SWR_FWD 34	  // SWR forward ADC input
// #define I2C_SDA 7         // I2C SDA
// #define LED_BUILTIN 8     // onboard LED
// #define I2C_SCL 9         // I2C SCL

#endif

//! end of credentials.h