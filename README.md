# Magnetic Loop Antenna Controller
## Webserver for control of motorized tuning capacitor for magnetic loop antenna

Karl Berger
W4KRL
May 20, 2025


This code is for the ESP32 DevKit board, in PlatformIO with Arduino framework 3.x.
It uses the WebSocket protocol to communicate with the web interface.
The web interface is defined by html, JavaScript and CSS in flash memory.
The device employs Standard Commands for Programmable Instruments
(SCPI; often pronounced "skippy") for command and control.
The delimiter for web socket commands is "~" to avoid conflict
with SCPI commands using ':' and ';' as delimiters.

The vacuum capacitor motor is stopped if travel reaches upper or lower limits.
The limit switches are normally closed (NC) reed switches with software
debouncing and external pullup resistors. The input signal is HIGH when a
corresponding limit is reached.

Local reporistory: C:\Users\KarlB\OneDrive\Documents\PlatformIO\Projects\magloop-controller

Github repositiry: 