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
#include "cmsis_os.h"
#include <stdint.h>

extern osMessageQueueId_t qInputToDisplayHandle;

void DisplayTask_Run(void *argument)
{
  uint8_t event;
  uint8_t show_prompt = 0u;
  uint32_t screen_start_tick;

  (void)argument;

  LCD1602_Init();

  /* Power-on screen: Idle-Splash. Each row is exactly 16 characters. */
  LCD1602_WriteLines("     Taylor     ",
                     " Proofing Oven  ");
  screen_start_tick = osKernelGetTickCount();

  for (;;)
  {
    /* Wake for button events or at least every 100 ms for screen timing. */
    (void)osMessageQueueGet(qInputToDisplayHandle, &event, NULL,
                            APP_DISPLAY_TASK_WAKE_MS);

    /* Idle-Splash and Idle-Prompt alternate every two seconds. */
    if ((uint32_t)(osKernelGetTickCount() - screen_start_tick) >=
        APP_TOGGLE_PAIR_MS)
    {
      show_prompt = (uint8_t)!show_prompt;

      if (show_prompt != 0u)
      {
        LCD1602_WriteLines(" To Start Press ",
                           " Mode           ");
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