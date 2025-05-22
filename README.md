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
The limit switches are normally low IR blockage sensors. The input signal is HIGH when a
corresponding limit is reached.

Local reporistory: C:\Users\KarlB\OneDrive\Documents\PlatformIO\Projects\magloop-controller

Github repository: https://github.com/W4KRL/magloop-controller

|Pin ID|IN|OUT|ANALOG|PIN #|CHIP|PIN#|ANALOG|OIT|IN|PIN ID|

| MAGLOOP     |      | GPIO | IN | OUT | COMMENT | PIN # |   CHIP   | PIN # | COMMENT | OUT | IN | GPIO |         | MAGLOOP |
|-------------|------|:----:|:--:|:---:|:-------:|:-----:|:--------:|:-----:|---------|-----|----|------|---------|---------|
|             | EN   |      |    |     |         |   15  |          |   16  |         | PWM | Y  | 23   | MOSI    | HB_IN1  |
|             |      |  36  |  Y |  N  |  ADC1_0 |   14  |          |   17  |         | PWM | Y  | 22   | SCL     | HB_IN2  |
|             |      |  39  |  Y |  N  |  ADC1_3 |   13  |          |   18  |         |     |    | 1    | TXD0    |         |
| ADC SWR FWD |      |  34  |  Y |  N  |  ADC1_6 |   12  |          |   19  |         |     |    | 3    | RXD0    |         |
| ADC SWR RFL |      |  35  |  Y |  N  |  ADC1_7 |   11  |          |   20  |         | PWM | Y  | 21   | SDA     |         |
|             |      |  32  |  Y | PWM |  ADC1_4 |   10  |          |   21  |         | PWM | Y  | 19   | MISO    |         |
|             |      |  33  |  Y | PWM |  ADC1_5 |   9   |          |   22  |         | PWM | Y  | 18   | SCK     |         |
|             | DAC1 |  25  |  Y | PWM |         |   8   |          |   23  |         | PWM | Y  | 5    | SS,BOOT |         |
|             | DAC2 |  26  |  Y | PWM |         |   7   |          |   24  |         | PWM | Y  | 17   | TXD2    |         |
|             |      |  27  |  Y | PWM |         |   6   |          |   25  |         | PWM | Y  | 16   | RXD2    |         |
|             |      |  14  |  Y | PWM |         |   5   |          |   26  |         | PWM | Y  | 4    |         |         |
|             | BOOT |  12  |  Y | PWM |         |   4   |          |   27  |         | PWM | Y  | 2    | BOOT    |         |
|             |      |  13  |  Y | PWM |         |   3   |          |   28  |         | PWM | Y  | 15   | BOOT    |         |
|             | GND  |      |    |     |         |   2   |          |   29  |         |     |    |      | GND     |         |
|             | VIN  |      |    |     |         |   1   |          |   30  |         |     |    |      | 3V3     |         |
|             |      |      |    |     |         |       | USB MINI |       |         |     |    |      |         |         |