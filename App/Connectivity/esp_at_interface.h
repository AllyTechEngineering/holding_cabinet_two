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

typedef enum {
  ESP_AT_TRANSACTION_IDLE = 0,
  ESP_AT_TRANSACTION_ACTIVE,
  ESP_AT_TRANSACTION_OK,
  ESP_AT_TRANSACTION_ERROR
} EspAtTransactionState;

uint8_t EspAt_LineAssemblyProcessByte(uint8_t rx_byte);
uint8_t EspAt_IsCommandEcho(const uint8_t *line, uint16_t line_length,
                            const uint8_t *command, uint16_t command_length);
uint8_t EspAt_IsTerminalOk(const uint8_t *line, uint16_t line_length);
uint8_t EspAt_IsTerminalError(const uint8_t *line, uint16_t line_length);
const uint8_t *EspAt_GetLineBuffer(void);
uint16_t EspAt_GetLineLength(void);
void EspAt_ResetLineAssembly(void);

uint8_t EspAt_StartTransaction(void);
void EspAt_ProcessTransactionLine(const uint8_t *line, uint16_t line_length);
EspAtTransactionState EspAt_GetTransactionState(void);
void EspAt_ResetTransaction(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_AT_INTERFACE_H */