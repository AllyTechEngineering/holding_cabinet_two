# Persistence

Content owner for what survives power loss and how it's stored.
Electrical/hardware findings are cited from bring-up testing and the
STM32L476xx datasheet — not re-derived here.

## What persists

- Temp setpoint
- Temp units (F/C)
- Timer duration (last configured HH:MM)

Written only on confirm (Enter press at `SetTemp-Confirm`,
`SetTime-Confirm`, or `Settings-Confirm` — see the respective mode
files) — never on every Up/Down tap. Deliberate Flash-wear decision:
values change on every button tap during adjustment but only need to
survive power loss once confirmed.

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