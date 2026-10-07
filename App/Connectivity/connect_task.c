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
#include "cmsis_os.h"
#include "uart_transport.h"


extern UART_HandleTypeDef huart2;
extern osMessageQueueId_t qUartRxToConnectHandle;

volatile uint8_t g_connectRxBuffer[32] = {0};
volatile uint16_t g_connectRxCount = 0u;

void ConnectTask_Run(void *argument) {
  static const uint8_t at_command[] = "AT\r\n";

  (void)argument;
  uint8_t rx_byte;

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

  osDelay(1000u);

  if (UartTransport_Transmit(at_command, sizeof(at_command) - 1u, 100u) !=
      HAL_OK) {
    for (;;) {
      osDelay(1000u);
    }
  }

  for (;;) {
    if (osMessageQueueGet(qUartRxToConnectHandle, &rx_byte, NULL,
                          osWaitForever) == osOK) {
      if (g_connectRxCount < sizeof(g_connectRxBuffer)) {
        g_connectRxBuffer[g_connectRxCount] = rx_byte;
        g_connectRxCount++;
      }
    }
  }
}