/********************************************************************************
  * @file           : time_editor.c
  * @brief          : For countdown timer adjustment, track the current proposal and the button-hold state.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  *******************************************************************************/

#include "time_editor.h"
#include "app_config.h"

static uint16_t change_minutes(uint16_t minutes, int8_t direction,
                               uint16_t step) {
  if (direction > 0) {
    if (minutes >= APP_TIME_MAX_MINUTES ||
        step >= APP_TIME_MAX_MINUTES - minutes) {
      return APP_TIME_MAX_MINUTES;
    }
    return (uint16_t)(minutes + step);
  }

  if (minutes <= APP_TIME_MIN_MINUTES ||
      step >= minutes - APP_TIME_MIN_MINUTES) {
    return APP_TIME_MIN_MINUTES;
  }
  return (uint16_t)(minutes - step);
}

void TimeEditor_Init(TimeEditor_t *editor) {
  editor->minutes = APP_TIME_DEFAULT_MINUTES;
  editor->held_direction = 0;
  editor->hold_start_ms = 0u;
  editor->last_repeat_ms = 0u;
}

bool TimeEditor_Press(TimeEditor_t *editor, TimeEditorDirection_t direction,
                      uint32_t now_ms) {
  uint16_t next_minutes;

  editor->held_direction = (int8_t)direction;
  editor->hold_start_ms = now_ms;
  editor->last_repeat_ms = now_ms;

  next_minutes = change_minutes(editor->minutes, editor->held_direction,
                                APP_TIME_TAP_STEP_MINUTES);
  if (next_minutes == editor->minutes) {
    return false;
  }

  editor->minutes = next_minutes;
  return true;
}

void TimeEditor_Release(TimeEditor_t *editor,
                        TimeEditorDirection_t direction) {
  if (editor->held_direction == (int8_t)direction) {
    editor->held_direction = 0;
  }
}

void TimeEditor_Stop(TimeEditor_t *editor) {
  editor->held_direction = 0;
}

bool TimeEditor_Poll(TimeEditor_t *editor, uint32_t now_ms) {
  uint32_t held_ms;
  uint32_t repeat_ms;
  uint16_t step;
  uint16_t next_minutes;

  if (editor->held_direction == 0) {
    return false;
  }

  held_ms = (uint32_t)(now_ms - editor->hold_start_ms);
  if (held_ms < APP_TIME_HOLD_START_MS) {
    return false;
  }

  if (held_ms < APP_TIME_RAMP_STAGE2_MS) {
    repeat_ms = APP_TIME_REPEAT_1MIN_MS;
    step = APP_TIME_TAP_STEP_MINUTES;
  } else if (held_ms < APP_TIME_RAMP_STAGE3_MS) {
    repeat_ms = APP_TIME_REPEAT_5MIN_SLOW_MS;
    step = APP_TIME_REPEAT_LARGE_STEP_MINUTES;
  } else {
    repeat_ms = APP_TIME_REPEAT_5MIN_FAST_MS;
    step = APP_TIME_REPEAT_LARGE_STEP_MINUTES;
  }

  if ((uint32_t)(now_ms - editor->last_repeat_ms) < repeat_ms) {
    return false;
  }

  editor->last_repeat_ms = now_ms;
  next_minutes =
      change_minutes(editor->minutes, editor->held_direction, step);

  if (next_minutes == editor->minutes) {
    return false;
  }

  editor->minutes = next_minutes;
  return true;
}

uint16_t TimeEditor_Minutes(const TimeEditor_t *editor) {
  return editor->minutes;
}