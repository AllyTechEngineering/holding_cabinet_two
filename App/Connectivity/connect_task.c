/********************************************************************************
 * @file           : connect_task.c
 * @brief          : Connectivity task implementation
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 AllyTech LLC.
 * All rights reserved.
 ********************************************************************************/

#include "connect_task.h"
#include "uart_transport.h"
#include "cmsis_os.h"

extern UART_HandleTypeDef huart2;
extern osMessageQueueId_t qUartRxToConnectHandle;

void ConnectTask_Run(void *argument) {
  (void)argument;

  if (UartTransport_Init(&huart2, qUartRxToConnectHandle) != HAL_OK) {
    for (;;) {
      osDelay(1000u);
    }
  }

  if (UartTransport_StartRx() != HAL_OK) {
    for (;;) {
      osDelay(1000u);
    }
  }
}