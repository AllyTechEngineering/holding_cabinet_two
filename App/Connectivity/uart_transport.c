/********************************************************************************
 * @file           : uart_transport.c
 * @brief          : UART transport implementation for ESP32-C6 communications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 AllyTech LLC.
 * All rights reserved.
 ********************************************************************************/

#include "uart_transport.h"

static UART_HandleTypeDef *s_huart = NULL;
static osMessageQueueId_t s_rx_queue = NULL;
static uint8_t s_rx_byte = 0u;

HAL_StatusTypeDef UartTransport_Init(UART_HandleTypeDef *huart,
                                     osMessageQueueId_t rx_queue)
{
  if ((huart == NULL) || (rx_queue == NULL)) {
    return HAL_ERROR;
  }

  s_huart = huart;
  s_rx_queue = rx_queue;

  return HAL_OK;
}

HAL_StatusTypeDef UartTransport_StartRx(void)
{
  if (s_huart == NULL) {
    return HAL_ERROR;
  }

  return HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1u);
}

HAL_StatusTypeDef UartTransport_Transmit(const uint8_t *data,
                                         uint16_t length,
                                         uint32_t timeout_ms)
{
  if ((s_huart == NULL) || (data == NULL) || (length == 0u)) {
    return HAL_ERROR;
  }

  return HAL_UART_Transmit(s_huart, data, length, timeout_ms);
}