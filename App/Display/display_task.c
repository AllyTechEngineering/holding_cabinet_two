/********************************************************************************
  * @file           : display_task.c
  * @brief          : FreeRTOS task managing LCD1602 display updates
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  *******************************************************************************/

#include "display_task.h"
#include "lcd1602_driver.h"
#include "app_config.h"
#include "app_types.h"
#include "cmsis_os.h"
#include <stdint.h>

extern osMessageQueueId_t qInputToDisplayHandle;

void DisplayTask_Run(void *argument)
{
  uint8_t event;
  uint8_t show_prompt = 0u;
  uint8_t in_temp_decision = 0u;
  uint32_t screen_start_tick;
  uint32_t last_activity_tick;

  (void)argument;

  LCD1602_Init();

  /* Power-on state is always Idle-Splash. */
  LCD1602_WriteLines("     Taylor     ",
                     " Proofing Oven  ");
  screen_start_tick = osKernelGetTickCount();
  last_activity_tick = screen_start_tick;

  for (;;)
  {
    if (osMessageQueueGet(qInputToDisplayHandle, &event, NULL,
                          APP_DISPLAY_TASK_WAKE_MS) == osOK)
    {
      if (event == EVT_MODE_PRESSED)
      {
        if (in_temp_decision == 0u)
        {
          /* Mode from either Idle screen starts a new proof setup. */
          in_temp_decision = 1u;
          last_activity_tick = osKernelGetTickCount();

          LCD1602_WriteLines(" To Set Temp    ",
                             " Enter Y Mode N ");
        }
        else
        {
          /* Mode at SetTemp-Decision returns to Idle-Splash. */
          in_temp_decision = 0u;
          show_prompt = 0u;

          LCD1602_WriteLines("     Taylor     ",
                             " Proofing Oven  ");
          screen_start_tick = osKernelGetTickCount();
        }
      }
      else if (in_temp_decision != 0u &&
               (event == EVT_UP_PRESSED ||
                event == EVT_DOWN_PRESSED ||
                event == EVT_ENTER_PRESSED))
      {
        last_activity_tick = osKernelGetTickCount();
      }
    }

    if (in_temp_decision != 0u)
    {
      if ((uint32_t)(osKernelGetTickCount() - last_activity_tick) >=
          APP_INACTIVITY_TIMEOUT_MS)
      {
        in_temp_decision = 0u;
        show_prompt = 0u;

        LCD1602_WriteLines("     Taylor     ",
                           " Proofing Oven  ");
        screen_start_tick = osKernelGetTickCount();
      }
    }
    else if ((uint32_t)(osKernelGetTickCount() - screen_start_tick) >=
             APP_TOGGLE_PAIR_MS)
    {
      /* Only the two Idle screens alternate. */
      show_prompt = (uint8_t)!show_prompt;

      if (show_prompt != 0u)
      {
        LCD1602_WriteLines(" To Start       ",
                           " Press Mode     ");
      }
      else
      {
        LCD1602_WriteLines("     Taylor     ",
                           " Proofing Oven  ");
      }

      screen_start_tick = osKernelGetTickCount();
    }
  }
}