/********************************************************************************
 * @file           : switch_driver.c
 * @brief          : Raw GPIO read and active-low debounce logic for the four
 * physical switches
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 AllyTech LLC.
 * All rights reserved.
 *
 *******************************************************************************/

#include "switch_driver.h"
#include "app_config.h"
#include "main.h"

#define SWITCH_COUNT 4u

static uint8_t stable_pressed_mask;
static uint8_t consecutive_reads[SWITCH_COUNT];

static uint8_t read_pressed_mask(void)
{
  uint8_t mask = 0u;

  /* The switches are active low: GPIO RESET means pressed. */
  if (HAL_GPIO_ReadPin(UpArrowSwitch_GPIO_Port,
                       UpArrowSwitch_Pin) == GPIO_PIN_RESET)
    mask |= SWITCH_UP_MASK;

  if (HAL_GPIO_ReadPin(DownArrowSwitch_GPIO_Port,
                       DownArrowSwitch_Pin) == GPIO_PIN_RESET)
    mask |= SWITCH_DOWN_MASK;

  if (HAL_GPIO_ReadPin(ModeSwitch_GPIO_Port,
                       ModeSwitch_Pin) == GPIO_PIN_RESET)
    mask |= SWITCH_MODE_MASK;

  if (HAL_GPIO_ReadPin(EnterSwitch_GPIO_Port,
                       EnterSwitch_Pin) == GPIO_PIN_RESET)
    mask |= SWITCH_ENTER_MASK;

  return mask;
}

void SwitchDriver_Init(void)
{
  stable_pressed_mask = read_pressed_mask();

  for (uint8_t i = 0u; i < SWITCH_COUNT; ++i)
  {
    consecutive_reads[i] = 0u;
  }
}

SwitchSnapshot_t SwitchDriver_Poll(void)
{
  SwitchSnapshot_t result = {0};
  uint8_t raw_pressed_mask = read_pressed_mask();

  for (uint8_t i = 0u; i < SWITCH_COUNT; ++i)
  {
    uint8_t bit = (uint8_t)(1u << i);
    uint8_t raw_pressed = raw_pressed_mask & bit;
    uint8_t stable_pressed = stable_pressed_mask & bit;

    if (raw_pressed == stable_pressed)
    {
      consecutive_reads[i] = 0u;
      continue;
    }

    if (++consecutive_reads[i] >= APP_INPUT_DEBOUNCE_COUNT)
    {
      consecutive_reads[i] = 0u;
      stable_pressed_mask ^= bit;

      if (stable_pressed_mask & bit)
        result.pressed_edges |= bit;
      else
        result.released_edges |= bit;
    }
  }

  result.pressed_mask = stable_pressed_mask;
  return result;
}