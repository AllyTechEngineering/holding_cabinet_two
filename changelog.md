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