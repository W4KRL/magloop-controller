

/**
 * @file swrCalc.h
 * @brief Header file for SWR (Standing Wave Ratio) calculation functions.
 *
 * This file declares functions for initializing and updating SWR measurements
 * using ADC pins defined in configuration.h. Intended for use with Arduino-based
 * magloop controller projects.
 *
 * Functions:
 *  - swrCalcBegin(): Initializes SWR calculation hardware and software resources.
 *  - swrUpdate(): Performs an SWR measurement update.
 *
 * Dependencies:
 *  - Arduino.h: Core Arduino functions and types.
 *  - configuration.h: Contains SWR pin definitions and configuration.
 *
 * @author Karl Berger
 * @date 2025-05-20
 */

#ifndef SWRCALC_H
#define SWRCALC_H

#include <Arduino.h>
#include "configuration.h" // for SWR pin definitions

// ADC pins are defined in configuration.h

void swrCalcBegin();
void swrUpdate();

#endif