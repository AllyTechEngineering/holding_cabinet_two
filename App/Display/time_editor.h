/********************************************************************************
  * @file           : time_editor.h
  * @brief          : Header for time_editor.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  *******************************************************************************/

#ifndef TIME_EDITOR_H
#define TIME_EDITOR_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  TIME_EDITOR_DOWN = -1,
  TIME_EDITOR_UP = 1
} TimeEditorDirection_t;

typedef struct {
  uint16_t minutes;
  int8_t held_direction;
  uint32_t hold_start_ms;
  uint32_t last_repeat_ms;
} TimeEditor_t;

/* Start a new time proposal at the configured default. */
void TimeEditor_Init(TimeEditor_t *editor);

/* Apply the initial one-minute change and start tracking a hold. */
bool TimeEditor_Press(TimeEditor_t *editor, TimeEditorDirection_t direction,
                      uint32_t now_ms);

/* Stop repeating when the matching button is released. */
void TimeEditor_Release(TimeEditor_t *editor,
                        TimeEditorDirection_t direction);

/* Stop repeating when the user leaves the time adjustment screens. */
void TimeEditor_Stop(TimeEditor_t *editor);

/* Call periodically while a time adjustment screen is active.
   Returns true only when the displayed value changes. */
bool TimeEditor_Poll(TimeEditor_t *editor, uint32_t now_ms);

uint16_t TimeEditor_Minutes(const TimeEditor_t *editor);

#endif /* TIME_EDITOR_H */