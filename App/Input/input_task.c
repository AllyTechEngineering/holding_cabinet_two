/********************************************************************************
  * @file           : input_task.c
  * @brief          : FreeRTOS task debouncing the four physical switches and detecting the Settings-entry chord
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  *******************************************************************************/

#include "input_task.h"
#include "switch_driver.h"
#include "app_config.h"
#include "app_types.h"
#include "cmsis_os.h"
#include "main.h"

extern osMessageQueueId_t qInputToDisplayHandle;

static const ButtonEvent_t pressed_events[4] = {
  EVT_UP_PRESSED,
  EVT_DOWN_PRESSED,
  EVT_MODE_PRESSED,
  EVT_ENTER_PRESSED
};

static const ButtonEvent_t released_events[4] = {
  EVT_UP_RELEASED,
  EVT_DOWN_RELEASED,
  EVT_MODE_RELEASED,
  EVT_ENTER_RELEASED
};

static void send_event(ButtonEvent_t event)
{
  /* Button edges are discrete events: wait for space rather than
     replace an event already in the FIFO. */
  if (osMessageQueuePut(qInputToDisplayHandle,
                        &event, 0U, osWaitForever) != osOK)
  {
    HAL_GPIO_WritePin(HeatRelay_GPIO_Port,
                      HeatRelay_Pin, GPIO_PIN_SET);
    Error_Handler();
  }
}

void InputTask_Run(void *argument)
{
  uint8_t chord_tracking = 0u;
  uint8_t chord_sent = 0u;
  uint32_t chord_start_tick = 0u;

  (void)argument;
  SwitchDriver_Init();

  for (;;)
  {
    SwitchSnapshot_t switches = SwitchDriver_Poll();

    for (uint8_t i = 0u; i < 4u; ++i)
    {
      uint8_t bit = (uint8_t)(1u << i);

      if ((switches.pressed_edges & bit) != 0u)
        send_event(pressed_events[i]);

      if ((switches.released_edges & bit) != 0u)
        send_event(released_events[i]);
    }

    if ((switches.pressed_mask &
         (SWITCH_UP_MASK | SWITCH_DOWN_MASK)) ==
        (SWITCH_UP_MASK | SWITCH_DOWN_MASK))
    {
      if (chord_tracking == 0u)
      {
        chord_tracking = 1u;
        chord_start_tick = osKernelGetTickCount();
      }
      else if (chord_sent == 0u &&
               (uint32_t)(osKernelGetTickCount() - chord_start_tick) >=
                   APP_SETTINGS_CHORD_MS)
      {
        send_event(EVT_ENTER_SETTINGS);
        chord_sent = 1u;
      }
    }
    else
    {
      chord_tracking = 0u;
      chord_sent = 0u;
    }

    osDelay(APP_INPUT_POLL_MS);
  }
}