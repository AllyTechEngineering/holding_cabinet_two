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

/**
 * @brief Initializes the UART transport layer.
 *
 * Associates the transport with the UART peripheral and receive queue used
 * for ESP32-C6 communications.
 *
 * @param huart UART handle used by the transport.
 * @param rx_queue Queue that receives incoming UART bytes.
 *
 * @return HAL_OK if initialization succeeds; HAL_ERROR otherwise.
 */
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

/**
 * @brief Starts interrupt-driven UART reception.
 *
 * Arms the UART to receive one byte. Subsequent reception is rearmed by the
 * UART receive-complete callback.
 *
 * @return HAL status returned by HAL_UART_Receive_IT().
 */
HAL_StatusTypeDef UartTransport_StartRx(void)
{
  if (s_huart == NULL) {
    return HAL_ERROR;
  }

  return HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1u);
}

/**
 * @brief Transmits data through the UART transport.
 *
 * Transmission is blocking for the requested data length or until the
 * specified timeout expires.
 *
 * @param data Pointer to the data to transmit.
 * @param length Number of bytes to transmit.
 * @param timeout_ms Maximum transmit wait time in milliseconds.
 *
 * @return HAL status returned by HAL_UART_Transmit(), or HAL_ERROR if the
 *         transport or transmit parameters are invalid.
 */
HAL_StatusTypeDef UartTransport_Transmit(const uint8_t *data,
                                         uint16_t length,
                                         uint32_t timeout_ms)
{
  if ((s_huart == NULL) || (data == NULL) || (length == 0u)) {
    return HAL_ERROR;
  }

  return HAL_UART_Transmit(s_huart, data, length, timeout_ms);
}

/**
 * @brief Handles completion of an interrupt-driven UART receive operation.
 *
 * Places the received byte into the connectivity receive queue and rearms
 * reception for the next byte.
 *
 * @param huart UART handle associated with the completed receive operation.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if ((huart != s_huart) || (s_rx_queue == NULL)) {
    return;
  }

  (void)osMessageQueuePut(s_rx_queue, &s_rx_byte, 0u, 0u);
  (void)HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1u);
}

/**
 * @brief Handles UART errors for the connectivity transport.
 *
 * Rearms interrupt-driven reception after a UART error.
 *
 * @param huart UART handle associated with the reported error.
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart != s_huart) {
    return;
  }

  (void)HAL_UART_Receive_IT(s_huart, &s_rx_byte, 1u);
}