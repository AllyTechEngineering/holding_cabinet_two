/**
  ******************************************************************************
  * @file           : thermistor_driver.h
  * @brief          : Header for thermistor_driver.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  ******************************************************************************
  */

#ifndef __THERMISTOR_DRIVER_H
#define __THERMISTOR_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Sentinel values returned by Thermistor_ReadTenthsC() in place of a real
 * reading -- chosen well outside the physically valid range for this
 * product (65-120F / ~183-489 tenths-C), so a fault can never be mistaken
 * for a real temperature. See Docs/error_codes.md for the error codes
 * these map to once HeatTask forwards them (NTC Open = 10, NTC Short = 11). */
#define THERMISTOR_FAULT_OPEN   0xFFFFu   /* ADC pegged near max -- NTC disconnected */
#define THERMISTOR_FAULT_SHORT  0xFFFEu   /* ADC pegged near min -- NTC shorted */

/* Reads ADC1 (blocking, single conversion) and converts to tenths of a
 * degree Celsius via the Beta equation, or returns one of the
 * THERMISTOR_FAULT_* sentinels above. */
uint16_t Thermistor_ReadTenthsC(void);

#ifdef __cplusplus
}
#endif

#endif /* __THERMISTOR_DRIVER_H */