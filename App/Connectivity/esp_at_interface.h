/********************************************************************************
 * @file           : esp_at_interface.h
 * @brief          : ESP-AT interface definitions
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 AllyTech LLC.
 * All rights reserved.
 ********************************************************************************/

#ifndef __ESP_AT_INTERFACE_H
#define __ESP_AT_INTERFACE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_AT_LINE_BUFFER_SIZE 128u
#define ESP_AT_COMMAND_BUFFER_SIZE 128u

typedef enum {
  ESP_AT_TRANSACTION_IDLE = 0,
  ESP_AT_TRANSACTION_ACTIVE,
  ESP_AT_TRANSACTION_OK,
  ESP_AT_TRANSACTION_ERROR,
  ESP_AT_TRANSACTION_TIMEOUT,
} EspAtTransactionState;

typedef enum {
  ESP_AT_URC_NONE = 0,
  ESP_AT_URC_READY,
  ESP_AT_URC_WIFI_CONNECTED,
  ESP_AT_URC_WIFI_GOT_IP,
  ESP_AT_URC_WIFI_DISCONNECT,
} EspAtUrcType;

uint8_t EspAt_StartCommand(const uint8_t *command, uint16_t command_length,
                           uint32_t timeout_ms);
uint8_t EspAt_IsActiveCommandEcho(const uint8_t *line, uint16_t line_length);

uint8_t EspAt_LineAssemblyProcessByte(uint8_t rx_byte);
uint8_t EspAt_IsCommandEcho(const uint8_t *line, uint16_t line_length,
                            const uint8_t *command, uint16_t command_length);
uint8_t EspAt_IsTerminalOk(const uint8_t *line, uint16_t line_length);
uint8_t EspAt_IsTerminalError(const uint8_t *line, uint16_t line_length);
EspAtUrcType EspAt_ClassifyUrc(const uint8_t *line, uint16_t line_length);
const uint8_t *EspAt_GetLineBuffer(void);
uint16_t EspAt_GetLineLength(void);
void EspAt_ResetLineAssembly(void);

uint8_t EspAt_StartTransaction(uint32_t timeout_ms);
void EspAt_ProcessTransactionLine(const uint8_t *line, uint16_t line_length);
void EspAt_ProcessTransactionTimeout(void);
EspAtTransactionState EspAt_GetTransactionState(void);
void EspAt_ResetTransaction(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_AT_INTERFACE_H */