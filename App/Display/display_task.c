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
#include "app_config.h"
#include "app_types.h"
#include "cmsis_os.h"
#include "lcd1602_driver.h"
#include "run_timer.h"
#include "time_editor.h"
#include <stdint.h>


extern osMessageQueueId_t qInputToDisplayHandle;
extern osMessageQueueId_t qHeatToDisplayHandle;

typedef enum {
  UI_IDLE_SPLASH,
  UI_IDLE_PROMPT,
  UI_TEMP_DECISION,
  UI_TEMP_ADJUST,
  UI_TEMP_CONFIRM,
  UI_TIME_DECISION,
  UI_TIME_ADJUST,
  UI_TIME_CONFIRM,
  UI_RUN_DECISION UI_RUN_ACTIVE
} UiScreen_t;

static void display_screen(UiScreen_t screen, uint16_t temp_value,
                           uint8_t unit_celsius) {
  if (screen == UI_IDLE_SPLASH) {
    LCD1602_WriteLines("     Taylor     ", " Proofing Oven  ");
  } else if (screen == UI_IDLE_PROMPT) {
    LCD1602_WriteLines(" To Start       ", " Press Mode     ");
  } else if (screen == UI_TEMP_DECISION) {
    LCD1602_WriteLines(" To Set Temp    ", " Enter Y Mode N ");
  } else if (screen == UI_TIME_DECISION) {
    LCD1602_WriteLines("Countdown Timer ", "Enter Y Mode N  ");
  } else if (screen == UI_RUN_DECISION) {
    LCD1602_WriteLines(" To Start Proof ", " Enter Y Mode N ");
  } else {
    /* Positions 12–14 hold the right-aligned, whole-degree value. */
    char row1[17] = " Set Temp:    F ";

    row1[11] = (temp_value >= 100u) ? (char)('0' + (temp_value / 100u)) : ' ';
    row1[12] = (char)('0' + ((temp_value / 10u) % 10u));
    row1[13] = (char)('0' + (temp_value % 10u));
    row1[14] = (unit_celsius != 0u) ? 'C' : 'F';

    if (screen == UI_TEMP_ADJUST) {
      LCD1602_WriteLines(row1, " Up+ or Down-   ");
    } else {
      LCD1602_WriteLines(row1, "Enter Y Mode N  ");
    }
  }
}

static void display_time_screen(UiScreen_t screen, const TimeEditor_t *editor) {
  uint16_t total_minutes = TimeEditor_Minutes(editor);
  uint16_t hours = (uint16_t)(total_minutes / 60u);
  uint16_t minutes = (uint16_t)(total_minutes % 60u);
  char row1[17] = " Set Time:      ";

  row1[11] = hours >= 10u ? (char)('0' + (hours / 10u)) : ' ';
  row1[12] = (char)('0' + (hours % 10u));
  row1[13] = ':';
  row1[14] = (char)('0' + (minutes / 10u));
  row1[15] = (char)('0' + (minutes % 10u));

  LCD1602_WriteLines(row1, screen == UI_TIME_ADJUST ? " Up+ or Down-   "
                                                    : " Enter Y Mode N ");
}
static void display_run_screen(const RunTimer_t *timer,
                               const HeatStatus_t *status, uint32_t now_ms,
                               uint8_t unit_celsius) {
  char row1[17] = " Temp: ---F     ";
  char row2[17] = " Time:          ";
  uint16_t reading = status->currentTempTenthsC;

  row1[10] = unit_celsius != 0u ? 'C' : 'F';

  if (status->errorCode == 0u && reading != 0u && reading < 1000u) {
    uint16_t temp = unit_celsius != 0u
                        ? (uint16_t)((reading + 5u) / 10u)
                        : (uint16_t)((reading * 9u + 25u) / 50u + 32u);

    row1[7] = temp >= 100u ? (char)('0' + temp / 100u) : ' ';
    row1[8] = (char)('0' + (temp / 10u) % 10u);
    row1[9] = (char)('0' + temp % 10u);
  }

  if (RunTimer_Mode(timer) == RUN_TIMER_TIMED) {
    uint16_t remaining = RunTimer_RemainingMinutes(timer, now_ms);
    uint16_t hours = (uint16_t)(remaining / 60u);
    uint16_t minutes = (uint16_t)(remaining % 60u);

    row2[7] = hours >= 10u ? (char)('0' + hours / 10u) : ' ';
    row2[8] = (char)('0' + hours % 10u);
    row2[9] = ':';
    row2[10] = (char)('0' + minutes / 10u);
    row2[11] = (char)('0' + minutes % 10u);
    LCD1602_WriteLines(row1, row2);
  } else {
    uint32_t elapsed = (uint32_t)(now_ms - timer->start_ms);
    LCD1602_WriteLines(row1, ((elapsed / APP_TIME_TOGGLE_PAIR_MS) % 2u) == 0u
                                 ? "Countdown Timer "
                                 : "Not Used        ");
  }
}

