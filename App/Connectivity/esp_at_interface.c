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

static uint8_t s_esp_at_line_buffer[ESP_AT_LINE_BUFFER_SIZE] = {0};
static uint16_t s_esp_at_line_length = 0u;
static uint8_t s_esp_at_pending_cr = 0u;


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
uint8_t EspAt_LineAssemblyProcessByte(uint8_t rx_byte)
{
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
 * @brief Returns the current assembled ESP-AT line buffer.
 *
 * @return Pointer to the line buffer.
 */
const uint8_t *EspAt_GetLineBuffer(void)
{
  return s_esp_at_line_buffer;
}


/**
 * @brief Returns the current assembled ESP-AT line length.
 *
 * @return Number of bytes in the assembled line.
 */
uint16_t EspAt_GetLineLength(void)
{
  return s_esp_at_line_length;
}


/**
 * @brief Resets ESP-AT line assembly state for the next line.
 */
void EspAt_ResetLineAssembly(void)
{
  s_esp_at_line_length = 0u;
  s_esp_at_pending_cr = 0u;
}