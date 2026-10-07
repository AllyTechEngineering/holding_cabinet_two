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
#include "esp_at_interface.h"
#include "uart_transport.h"

extern UART_HandleTypeDef huart1;
extern osMessageQueueId_t qUartRxToConnectHandle;

volatile uint8_t g_connectRxBuffer[32] = {0};
volatile uint16_t g_connectRxCount = 0u;
volatile uint32_t g_connectEchoCount = 0u;
volatile uint32_t g_connectOkCount = 0u;
volatile uint32_t g_connectErrorCount = 0u;

void ConnectTask_Run(void *argument) {
  static const uint8_t at_command[] = "AT+FAKE\r\n";

  (void)argument;
  uint8_t rx_byte;

  if (UartTransport_Init(&huart1, qUartRxToConnectHandle) != HAL_OK) {
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
      if (EspAt_LineAssemblyProcessByte(rx_byte) != 0u) {
        const uint8_t *line_buffer = EspAt_GetLineBuffer();
        uint16_t line_length = EspAt_GetLineLength();

        if (EspAt_IsCommandEcho(line_buffer, line_length, at_command,
                                sizeof(at_command) - 1u) != 0u) {
          g_connectEchoCount++;
          EspAt_ResetLineAssembly();
          continue;
        }

        if (EspAt_IsTerminalOk(line_buffer, line_length) != 0u) {
          g_connectOkCount++;
        }
        if (EspAt_IsTerminalError(line_buffer, line_length) != 0u) {
          g_connectErrorCount++;
        }
        g_connectRxCount = 0u;

        while ((g_connectRxCount < line_length) &&
               (g_connectRxCount < (sizeof(g_connectRxBuffer) - 1u))) {
          g_connectRxBuffer[g_connectRxCount] = line_buffer[g_connectRxCount];
          g_connectRxCount++;
        }

        g_connectRxBuffer[g_connectRxCount] = '\0';
        EspAt_ResetLineAssembly();
      }
    }
  }
}