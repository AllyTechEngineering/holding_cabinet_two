#include "run_timer.h"
#include "app_config.h"

#define MS_PER_MINUTE 60000u

static uint32_t elapsed_ms(const RunTimer_t *timer, uint32_t now_ms) {
  /* Unsigned subtraction handles a single 32-bit clock rollover. */
  return (uint32_t)(now_ms - timer->start_ms);
}

void RunTimer_Init(RunTimer_t *timer) {
  timer->mode = RUN_TIMER_INACTIVE;
  timer->duration_minutes = 0u;
  timer->start_ms = 0u;
}

void RunTimer_Stop(RunTimer_t *timer) {
  RunTimer_Init(timer);
}

bool RunTimer_StartTimed(RunTimer_t *timer, uint16_t duration_minutes,
                         uint32_t now_ms) {
  if (duration_minutes < APP_TIME_MIN_MINUTES ||
      duration_minutes > APP_TIME_MAX_MINUTES) {
    return false;
  }

  timer->mode = RUN_TIMER_TIMED;
  timer->duration_minutes = duration_minutes;
  timer->start_ms = now_ms;
  return true;
}

void RunTimer_StartUntimed(RunTimer_t *timer, uint32_t now_ms) {
  timer->mode = RUN_TIMER_UNTIMED;
  timer->duration_minutes = 0u;
  timer->start_ms = now_ms;
}

RunTimerMode_t RunTimer_Mode(const RunTimer_t *timer) {
  return timer->mode;
}

uint16_t RunTimer_DurationMinutes(const RunTimer_t *timer) {
  return timer->duration_minutes;
}

bool RunTimer_Expired(const RunTimer_t *timer, uint32_t now_ms) {
  uint32_t duration_ms;

  if (timer->mode != RUN_TIMER_TIMED) {
    return false;
  }

  duration_ms = (uint32_t)timer->duration_minutes * MS_PER_MINUTE;
  return elapsed_ms(timer, now_ms) >= duration_ms;
}

uint16_t RunTimer_RemainingMinutes(const RunTimer_t *timer, uint32_t now_ms) {
  uint32_t duration_ms;
  uint32_t elapsed;
  uint32_t remaining_ms;

  if (timer->mode != RUN_TIMER_TIMED) {
    return 0u;
  }

  duration_ms = (uint32_t)timer->duration_minutes * MS_PER_MINUTE;
  elapsed = elapsed_ms(timer, now_ms);

  if (elapsed >= duration_ms) {
    return 0u;
  }

  remaining_ms = duration_ms - elapsed;
  return (uint16_t)((remaining_ms + MS_PER_MINUTE - 1u) / MS_PER_MINUTE);
}