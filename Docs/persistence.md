# Persistence

Content owner for what survives power loss and how it's stored.
Electrical/hardware findings are cited from bring-up testing and the
STM32L476xx datasheet — not re-derived here.

## What persists

- Temp setpoint
- Temp units (F/C)
- Timer duration (last configured HH:MM)

For a new proof, the selected setpoint and timer duration are saved
when the user presses Enter at `Run-Decision`. During an active-run
edit, Up/Down changes proposed values only. They are applied and saved
only when the user presses Enter at `Run-Decision`. Mode at
`Run-Decision` stops the run and discards those proposed values.
Temperature units are saved when Enter confirms the Settings choice.
No value is written to Flash on every Up/Down press.

## Mechanism

Flash-emulated EEPROM — the STM32L476RG has no dedicated EEPROM
peripheral. Confirmed from the datasheet: 1MB flash, 2 banks × 256
pages × 2KB/page (Bank 1: `0x08000000`–`0x0807FFFF`, Bank 2:
`0x08080000`–`0x080FFFFF`); page erase ≈22ms, page program ≈21-23ms.

**Storage location:** the last 2 pages of Bank 2 —
`0x080FF000`–`0x080FFFFF` — farthest from where the linker places
growing application code.

**Scheme:** hand-rolled 2-page ping-pong, not ST's EEPROM-emulation
middleware. Only 3 small values persist (setpoint, units, timer
duration), which doesn't justify a library built for dozens of
variables. Mechanics: write the new record to whichever of the two
pages is currently marked invalid, mark it valid, erase the old page.
Gives basic wear-leveling and power-loss safety (a reset mid-write
leaves the old page still valid) without extra machinery.

## What does NOT persist — VBAT finding

The Nucleo-64 board's VBAT pin is not connected to VDD by default
(confirmed against the official UM1724 user manual) — this is a
board-wiring fact, not an MCU limitation; the STM32L476xx itself does
support VBAT-backed RTC/backup registers (300nA VBAT mode, per the
datasheet). Without a coin cell physically added to this board, RTC
backup-register persistence is not viable as currently built.

## Boot behavior

The system never auto-resumes heating on power-up — it always boots
to `Idle-Splash`, regardless of what was happening when power was
lost. A proof in progress at the moment of power loss is **not**
automatically resumed; only the configuration values above (setpoint,
units, timer duration) survive, not run/elapsed state. Deliberate
safety choice: an unattended appliance should never silently resume
heating after an unknown gap in power.