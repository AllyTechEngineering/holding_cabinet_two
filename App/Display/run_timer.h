#ifndef RUN_TIMER_H
#define RUN_TIMER_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  RUN_TIMER_INACTIVE,
  RUN_TIMER_UNTIMED,
  RUN_TIMER_TIMED
} RunTimerMode_t;

typedef struct {
  RunTimerMode_t mode;
  uint16_t duration_minutes;
  uint32_t start_ms;
} RunTimer_t;

void RunTimer_Init(RunTimer_t *timer);
void RunTimer_Stop(RunTimer_t *timer);

/* Returns false and leaves the timer unchanged if duration is out of range.
   Also restarts an existing timed run from now_ms when called again. */
bool RunTimer_StartTimed(RunTimer_t *timer, uint16_t duration_minutes,
                         uint32_t now_ms);

void RunTimer_StartUntimed(RunTimer_t *timer, uint32_t now_ms);

RunTimerMode_t RunTimer_Mode(const RunTimer_t *timer);
uint16_t RunTimer_DurationMinutes(const RunTimer_t *timer);

/* Returns zero for an inactive or untimed run. For a timed run, rounds
   remaining time up to the next whole minute for the HH:MM display. */
uint16_t RunTimer_RemainingMinutes(const RunTimer_t *timer, uint32_t now_ms);

/* Becomes true when a timed run reaches its duration. */
bool RunTimer_Expired(const RunTimer_t *timer, uint32_t now_ms);

#endif /* RUN_TIMER_H */