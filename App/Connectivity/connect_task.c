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
#include "connectivity_logic.h"
#include "esp_at_interface.h"
#include "uart_transport.h"

extern UART_HandleTypeDef huart1;
extern osMessageQueueId_t qUartRxToConnectHandle;

void ConnectTask_Run(void *argument)
{
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

  if (ConnectivityLogic_Start() == 0u) {
    for (;;) {
      osDelay(1000u);
    }
  }

  for (;;) {
    if (osMessageQueueGet(qUartRxToConnectHandle, &rx_byte, NULL, 10u) ==
        osOK) {
      if (EspAt_LineAssemblyProcessByte(rx_byte) != 0u) {
        const uint8_t *line_buffer = EspAt_GetLineBuffer();
        uint16_t line_length = EspAt_GetLineLength();

        if (EspAt_IsActiveCommandEcho(line_buffer, line_length) != 0u) {
          EspAt_ResetLineAssembly();
          continue;
        }

        EspAtUrcType urc = EspAt_ClassifyUrc(line_buffer, line_length);

        if (urc != ESP_AT_URC_NONE) {
          ConnectivityLogic_HandleUrc(urc);
          EspAt_ResetLineAssembly();
          continue;
        }

        EspAt_ProcessTransactionLine(line_buffer, line_length);
        EspAt_ResetLineAssembly();
      }
    }

    ConnectivityLogic_Process();
  }
}