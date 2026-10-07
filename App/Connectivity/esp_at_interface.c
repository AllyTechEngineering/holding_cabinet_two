/********************************************************************************
 * @file           : esp_at_interface.c
 * @brief          : ESP-AT interface implementation
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 AllyTech LLC.
 * All rights reserved.
 ********************************************************************************/

#include "esp_at_interface.h"
#include <stddef.h>

static uint8_t s_esp_at_line_buffer[ESP_AT_LINE_BUFFER_SIZE] = {0};
static uint16_t s_esp_at_line_length = 0u;
static uint8_t s_esp_at_pending_cr = 0u;
static EspAtTransactionState s_esp_at_transaction_state =
    ESP_AT_TRANSACTION_IDLE;

/**
 * @brief Processes one received ESP-AT byte for response-line assembly.
 *
 * Builds non-empty response lines terminated by CR-LF. Empty CR-LF
 * sequences are ignored.
 *
 * @param rx_byte Received UART byte.
 *
 * @return 1u when a complete non-empty line is available; 0u otherwise.
 */
uint8_t EspAt_LineAssemblyProcessByte(uint8_t rx_byte) {
  if (rx_byte == '\r') {
    s_esp_at_pending_cr = 1u;
    return 0u;
  }

  if ((rx_byte == '\n') && (s_esp_at_pending_cr != 0u)) {
    s_esp_at_pending_cr = 0u;

    if (s_esp_at_line_length == 0u) {
      return 0u;
    }

    s_esp_at_line_buffer[s_esp_at_line_length] = '\0';
    return 1u;
  }

  if (s_esp_at_pending_cr != 0u) {
    s_esp_at_pending_cr = 0u;

    if (s_esp_at_line_length < (ESP_AT_LINE_BUFFER_SIZE - 1u)) {
      s_esp_at_line_buffer[s_esp_at_line_length] = '\r';
      s_esp_at_line_length++;
    }
  }

  if (s_esp_at_line_length < (ESP_AT_LINE_BUFFER_SIZE - 1u)) {
    s_esp_at_line_buffer[s_esp_at_line_length] = rx_byte;
    s_esp_at_line_length++;
  }

  return 0u;
}
/**
 * @brief Determines whether a received line is the echo of a transmitted
 * command.
 *
 * The command length includes the trailing CR-LF. The received line does not
 * include CR-LF because line assembly removes the terminator.
 *
 * @param line Received ESP-AT line.
 * @param line_length Length of the received line.
 * @param command Transmitted ESP-AT command.
 * @param command_length Length of the transmitted command including CR-LF.
 *
 * @return 1u when the line matches the transmitted command; 0u otherwise.
 */
uint8_t EspAt_IsCommandEcho(const uint8_t *line, uint16_t line_length,
                            const uint8_t *command, uint16_t command_length) {
  uint16_t command_text_length;

  if ((line == NULL) || (command == NULL) || (command_length < 2u)) {
    return 0u;
  }

  command_text_length = command_length - 2u;

  if (line_length != command_text_length) {
    return 0u;
  }

  for (uint16_t i = 0u; i < line_length; i++) {
    if (line[i] != command[i]) {
      return 0u;
    }
  }

  return 1u;
}

/**
 * @brief Determines whether a received line is the ESP-AT terminal OK response.
 *
 * @param line Received ESP-AT line.
 * @param line_length Length of the received line.
 *
 * @return 1u when the line is exactly "OK"; 0u otherwise.
 */
uint8_t EspAt_IsTerminalOk(const uint8_t *line, uint16_t line_length) {
  if ((line == NULL) || (line_length != 2u)) {
    return 0u;
  }

  if ((line[0] == 'O') && (line[1] == 'K')) {
    return 1u;
  }

  return 0u;
}

/**
 * @brief Determines whether a received line is the ESP-AT terminal ERROR
 * response.
 *
 * @param line Received ESP-AT line.
 * @param line_length Length of the received line.
 *
 * @return 1u when the line is exactly "ERROR"; 0u otherwise.
 */
uint8_t EspAt_IsTerminalError(const uint8_t *line, uint16_t line_length) {
  if ((line == NULL) || (line_length != 5u)) {
    return 0u;
  }

  if ((line[0] == 'E') && (line[1] == 'R') && (line[2] == 'R') &&
      (line[3] == 'O') && (line[4] == 'R')) {
    return 1u;
  }

  return 0u;
}

/**
 * @brief Starts a new response-dependent ESP-AT transaction.
 *
 * @return 1u when the transaction was started; 0u when a transaction is
 * already active.
 */
uint8_t EspAt_StartTransaction(uint32_t timeout_ms) {
    (void)timeout_ms;
  if (s_esp_at_transaction_state == ESP_AT_TRANSACTION_ACTIVE) {
    return 0u;
  }

  s_esp_at_transaction_state = ESP_AT_TRANSACTION_ACTIVE;
  return 1u;
}

/**
 * @brief Processes a received line for the active ESP-AT transaction.
 *
 * @param line Received ESP-AT line.
 * @param line_length Length of the received line.
 */
void EspAt_ProcessTransactionLine(const uint8_t *line, uint16_t line_length) {
  if (s_esp_at_transaction_state != ESP_AT_TRANSACTION_ACTIVE) {
    return;
  }

  if (EspAt_IsTerminalOk(line, line_length) != 0u) {
    s_esp_at_transaction_state = ESP_AT_TRANSACTION_OK;
  } else if (EspAt_IsTerminalError(line, line_length) != 0u) {
    s_esp_at_transaction_state = ESP_AT_TRANSACTION_ERROR;
  }
}

/**
 * @brief Returns the current ESP-AT transaction state.
 *
 * @return Current transaction state.
 */
EspAtTransactionState EspAt_GetTransactionState(void) {
  return s_esp_at_transaction_state;
}
/**
 * @brief Resets the ESP-AT transaction state to idle.
 */
void EspAt_ResetTransaction(void)
{
  s_esp_at_transaction_state = ESP_AT_TRANSACTION_IDLE;
}
/**
 * @brief Returns the current assembled ESP-AT line buffer.
 *
 * @return Pointer to the line buffer.
 */
const uint8_t *EspAt_GetLineBuffer(void) { return s_esp_at_line_buffer; }

/**
 * @brief Returns the current assembled ESP-AT line length.
 *
 * @return Number of bytes in the assembled line.
 */
uint16_t EspAt_GetLineLength(void) { return s_esp_at_line_length; }

/**
 * @brief Resets ESP-AT line assembly state for the next line.
 */
void EspAt_ResetLineAssembly(void) {
  s_esp_at_line_length = 0u;
  s_esp_at_pending_cr = 0u;
}