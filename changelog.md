# Changelog

## 2026-09-14
- Hardware bring-up complete: relays, switches, LED, LCD, NTC all
  verified on the bench. Found and fixed a boot-safety bug where
  relays were energizing on power-up.
- Restructured documentation from one ARCHITECTURE.md into an
  index (arch.md) plus per-subsystem files.

## 2026-09-08
- FreeRTOS task/queue architecture finalized: 5 tasks, 5 queues.
- UI spec reworked to v4: removed On/Off button, Enter absorbs
  start/confirm/stop.