/**
  ******************************************************************************
  * @file           : app_config.h
  * @brief          : Application-wide configuration constants and build-time
  *                    settings
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  ******************************************************************************
  */

#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Temperature setpoint bounds (Docs/set_temp_mode.md) ----
 * Canonical storage is Celsius throughout the firmware (tenths of a
 * degree); Fahrenheit is a display-layer-only conversion. 65-120F. */
#define APP_TEMP_MIN_TENTHS_C   183u   /* 65F */
#define APP_TEMP_MAX_TENTHS_C   489u   /* 120F */
#define APP_TEMP_DEFAULT_TENTHS_C 350u  /* 35°C = 95°F */

/* ---- Countdown timer (Docs/set_time_mode.md) ---- */
#define APP_TIME_MIN_MINUTES             15u     /* 0:15 */
#define APP_TIME_MAX_MINUTES             600u    /* 10:00 */
#define APP_TIME_DEFAULT_MINUTES         60u     /* 1:00 */

/* Up/Down: one minute on press, then accelerate while held. */
#define APP_TIME_TAP_STEP_MINUTES        1u
#define APP_TIME_HOLD_START_MS           600u
#define APP_TIME_RAMP_STAGE2_MS          2000u
#define APP_TIME_RAMP_STAGE3_MS          4000u
#define APP_TIME_REPEAT_1MIN_MS          200u
#define APP_TIME_REPEAT_5MIN_SLOW_MS     400u
#define APP_TIME_REPEAT_5MIN_FAST_MS     200u
#define APP_TIME_REPEAT_LARGE_STEP_MINUTES 5u

/* ---- UI timing (Docs/states_modes.md) ---- */
#define APP_TOGGLE_PAIR_MS         4000u     /* Existing screen pairs */
#define APP_TIME_TOGGLE_PAIR_MS    2000u     /* Time Adjust/Confirm pair */
#define APP_INACTIVITY_TIMEOUT_MS  180000u   /* 3 minutes -> Idle-Splash */
#define APP_SETTINGS_CHORD_MS      5000u     /* Up+Down held, idle-only */
#define APP_DISPLAY_TASK_WAKE_MS   100u      /* DisplayTask periodic wake */

/* ---- InputTask debounce ----
 * Standard mechanical-switch debounce: poll every 20ms, require 3
 * consecutive matching reads (60ms) before accepting a state change.
 * Not a hardware-derived value like the heater-fault timing below --
 * ordinary firmware practice, safe to leave as a default. */
#define APP_INPUT_POLL_MS       20u
#define APP_INPUT_DEBOUNCE_COUNT 3u

/* ---- SenseTask cadence (Docs/arch.md Section 5: "every 2-3s") ---- */
#define APP_SENSE_TASK_PERIOD_MS  2500u

/* ---- Heater-fault detection (Docs/error_mode.md, Docs/error_codes.md) ----
 * PLACEHOLDER VALUES -- not yet validated against real hardware. Proposed
 * as a conservative starting point (see chat log); replace once bench
 * data from the real heating element/cabinet thermal mass is available.
 * Anchored to the moment HeatTask's relay command last changed state,
 * not a rolling window. */
#define APP_HEATER_FAULT_WINDOW_MS      600000u  /* 10 minutes */
#define APP_HEATER_ON_MIN_RISE_TENTHS   10u       /* >=1.0C rise expected after 10min commanded ON, else Heater Open (20) */
#define APP_HEATER_OFF_MAX_RISE_TENTHS  5u        /* >0.5C rise not expected after 10min commanded OFF, else Heater Short (21) */

/* ---- Bang-bang hysteresis band ----
 * NOT YET DECIDED -- no value has been specified anywhere in the design
 * docs. Needed before HeatTask's control loop can be written. */
/* #define APP_HYSTERESIS_TENTHS_C   TBD */

#ifdef __cplusplus
}
#endif

#endif /* __APP_CONFIG_H */