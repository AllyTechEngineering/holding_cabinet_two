/**
  ******************************************************************************
  * @file           : thermistor_driver.c
  * @brief          : NTC thermistor read + Beta-equation conversion, with
  *                    open/short fault detection
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  ******************************************************************************
  */

#include "thermistor_driver.h"
#include "main.h"
#include <math.h>
/* ADC1 is created by CubeMX in Core/Src/main.c. */
extern ADC_HandleTypeDef hadc1;

/* NTC divider topology, confirmed during hardware bring-up (Docs/arch.md
 * Section 8): MF52B, R25 = 10k ohm, Beta(25/50) = 3950K. NTC on the low
 * side of the divider (R1 = 10k from 3.3V to sense node, NTC from sense
 * node to GND) -- higher ADC reading means colder. No firmware
 * calibration offset applied; the ~5.7F residual offset found during
 * bring-up is accepted as within bang-bang hysteresis tolerance. */
#define NTC_R25_OHMS      10000.0f
#define NTC_BETA          3950.0f
#define NTC_T25_KELVIN    298.15f
#define NTC_R1_OHMS       10000.0f
#define ADC_MAX_COUNT     4095

/* Fault thresholds, derived from the same divider math:
 * R_ntc = R1 * ADC / (ADC_MAX - ADC).
 * As R_ntc -> infinity (open circuit), ADC -> ADC_MAX.
 * As R_ntc -> 0 (short), ADC -> 0.
 * The open threshold (>=4090) is carried over verified from bring-up;
 * the short threshold (<=5) is new, chosen symmetrically. */
#define ADC_OPEN_THRESHOLD   4090u
#define ADC_SHORT_THRESHOLD  5u

static float ntc_resistance_to_celsius(float r_ohms)
{
  float tempK = 1.0f / ((1.0f / NTC_T25_KELVIN) + (1.0f / NTC_BETA) * logf(r_ohms / NTC_R25_OHMS));
  return tempK - 273.15f;
}

uint16_t Thermistor_ReadTenthsC(void)
{
  uint32_t rawAdc;
  float r_ohms;
  float tempC;
  int32_t tenths;

  if (HAL_ADC_Start(&hadc1) != HAL_OK)
  {
    return THERMISTOR_FAULT_READ;
  }

  if (HAL_ADC_PollForConversion(&hadc1, 10U) != HAL_OK)
  {
    (void)HAL_ADC_Stop(&hadc1);
    return THERMISTOR_FAULT_READ;
  }

  rawAdc = HAL_ADC_GetValue(&hadc1);

  if (HAL_ADC_Stop(&hadc1) != HAL_OK)
  {
    return THERMISTOR_FAULT_READ;
  }

  if (rawAdc >= ADC_OPEN_THRESHOLD)
  {
    return THERMISTOR_FAULT_OPEN;
  }

  if (rawAdc <= ADC_SHORT_THRESHOLD)
  {
    return THERMISTOR_FAULT_SHORT;
  }

  r_ohms = NTC_R1_OHMS * (float)rawAdc /
           (float)(ADC_MAX_COUNT - rawAdc);
  tempC = ntc_resistance_to_celsius(r_ohms);

  tenths = (int32_t)lroundf(tempC * 10.0f);
  if (tenths < 0)
  {
    tenths = 0;
  }

  return (uint16_t)tenths;
}