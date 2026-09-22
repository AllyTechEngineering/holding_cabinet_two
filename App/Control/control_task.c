/********************************************************************************
  * @file           : control_task.c
  * @brief          : FreeRTOS task orchestrating heater control loop based on
  *                    baseplate temperature feedback
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  *******************************************************************************/
#include "control_task.h"
#include "app_types.h"
#include "cmsis_os.h"
#include "main.h"
#include "thermistor_driver.h"
#include <stdint.h>

extern osMessageQueueId_t qSenseToHeatHandle;
extern osMessageQueueId_t qHeatToDisplayHandle;

#define HEAT_STATUS_WAIT_MS 100u

/* Codes 10 and 11 are defined in Docs/error_codes.md.
   Code 12 identifies an ADC read failure and must be added there. */
#define NTC_OPEN_ERROR  10u
#define NTC_SHORT_ERROR 11u
#define NTC_READ_ERROR  12u

static void publish_status(const HeatStatus_t *status) {
  HeatStatus_t discarded;

  /* qHeatToDisplay has depth 1: replace an unread older status. */
  (void)osMessageQueueGet(qHeatToDisplayHandle, &discarded, NULL, 0u);

  if (osMessageQueuePut(qHeatToDisplayHandle, status, 0u, 0u) != osOK) {
    HAL_GPIO_WritePin(HeatRelay_GPIO_Port, HeatRelay_Pin, GPIO_PIN_SET);
    Error_Handler();
  }
}

void HeatTask_Run(void *argument) {
  uint16_t reading;
  HeatStatus_t status = {0};

  (void)argument;

  /* This stage reports temperature only. The relay is active-low. */
  HAL_GPIO_WritePin(HeatRelay_GPIO_Port, HeatRelay_Pin, GPIO_PIN_SET);

  for (;;) {
    if (osMessageQueueGet(qSenseToHeatHandle, &reading, NULL,
                          HEAT_STATUS_WAIT_MS) != osOK) {
      continue;
    }

    status.currentTempTenthsC = reading;
    status.relayOn = 0u;

    if (reading == THERMISTOR_FAULT_OPEN) {
      status.errorCode = NTC_OPEN_ERROR;
    } else if (reading == THERMISTOR_FAULT_SHORT) {
      status.errorCode = NTC_SHORT_ERROR;
    } else if (reading == THERMISTOR_FAULT_READ) {
      status.errorCode = NTC_READ_ERROR;
    } else {
      status.errorCode = 0u;
    }

    publish_status(&status);
  }
}