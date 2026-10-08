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

typedef enum {
  CONNECTIVITY_STARTUP_IDLE = 0,
  CONNECTIVITY_STARTUP_WAIT_READY,
  CONNECTIVITY_STARTUP_WAIT_AT,
  CONNECTIVITY_STARTUP_WAIT_ATE0,
  CONNECTIVITY_STARTUP_INITIALIZED,
  CONNECTIVITY_STARTUP_FAILED,
} ConnectivityStartupState;

static const uint8_t s_at_command[] = "AT\r\n";
static const uint8_t s_ate0_command[] = "ATE0\r\n";

static ConnectivityStartupState s_startup_state = CONNECTIVITY_STARTUP_IDLE;

/**
 * @brief Starts ESP-AT startup synchronization.
 *
 * UART reception must already be active before this function is called.
 *
 * @return 1u when startup synchronization begins; 0u otherwise.
 */
uint8_t ConnectivityLogic_Start(void)
{
  if (s_startup_state != CONNECTIVITY_STARTUP_IDLE) {
    return 0u;
  }

  s_startup_state = CONNECTIVITY_STARTUP_WAIT_READY;

  return 1u;
}

/**
 * @brief Processes ESP-AT startup synchronization.
 */
void ConnectivityLogic_Process(void)
{
  EspAtTransactionState transaction_state;

  EspAt_ProcessTransactionTimeout();
  transaction_state = EspAt_GetTransactionState();

  if (s_startup_state == CONNECTIVITY_STARTUP_WAIT_AT) {
    if (transaction_state == ESP_AT_TRANSACTION_OK) {
      EspAt_ResetTransaction();

      if (EspAt_StartCommand(s_ate0_command,
                             sizeof(s_ate0_command) - 1u,
                             1000u) == 0u) {
        s_startup_state = CONNECTIVITY_STARTUP_FAILED;
        return;
      }

      s_startup_state = CONNECTIVITY_STARTUP_WAIT_ATE0;
    }

    return;
  }

  if (s_startup_state == CONNECTIVITY_STARTUP_WAIT_ATE0) {
    if (transaction_state == ESP_AT_TRANSACTION_OK) {
      EspAt_ResetTransaction();
      s_startup_state = CONNECTIVITY_STARTUP_INITIALIZED;
    } else if ((transaction_state == ESP_AT_TRANSACTION_ERROR) ||
               (transaction_state == ESP_AT_TRANSACTION_TIMEOUT)) {
      s_startup_state = CONNECTIVITY_STARTUP_FAILED;
    }
  }
}

/**
 * @brief Returns whether ESP-AT startup synchronization is complete.
 *
 * @return 1u when ESP-AT communication is initialized; 0u otherwise.
 */
uint8_t ConnectivityLogic_IsEspAtInitialized(void)
{
  return (s_startup_state == CONNECTIVITY_STARTUP_INITIALIZED) ? 1u : 0u;
}

/**
 * @brief Handles a classified ESP-AT unsolicited result code.
 *
 * @param urc Classified ESP-AT unsolicited result code.
 */
void ConnectivityLogic_HandleUrc(EspAtUrcType urc)
{
  if ((s_startup_state == CONNECTIVITY_STARTUP_WAIT_READY) &&
      (urc == ESP_AT_URC_READY)) {
    if (EspAt_StartCommand(s_ate0_command,
                           sizeof(s_ate0_command) - 1u,
                           1000u) == 0u) {
      s_startup_state = CONNECTIVITY_STARTUP_FAILED;
      return;
    }

    s_startup_state = CONNECTIVITY_STARTUP_WAIT_ATE0;
  }
}