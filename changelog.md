# Changelog

## 2026-09-08

- FreeRTOS task/queue architecture finalized: 5 tasks, 5 queues.
- UI spec reworked to v4: removed On/Off button, Enter absorbs
  start/confirm/stop.

## 2026-09-14

- Hardware bring-up complete: relays, switches, LED, LCD, NTC all
  verified on the bench. Found and fixed a boot-safety bug where
  relays were energizing on power-up.
- Restructured documentation from one ARCHITECTURE.md into an
  index (arch.md) plus per-subsystem files.

## 2026-09-17

- UI navigation redesigned from the ground up, based on the V1 tab of
  `Proofing_Oven.xlsx`: Mode now drives forward through a chain of
  Enter=Yes/Mode=No decision screens, replacing the old
  Enter-absorbs-everything model. V1 supersedes the v4 spec wherever
  they conflict.
- Doc restructuring finally executed: `states_modes.md` (mechanics
  only) plus six per-mode content files (`idle_off_mode.md`,
  `set_temp_mode.md`, `set_time_mode.md`, `run_mode.md`,
  `complete_mode.md`, `settings_mode.md`) are now written and locked,
  closing a gap that had sat empty since the original restructure.
- Temp range set: 65–120°F. No active cooling — a setpoint below
  ambient is valid input but will never be reached; flagged for
  `HeatTask` control-loop design.
- Settings will be built as generic scrollable-menu infrastructure
  from the start, even though only one item (Temp units F/C) exists
  today.

## 2026-09-22

- Added SetTime-Adjust and SetTime-Confirm to DisplayTask. The screens
  alternate every 2 seconds; Enter retains the proposed duration and
  advances to Run-Decision, while Mode discards it.
- Added `time_editor.c/h` to manage the proposed duration, bounds, and
  Up/Down button holds independently of LCD and RTOS code. Added the
  module to the CMake build.
- Set the initial duration to 1:00 and the range to 0:15–10:00. Raised
  the maximum from 4:00 to 10:00 after observing that sourdough may
  need more than four hours.
- Added one-minute taps and a hold ramp: after 0.6 seconds, one minute
  every 200 ms; after 2 seconds, five minutes every 400 ms; after
  4 seconds, five minutes every 200 ms. Release stops repetition.
- Updated `set_time_mode.md` and `folders_files.md` to describe the
  timer behavior and module. The user tested the time adjustment UI
  on the board and confirmed it works.

## 2026-09-29

- Standardized application temperature handling on whole-degree Celsius
  internally while retaining Fahrenheit or Celsius as the user-selectable
  display unit.
- Defined temperature conversion rounding and separated the selectable
  setpoint range from the measured-temperature display range.
- Added the DisplayTask-to-HeatTask command interface for confirmed
  setpoints, run requests, and stop requests.
- Added on/off heater control with an initial 2°C hysteresis. The confirmed
  setpoint is the upper control limit and the lower control limit is the
  setpoint minus the configured hysteresis.
- Added heater shutdown behavior for run termination, timed-proof expiry,
  and detected NTC faults.
- Synchronized the Heater indicator LED with the firmware heater command.
- Added fail-safe heater and Heater indicator shutdown if HeatTask cannot
  publish its status.
- Added functional test procedures FT-007 through FT-011 covering heater
  operation below the lower control limit, operation at or above the
  setpoint, stopping an active run, timed-proof expiry, and NTC faults.
- Bench-tested the normal heater-control use cases on the powered mock-up
  with the heating element disconnected. Heater relay and Heater indicator
  behavior operated as expected.
- Observed that the disconnected NTC input may be susceptible to electrical
  noise during the open-circuit test. Hardware resolution is deferred to
  the separate hardware project.