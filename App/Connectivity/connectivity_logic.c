/********************************************************************************
 * @file           : connectivity_logic.c
 * @brief          : Connectivity logic implementation
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 AllyTech LLC.
 * All rights reserved.
 ********************************************************************************/

#include "connectivity_logic.h"

static EspAtUrcType s_last_urc = ESP_AT_URC_NONE;
static uint32_t s_urc_count = 0u;

/**
 * @brief Handles a classified ESP-AT unsolicited result code.
 *
 * @param urc Classified ESP-AT unsolicited result code.
 */
void ConnectivityLogic_HandleUrc(EspAtUrcType urc)
{
  if (urc == ESP_AT_URC_NONE) {
    return;
  }

  s_last_urc = urc;
  s_urc_count++;
}

/**
 * @brief Returns the most recently delivered ESP-AT unsolicited result code.
 *
 * @return Most recently delivered URC type.
 */
EspAtUrcType ConnectivityLogic_GetLastUrc(void)
{
  return s_last_urc;
}

/**
 * @brief Returns the number of ESP-AT unsolicited result codes delivered to
 * connectivity logic.
 *
 * @return Number of delivered URCs.
 */
uint32_t ConnectivityLogic_GetUrcCount(void)
{
  return s_urc_count;
}