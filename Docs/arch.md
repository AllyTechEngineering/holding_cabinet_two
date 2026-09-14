# Architecture — Holding Cabinet (Proofer)

System-level view of the firmware and hardware architecture: how the
system is built, not what it does or why specific decisions
were made. For user-facing behavior and the front-panel state machine,
see `ui_spec.md`.

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
USART.

---

## 2. Peripheral / Bus Summary

See holding_cabinet_two.txt
See holding_cabinet_two.pdf for more details.
Note: these files are automatically updated when the Generate Code and
Generate Reports are used in STM32Cube MX

---

## 3. Interfaces

See holding_cabinet_two.txt
See holding_cabinet_two.pdf for more details.
Note: these files are automatically updated when the Generate Code and
Generate Reports are used in STM32Cube MX

### 3.1 I2C1 Bus (Shared)

LCD1602 via PCF8574 I2C backpack, confirmed present at address 0x27
during bring-up (see Section 8).

### 3.2 STM32 ↔ Wi-Fi/BLE Module (UART)

Module selected (ESP32-C6-MINI-1-N4 on a breakout board for
production; WROOM-1 breakout for prototyping). USART2 (PA2/PA3) is the
physical link. See Section 8 for two hardware-specific findings that
affect this interface.

### 3.3 BLE ↔ Companion App

TODO: future.

---

## 4. Setup Menu Entry

Superseded — this is now fully specified in `ui_spec.md`, Section 6
(Settings Mode). Not duplicated here; see that document.

---

## 5. Data Flow (Control Loop, High-Level)

Two producer tasks feed a hub of two state-owning tasks:

- `SenseTask` reads the NTC thermistor every 2–3 s and pushes a
  Celsius reading to `HeatTask`.
- `InputTask` debounces the four physical switches (Up, Down, Mode,
  Enter) and pushes button events to `DisplayTask`.
- `DisplayTask` owns setpoint, mode, and timer state; it consumes
  button events, renders the LCD, and forwards confirmed
  setpoint/timer/run commands to `HeatTask`.
- `HeatTask` consumes both the temperature stream and DisplayTask's
  commands, runs bang-bang control with hysteresis against the
  Celsius setpoint, drives the heat relay, and reports current
  temperature + relay status back to `DisplayTask`.
- `ConnectTask` is not yet wired into this flow — reserved for
  Models 3/4, pending the ESP32 AT-command protocol design.

Full user-action-to-queue mapping (which button press produces which
queue message) lives in `ui_spec.md`, Section 15 — not duplicated
here to avoid two sources of truth drifting apart.

---

## 6. FreeRTOS Task Architecture

**Tasks:**

| Task | Priority | Stack (words) | Entry | Responsibility |
|---|---|---|---|---|
| SenseTask | osPriorityNormal | 128 | StartSenseTask | Read ADC1 every 2–3s, Steinhart/Beta convert, push temp |
| InputTask | osPriorityAboveNormal | 128 | StartInputTask | Poll/debounce 4 switches, push button events |
| DisplayTask | osPriorityLow | 128 | StartDisplayTask | Own setpoint/mode/timer state, render LCD, forward commands |
| HeatTask | osPriorityHigh3 | 128 | StartHeatTask | Bang-bang + hysteresis control, drive relay, report status |
| ConnectTask | osPriorityHigh | 128 | StartConnectTask | USART2 link to ESP32 — Models 3/4 only, not yet implemented |

**Queues** (all depth 1 — latest value wins, no backlog needed):

| Queue | Producer → Consumer | Payload (placeholder type) |
|---|---|---|
| qSenseToHeat | SenseTask → HeatTask | temperature (int16, tenths °C) |
| qInputToDisplay | InputTask → DisplayTask | button event enum |
| qDisplayToHeat | DisplayTask → HeatTask | setpoint / timer / run / stop commands |
| qHeatToDisplay | HeatTask → DisplayTask | temp + relay on/off status |
| qUartRxToConnect | USART2 RX ISR → ConnectTask | placeholder, protocol TBD |

**Heap sizing lesson (worth keeping — bit us once already):** CubeMX
validates dynamic-allocation task stacks against `TOTAL_HEAP_SIZE` and
sets an internal `FootprintOK` flag; a hard failure here shows as a
red circle-X on the FREERTOS tree node, not just a warning. Adding
the 5th task (InputTask) pushed usage over the previous 3000-byte
budget. Fixed by raising `TOTAL_HEAP_SIZE` to 8192 bytes — trivial
against the L476RG's 128KB SRAM, with headroom for the still-unwired
ConnectTask/UART path. If this reappears after adding tasks/queues on
the production MCU (much less SRAM on the C031), recheck this budget
first.

