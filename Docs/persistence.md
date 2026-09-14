# Architecture — Holding Cabinet (Proofer)

Index of all firmware/hardware documentation for this project, plus
system-level content that doesn't belong to any single subsystem file.
If you're looking for how a specific piece works, start with the
Documentation Index (Section 3) — this file itself is deliberately
thin.

---

## 1. System Overview

Single-MCU embedded system on a shared PCBA and firmware codebase,
configured per product model at build time.

**Development platform:** NUCLEO-L476RG (STM32L476RG)
**Production target:** STM32C031C6 — not yet validated against this
architecture; task/queue design is MCU-agnostic in concept, but stack
sizes, heap sizing, and peripheral instances will need re-verification
on the smaller part before production.
**RTOS:** FreeRTOS / CMSIS-RTOS v2
**Companion Wi-Fi/BLE module (Models 3/4):** ESP32-C6-MINI-1-N4 for
production; prototype bench work uses an ESP32-C6-WROOM-1 breakout
board for convenience. Same C6 silicon, different module package —
confirmed intentional, not a drift between docs.

The STM32 is the real-time control brain for all models. The companion
module owns the radio stack entirely, communicating to the STM32 over
USART. Full detail: `connectivity.md`.

---

## 2. Peripheral / Bus Summary

See `../holding_cabinet_two.txt`
See `../holding_cabinet_two.pdf` for more details.
Note: these files are automatically updated when the Generate Code and
Generate Reports are used in STM32Cube MX. This is the full,
comprehensive auto-generated peripheral/clock report — for a specific
subsystem's pin assignment and electrical details, see the relevant
file in Section 3 instead.

---

## 3. Documentation Index

| File               | Covers                                                                        |
|--------------------|-------------------------------------------------------------------------------|
| `tasks_queues.md`  | FreeRTOS task list, priorities, stacks, queue definitions, heap-sizing lesson |
| `connectivity.md`  | ESP32 AT firmware, UART0/UART1 split, USART2 wiring, Wi-Fi/BLE scope          |
| `persistence.md`   | Flash-emulated EEPROM, VBAT/coin-cell finding, what survives power loss       |
| `ntc_sensing.md`   | NTC divider topology, part specs, Beta equation, electrical                   |
| `buttons.md`       | Switch circuit, active-low polarity, debounce, electrical                     |
| `led_display.md`   | LED names, purpose, drive circuit, electrical                                 |
| `relay_control.md` | Relay board circuit, active-low logic, boot-safety fix, electrical            |
| `audible_alert.md` | Buzzer purpose and electrical                        |
| `states_modes.md`  | State-transition table — mechanics only, not behavior                         |
| `settings_mode.md` | Settings screen behavior and content                                          |
| `idle_off_mode.md` | Idle/off screen behavior and content                                          |
| `run_mode.md`      | Run mode behavior and content                                                 |
| `complete_mode.md` | Complete mode behavior and content                                            |
| `timer.md`         | Countdown mechanics — format, step size, acceleration                         |
| `display.md`       | General LCD rules — padding convention, backlight, I2C address, init sequence |

---

## 4. Build Configuration Strategy

**Not yet decided.** This was explicitly deferred — offered as a
fork early in FreeRTOS setup (single shared codebase with
compile-time Model 1–4 variants vs. separate build targets), and
task/queue architecture was prioritized instead. Revisit before
Models 3/4 connectivity work begins, since USART2/ConnectTask
scope depends on which model is being built. No subsystem file owns
this — it's cross-cutting.

---

## 5. Revision Notes

- v1: Initial CubeMX-generated skeleton, most sections placeholder.
- v2: System Overview, Peripheral/Interface pointers to CubeMX
  reports filled in.
- v3: Sections 4–8 completed from decisions made during FreeRTOS
  setup and hardware bring-up (superseded by v4 split below).
- v4 (this revision): Restructured from a single monolithic document
  into a documentation index. All subsystem-specific content
  (FreeRTOS architecture, connectivity, persistence, and every
  hardware bring-up finding) moved out into dedicated files per
  Section 3. This file now holds only system overview, the index
  itself, and the one genuinely cross-cutting open item (build
  configuration strategy).