void DisplayTask_Run(void *argument) {
  UiScreen_t screen = UI_IDLE_SPLASH;
  TimeEditor_t time_editor;
  RunTimer_t run_timer;
  HeatStatus_t latest_heat_status = {0};
  uint8_t event;
  uint8_t timed_proof = 0u;
  uint8_t unit_celsius = 0u; /* Fahrenheit until Settings is implemented. */
  uint16_t proposed_temp =
      (uint16_t)((APP_TEMP_DEFAULT_TENTHS_C * 9u + 25u) / 50u + 32u);
  uint32_t screen_start_tick;
  uint32_t last_activity_tick;

  (void)argument;

  TimeEditor_Init(&time_editor);
  RunTimer_Init(&run_timer);
  LCD1602_Init();
  display_screen(screen, proposed_temp, unit_celsius);
  screen_start_tick = osKernelGetTickCount();
  last_activity_tick = screen_start_tick;

  for (;;) {
    while (osMessageQueueGet(qHeatToDisplayHandle, &latest_heat_status, NULL,
                             0u) == osOK) {
      /* Keep the newest status for the Run screen. */
    }

    if (osMessageQueueGet(qInputToDisplayHandle, &event, NULL,
                          APP_DISPLAY_TASK_WAKE_MS) == osOK) {
      uint32_t now = osKernelGetTickCount();

      if (screen != UI_IDLE_SPLASH && screen != UI_IDLE_PROMPT &&
          (event == EVT_UP_PRESSED || event == EVT_DOWN_PRESSED ||
           event == EVT_MODE_PRESSED || event == EVT_ENTER_PRESSED)) {
        last_activity_tick = now;
      }

      if (screen == UI_IDLE_SPLASH || screen == UI_IDLE_PROMPT) {
        if (event == EVT_MODE_PRESSED) {
          timed_proof = 0u;
          screen = UI_TEMP_DECISION;
          last_activity_tick = now;
          display_screen(screen, proposed_temp, unit_celsius);
        }
      } else if (screen == UI_TEMP_DECISION) {
        if (event == EVT_MODE_PRESSED) {
          screen = UI_IDLE_SPLASH;
          display_screen(screen, proposed_temp, unit_celsius);
          screen_start_tick = now;
        } else if (event == EVT_ENTER_PRESSED) {
          proposed_temp =
              unit_celsius != 0u
                  ? (uint16_t)((APP_TEMP_DEFAULT_TENTHS_C + 5u) / 10u)
                  : (uint16_t)((APP_TEMP_DEFAULT_TENTHS_C * 9u + 25u) / 50u +
                               32u);
          screen = UI_TEMP_ADJUST;
          display_screen(screen, proposed_temp, unit_celsius);
          screen_start_tick = now;
        }
      } else if (screen == UI_TEMP_ADJUST || screen == UI_TEMP_CONFIRM) {
        if (event == EVT_MODE_PRESSED) {
          /* New proof: discard the proposal and return to Idle. */
          screen = UI_IDLE_SPLASH;
          display_screen(screen, proposed_temp, unit_celsius);
          screen_start_tick = now;
        } else if (event == EVT_ENTER_PRESSED) {
          /* Retain the proposed temperature for Run-Decision. */
          screen = UI_TIME_DECISION;
          display_screen(screen, proposed_temp, unit_celsius);
        } else if (event == EVT_UP_PRESSED || event == EVT_DOWN_PRESSED) {
          uint16_t minimum =
              unit_celsius != 0u
                  ? (uint16_t)((APP_TEMP_MIN_TENTHS_C + 9u) / 10u)
                  : 65u;
          uint16_t maximum = unit_celsius != 0u
                                 ? (uint16_t)(APP_TEMP_MAX_TENTHS_C / 10u)
                                 : 120u;

          if (event == EVT_UP_PRESSED && proposed_temp < maximum) {
            ++proposed_temp;
            display_screen(screen, proposed_temp, unit_celsius);
          } else if (event == EVT_DOWN_PRESSED && proposed_temp > minimum) {
            --proposed_temp;
            display_screen(screen, proposed_temp, unit_celsius);
          }

          /* Up/Down does not restart the screen-pair timer. */
        }
      } else if (screen == UI_TIME_DECISION) {
        if (event == EVT_MODE_PRESSED) {
          /* Skip the timer: this would be an untimed proof. */
          screen = UI_RUN_DECISION;
          display_screen(screen, proposed_temp, unit_celsius);
        } else if (event == EVT_ENTER_PRESSED) {
          TimeEditor_Init(&time_editor);
          screen = UI_TIME_ADJUST;
          screen_start_tick = now;
          display_time_screen(screen, &time_editor);
        }
      } else if (screen == UI_TIME_ADJUST || screen == UI_TIME_CONFIRM) {
        if (event == EVT_MODE_PRESSED) {
          /* Discard the proposed time. */
          TimeEditor_Init(&time_editor);
          timed_proof = 0u;
          screen = UI_TIME_DECISION;
          display_screen(screen, proposed_temp, unit_celsius);
        } else if (event == EVT_ENTER_PRESSED) {
          /* Keep the proposed time for the later run confirmation. */
          TimeEditor_Stop(&time_editor);
          timed_proof = 1u;
          screen = UI_RUN_DECISION;
          display_screen(screen, proposed_temp, unit_celsius);
        } else if (event == EVT_UP_PRESSED || event == EVT_DOWN_PRESSED) {
          TimeEditorDirection_t direction =
              event == EVT_UP_PRESSED ? TIME_EDITOR_UP : TIME_EDITOR_DOWN;

          if (TimeEditor_Press(&time_editor, direction, now)) {
            display_time_screen(screen, &time_editor);
          }
        } else if (event == EVT_UP_RELEASED) {
          TimeEditor_Release(&time_editor, TIME_EDITOR_UP);
        } else if (event == EVT_DOWN_RELEASED) {
          TimeEditor_Release(&time_editor, TIME_EDITOR_DOWN);
        }
      } else if (screen == UI_RUN_DECISION) {
        if (event == EVT_MODE_PRESSED) {
          screen = UI_IDLE_SPLASH;
          display_screen(screen, proposed_temp, unit_celsius);
          screen_start_tick = now;
        }
      }
    }

    uint32_t now = osKernelGetTickCount();

    if (screen == UI_TIME_ADJUST || screen == UI_TIME_CONFIRM) {
      if (TimeEditor_Poll(&time_editor, now)) {
        display_time_screen(screen, &time_editor);
      }
    }

    if (screen != UI_IDLE_SPLASH && screen != UI_IDLE_PROMPT &&
        (uint32_t)(now - last_activity_tick) >= APP_INACTIVITY_TIMEOUT_MS) {
      TimeEditor_Stop(&time_editor);
      screen = UI_IDLE_SPLASH;
      display_screen(screen, proposed_temp, unit_celsius);
      screen_start_tick = now;
    } else if ((screen == UI_IDLE_SPLASH || screen == UI_IDLE_PROMPT ||
                screen == UI_TEMP_ADJUST || screen == UI_TEMP_CONFIRM) &&
               (uint32_t)(now - screen_start_tick) >= APP_TOGGLE_PAIR_MS) {
      if (screen == UI_IDLE_SPLASH)
        screen = UI_IDLE_PROMPT;
      else if (screen == UI_IDLE_PROMPT)
        screen = UI_IDLE_SPLASH;
      else if (screen == UI_TEMP_ADJUST)
        screen = UI_TEMP_CONFIRM;
      else
        screen = UI_TEMP_ADJUST;

      display_screen(screen, proposed_temp, unit_celsius);
      screen_start_tick = now;
    } else if ((screen == UI_TIME_ADJUST || screen == UI_TIME_CONFIRM) &&
               (uint32_t)(now - screen_start_tick) >= APP_TIME_TOGGLE_PAIR_MS) {
      screen = screen == UI_TIME_ADJUST ? UI_TIME_CONFIRM : UI_TIME_ADJUST;
      display_time_screen(screen, &time_editor);
      screen_start_tick = now;
    }
  }
}