---

## 7. Build Configuration Strategy

**Not yet decided.** This was explicitly deferred — offered as a
fork early in FreeRTOS setup (single shared codebase with
compile-time Model 1–4 variants vs. separate build targets), and
task/queue architecture was prioritized instead. Revisit before
Models 3/4 connectivity work begins, since USART2/ConnectTask
scope depends on which model is being built.

---

## 8. Hardware Bring-Up Findings

Findings from initial bring-up that would bite a second unit, a board
revision, or future-me if left only in chat history.

**Relay board is active-low; CubeMX's default boot state was wrong.**
The relay PCBA is opto-isolated with PNP-labeled parts (schematic
shows S8050 NPN drivers downstream of the optocoupler — doesn't
change the IN-pin logic sense). `IN = LOW` energizes the relay.
CubeMX's auto-generated `MX_GPIO_Init()` defaulted these pins to
`GPIO_PIN_RESET` (LOW) — meaning all three relays energized at boot,
before firmware existed to control them. Fixed via a corrective
`HAL_GPIO_WritePin(..., GPIO_PIN_SET)` in `USER CODE BEGIN
MX_GPIO_Init_2`. Durable fix (not yet applied): set each relay pin's
individual "GPIO output level" to High directly in CubeMX's Pinout
view, so the generated code is correct at the source instead of
patched after the fact.

**Switches are active-low, not active-high.** Physical circuit is an
external pull-up with a normally-open switch to GND — idle = 3.3V,
pressed = 0V. Firmware compares against `GPIO_PIN_RESET` for
"pressed," not `GPIO_PIN_SET`.

**NTC divider topology and part specs (confirmed from schematic +
bring-up test):** NTC on the low side of the divider (10K R1 from
3.3V to sense node, NTC from sense node to GND) — higher ADC reading
means colder. Part: MF52B, R25 = 10,000 Ω, B(25/50) = 3950K, using the
Beta equation (not full Steinhart-Hart, since only B25/50 is known).
An early ~40°F reading error traced to a solderless breadboard
connection issue, not a component or math fault — resolved by
correcting the physical connection. Residual ~5.7°F offset after the
fix accepted as within normal component-tolerance stack-up; bang-bang
control with hysteresis absorbs it. No firmware calibration offset
applied.

**Nucleo-64 PA2/PA3 routing quirk.** USART2's default signal routing
on this dev board goes to a different onboard connector than the
standard TX/RX header pins. A zero-ohm jumper is required to route
PA2/PA3 to the header pins actually being used for the ESP32 link.
Caught via a UART self-loopback test (PA2 jumpered to PA3) that
returned consistent `HAL_TIMEOUT` with `ErrorCode = 0` — i.e.,
genuinely nothing arriving, not a noisy/corrupted signal — which
pointed at the physical path rather than firmware.

**ESP32-C6 AT firmware uses two separate UART ports.** `UART0`
(GPIO16=TX, GPIO17=RX) is flashing + boot/log output only. The actual
AT command/response interface is `UART1` (GPIO7=TX, GPIO6=RX,
GPIO5=CTS, GPIO4=RTS — CTS/RTS unused without hardware flow control).
Confirmed from two independent pages of Espressif's official
documentation. STM32 wiring for the real link: `PA2 → ESP32 GPIO6`,
`PA3 ← ESP32 GPIO7`, shared GND.

**No RTC backup-register persistence without added hardware.** The
Nucleo-64's VBAT pin is not connected to VDD by default (confirmed
against the official UM1724 user manual) — RTC backup registers will
not survive real power loss without physically adding a coin cell.
Setpoint/units/timer persistence uses Flash-emulated EEPROM instead,
written only on confirm (Enter press), not on every Up/Down tap.

---

## 9. Revision Notes

- v1: Initial CubeMX-generated skeleton, most sections placeholder.
- v2: System Overview, Peripheral/Interface pointers to CubeMX
  reports filled in.
- v3 (this revision): Sections 4–8 completed from decisions made
  during FreeRTOS setup and hardware bring-up. Section 4 marked
  superseded by `ui_spec.md`. Section 7 explicitly documented as an
  open decision, not invented. Section 8 added new.