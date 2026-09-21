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

typedef enum
{
  UI_IDLE_SPLASH,
  UI_IDLE_PROMPT,
  UI_TEMP_DECISION,
  UI_TEMP_ADJUST,
  UI_TEMP_CONFIRM
} UiScreen_t;

static void display_screen(UiScreen_t screen, uint16_t temp_value,
                           uint8_t unit_celsius)
{
  if (screen == UI_IDLE_SPLASH)
  {
    LCD1602_WriteLines("     Taylor     ",
                       " Proofing Oven  ");
  }
  else if (screen == UI_IDLE_PROMPT)
  {
    LCD1602_WriteLines(" To Start       ",
                       " Press Mode     ");
  }
  else if (screen == UI_TEMP_DECISION)
  {
    LCD1602_WriteLines(" To Set Temp    ",
                       " Enter Y Mode N ");
  }
  else
  {
    /* Positions 12–14 hold the right-aligned, whole-degree value. */
    char row1[17] = " Set Temp:    F ";

    row1[11] = (temp_value >= 100u)
                 ? (char)('0' + (temp_value / 100u))
                 : ' ';
    row1[12] = (char)('0' + ((temp_value / 10u) % 10u));
    row1[13] = (char)('0' + (temp_value % 10u));
    row1[14] = (unit_celsius != 0u) ? 'C' : 'F';

    if (screen == UI_TEMP_ADJUST)
    {
      LCD1602_WriteLines(row1, " Up+ or Down-   ");
    }
    else
    {
      LCD1602_WriteLines(row1, "Enter Y Mode N  ");
    }
  }
}

void DisplayTask_Run(void *argument)
{
  UiScreen_t screen = UI_IDLE_SPLASH;
  uint8_t event;
  uint8_t unit_celsius = 0u; /* Fahrenheit until Settings is implemented. */
  uint16_t proposed_temp =
      (uint16_t)((APP_TEMP_DEFAULT_TENTHS_C * 9u + 25u) / 50u + 32u);
  uint32_t screen_start_tick;
  uint32_t last_activity_tick;

  (void)argument;

  LCD1602_Init();
  display_screen(screen, proposed_temp, unit_celsius);
  screen_start_tick = osKernelGetTickCount();
  last_activity_tick = screen_start_tick;

  for (;;)
  {
    if (osMessageQueueGet(qInputToDisplayHandle, &event, NULL,
                          APP_DISPLAY_TASK_WAKE_MS) == osOK)
    {
      uint32_t now = osKernelGetTickCount();

      if (screen != UI_IDLE_SPLASH && screen != UI_IDLE_PROMPT &&
          (event == EVT_UP_PRESSED || event == EVT_DOWN_PRESSED ||
           event == EVT_MODE_PRESSED || event == EVT_ENTER_PRESSED))
      {
        last_activity_tick = now;
      }

      if (screen == UI_IDLE_SPLASH || screen == UI_IDLE_PROMPT)
      {
        if (event == EVT_MODE_PRESSED)
        {
          screen = UI_TEMP_DECISION;
          last_activity_tick = now;
          display_screen(screen, proposed_temp, unit_celsius);
        }
      }
      else if (screen == UI_TEMP_DECISION)
      {
        if (event == EVT_MODE_PRESSED)
        {
          screen = UI_IDLE_SPLASH;
          display_screen(screen, proposed_temp, unit_celsius);
          screen_start_tick = osKernelGetTickCount();
        }
        else if (event == EVT_ENTER_PRESSED)
        {
          proposed_temp = unit_celsius != 0u
                            ? (uint16_t)((APP_TEMP_DEFAULT_TENTHS_C + 5u) / 10u)
                            : (uint16_t)((APP_TEMP_DEFAULT_TENTHS_C * 9u + 25u)
                                         / 50u + 32u);
          screen = UI_TEMP_ADJUST;
          display_screen(screen, proposed_temp, unit_celsius);
          screen_start_tick = osKernelGetTickCount();
        }
      }
      else /* SetTemp-Adjust or SetTemp-Confirm */
      {
        if (event == EVT_MODE_PRESSED)
        {
          /* New proof: discard the proposal and return to Idle. */
          screen = UI_IDLE_SPLASH;
          display_screen(screen, proposed_temp, unit_celsius);
          screen_start_tick = osKernelGetTickCount();
        }
        else if (event == EVT_UP_PRESSED ||
                 event == EVT_DOWN_PRESSED)
        {
          /* Celsius limits stay within the documented 65–120°F range. */
          uint16_t minimum = unit_celsius != 0u
                               ? (uint16_t)((APP_TEMP_MIN_TENTHS_C + 9u) / 10u)
                               : 65u;
          uint16_t maximum = unit_celsius != 0u
                               ? (uint16_t)(APP_TEMP_MAX_TENTHS_C / 10u)
                               : 120u;

          if (event == EVT_UP_PRESSED && proposed_temp < maximum)
          {
            ++proposed_temp;
            display_screen(screen, proposed_temp, unit_celsius);
          }
          else if (event == EVT_DOWN_PRESSED && proposed_temp > minimum)
          {
            --proposed_temp;
            display_screen(screen, proposed_temp, unit_celsius);
          }

          /* Up/Down does not restart the screen-pair timer. */
        }
      }
    }

    uint32_t now = osKernelGetTickCount();

    if (screen != UI_IDLE_SPLASH && screen != UI_IDLE_PROMPT &&
        (uint32_t)(now - last_activity_tick) >=
            APP_INACTIVITY_TIMEOUT_MS)
    {
      screen = UI_IDLE_SPLASH;
      display_screen(screen, proposed_temp, unit_celsius);
      screen_start_tick = osKernelGetTickCount();
    }
    else if ((screen == UI_IDLE_SPLASH || screen == UI_IDLE_PROMPT ||
              screen == UI_TEMP_ADJUST || screen == UI_TEMP_CONFIRM) &&
             (uint32_t)(now - screen_start_tick) >= APP_TOGGLE_PAIR_MS)
    {
      if (screen == UI_IDLE_SPLASH)
        screen = UI_IDLE_PROMPT;
      else if (screen == UI_IDLE_PROMPT)
        screen = UI_IDLE_SPLASH;
      else if (screen == UI_TEMP_ADJUST)
        screen = UI_TEMP_CONFIRM;
      else
        screen = UI_TEMP_ADJUST;

      display_screen(screen, proposed_temp, unit_celsius);
      screen_start_tick = osKernelGetTickCount();
    }
  }
}