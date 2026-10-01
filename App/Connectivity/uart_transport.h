/********************************************************************************
 * @file           : uart_transport.h
 * @brief          : Header for uart_transport.c
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 AllyTech LLC.
 * All rights reserved.
 ********************************************************************************/

#ifndef __UART_TRANSPORT_H
#define __UART_TRANSPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"
#include "stm32l4xx_hal.h"

HAL_StatusTypeDef UartTransport_Init(UART_HandleTypeDef *huart,
                                     osMessageQueueId_t rx_queue);

HAL_StatusTypeDef UartTransport_StartRx(void);

HAL_StatusTypeDef UartTransport_Transmit(const uint8_t *data,
                                         uint16_t length,
                                         uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* __UART_TRANSPORT_H */
