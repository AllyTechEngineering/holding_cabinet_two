# History of AI chats

# Session Summary — Holding Cabinet Two - Project Start Up

## 1. FreeRTOS Task/Queue Architecture
- Defined 5 tasks: SenseTask, InputTask, DisplayTask, HeatTask, ConnectTask
  — priorities, stack sizes, and responsibilities assigned through
  interview rather than guessed upfront.
- Defined 5 queues (all depth 1, latest-value-wins): qSenseToHeat,
  qInputToDisplay, qDisplayToHeat, qHeatToDisplay, qUartRxToConnect.
- Established the polling-vs-interrupt split: buttons stay
  poll-based (human timescale, no ISR needed); future ESP32 UART
  link uses interrupt/queue hand-off (external, unpredictable timing).
- Stubbed the ISR-to-queue pattern for the future UART RX interrupt,
  even though the ESP32 protocol itself isn't designed yet.
- Diagnosed and fixed a real CubeMX FreeRTOS heap error: adding the
  5th task (InputTask) exceeded `TOTAL_HEAP_SIZE` (3000 bytes) —
  raised to 8192 bytes, resolved.
- Diagnosed a USART2 "yellow warning triangle" in CubeMX (benign —
  unused hardware-flow-control pins conflicting with relay GPIOs).
- Explained why PC14/PC15 (RCC_OSC32_IN/OUT) are locked by default —
  Nucleo-L476RG ships with an onboard 32.768kHz crystal wired there.

## 2. UI/UX Specification (`ui_spec.md`, now split into subsystem files)
- Full front-panel spec developed iteratively: Idle, Set Temp, Set
  Time, Settings, Run, Stop-Confirm, Complete — 7 distinct states,
  reached via 4 physical controls (Up/Down/Mode/Enter).
- **On/Off button eliminated** — Enter absorbs start/confirm/stop,
  reducing physical controls from 5 to 4 with zero new GPIO needed
  (reuses the original OnOffSwitch pin).
- Reviewed the user's own draft (`ui_spec_v1.docx`) and surfaced
  several real contradictions before they hit code: a 5th "Enter"
  button that didn't exist in GPIO config, an AM/PM setting that
  implied a real clock (deferred as placeholder — user may add a
  coin-cell RTC later), a leading-zero padding conflict (resolved:
  no padding, natural width), timer format conflict (resolved:
  HH:MM, no seconds), a 3-line idle screen that didn't fit 2-line
  hardware, and a Mode-button contradiction (opening a Settings
  sub-item vs. exiting Settings) — proposed fix: Enter opens
  sub-items, Mode always exits.
- Settings deliberately kept as a scrollable list (not flattened)
  because the user expects to add more settings over time — a
  documented, reasoned choice, not scope creep.
- Persistence requirement clarified: setpoint/units/timer survive
  power loss via Flash-emulated EEPROM (not RTC backup registers —
  Nucleo-64's VBAT isn't tied to VDD by default, confirmed against
  the official UM1724 manual); system never auto-resumes heating,
  always boots to idle.
- Long-press acceleration defined: tap / 400ms repeat / 150ms fast
  repeat, standard 3-phase pattern.

## 3. Hardware Bring-Up (all via `#define`-gated test blocks in `main.c`)
- **Relays:** found and fixed a real safety bug — CubeMX's default
  GPIO init energized all relays at boot (active-low logic,
  `GPIO_PIN_RESET` = ON). Fixed via `USER CODE` override; durable
  CubeMX-level fix also documented.
- **Switches:** verified all 4 individually via Live Watch; found
  actual polarity was active-low (idle=3.3V, pressed=0V) — opposite
  of the first description given — corrected in test code.
- **LED:** verified independently via isolated blink test.
- **LCD:** I2C scan confirmed PCF8574 backpack at address `0x27`;
  wrote a full HD44780 4-bit driver and printed a real test message;
  backlight control verified independently.
- **Relay pulse test:** each switch mapped to one relay (Up=Heat,
  Down=Fan, Mode=Humid), Enter as a panic all-off override —
  confirmed correct mapping by ear/eye.
- **NTC sensor:** confirmed divider topology from schematic (NTC
  low-side), sourced real part specs (MF52B, R25=10K, B=3950)
  against actual datasheets, implemented the Beta equation. Found a
  large reading error traced to a bad breadboard connection (not a
  math or component fault); after fixing, accepted a small ~5.7°F
  residual offset as normal tolerance (control loop's hysteresis
  absorbs it).
- **USART2 self-loopback:** found and diagnosed a Nucleo-specific
  quirk — PA2/PA3 needed a physical zero-ohm jumper to route to the
  standard TX/RX header instead of the board's default path.
- Found and fixed a genuine C bug in the test scaffolding itself: a
  backslash line-continuation with trailing whitespace (a real,
  documented cross-compiler hazard).

## 4. ESP32-C6 Connectivity Prep
- Confirmed 3.3V logic compatibility (no level shifting needed).
- Corrected an early wiring assumption: ESP-AT firmware uses **two
  separate UARTs** — UART0 (GPIO16/17) for flashing/logs only,
  UART1 (GPIO7=TX, GPIO6=RX) for actual AT commands. Verified
  directly against two independent pages of Espressif's own docs.
- Found a practical unblock for firmware download: the "Recommended"
  release requires a company-email-gated form, but older versions
  (e.g. v4.1.1.0) are direct, ungated downloads on the same page.

## 5. Documentation Restructuring
- Originally one `ARCHITECTURE.md` + one `ui_spec.md`; user correctly
  identified this as "jumbled" for a reviewer to navigate.
- Restructured into a 16-file, per-subsystem documentation set:
  `arch.md` (index + system-level-only content), `tasks_queues.md`,
  `connectivity.md`, `persistence.md`, `ntc_sensing.md`, `buttons.md`,
  `led_display.md`, `relay_control.md`, `audible_alert.md`,
  `states_modes.md`, `settings_mode.md`, `idle_off_mode.md`,
  `run_mode.md`, `complete_mode.md`, `timer.md`, `display.md`.
- Identified and resolved real content overlaps between the old docs
  (persistence rationale, queue cross-reference table) with explicit
  ownership rules so they don't drift apart.
- Settled on a lightweight version-control approach: `changelog.md`
  (lowercase, repo root) with dated, plain-English entries for
  significant changes — chosen over git tagging after UI-navigation
  guidance proved unreliable to give accurately without seeing the
  actual VS Code version in front of you.
- Established the boundary: `changelog.md` records *that* something
  changed (history); `arch.md`'s index describes what exists *now*
  (current map) — not the same job, not merged.

## 6. Process/Engineering Judgment Calls Made Explicit
- Named several build-vs-defer forks explicitly rather than
  silently picking one: Service Mode vs. disposable test scaffolding,
  flat Settings vs. scrollable list, RTC-now vs. RTC-later.
- Self-critiqued the hardware bring-up process against what a
  principal engineer would actually require for a multi-unit/team
  product (component characterization, current-limited bench supply,
  non-debugger-dependent test output) — explicitly scoped down to
  "solo consumer MVP" as the right bar, not the higher one.

# Session Summary — Holding Cabinet Two - FreeRTOS tasks and queues implementation

## Repo verification performed
- Cloned and read actual repo state rather than assuming: `Core/Src/freertos.c` (untouched CubeMX default, no tasks/queues created), `App/Common/app_types.h`/`app_config.h` (empty stubs), all four scaffolded task files (`sensor_task`, `control_task`, `display_task`, `connect_task` — header-comment-only, zero logic)
- Confirmed actual GPIO pin definitions in `main.h`: `UpArrowSwitch` (PC6), `DownArrowSwitch` (PC7), `ModeSwitch` (PC8), `EnterSwitch` (PC9) — **4 buttons, no On/Off** (On/Off was removed in v3 of the spec; Enter absorbed its roles)
- Found and read the working, already-tested NTC bring-up code (`HW_BRINGUP_TEST_NTC` block in `main.c`): ADC→resistance formula, `ntc_resistance_to_celsius()` (Beta equation, Beta=3950, R25=10K), and an existing fault check (ADC ≥ 4090 → open-circuit)
- Confirmed NTC divider topology against a photographed part label: +3.3V → R1 (10K, 1%) → sense node → NTC → GND (low-side divider), Beta=3950±1%
- Found a stale file-header comment in `thermistor_driver.c` saying "Steinhart-Hart conversion" — should say Beta equation, not yet corrected in the file itself

## Documentation state — found, not yet fixed
- `Docs/ui_spec.md` does not exist in the repo (was referenced constantly by `arch.md` but retired during an earlier doc restructure)
- Recovered the full v4 UI spec content from the "Project start up" chat history (state transition table, queue cross-reference, screen mockups, long-press acceleration timing)
- Discovered the planned 18-file doc restructure (per-subsystem files: `buttons.md`, `states_modes.md`, `run_mode.md`, `set_temp_mode.md`, `set_time_mode.md`, `settings_mode.md`, `idle_off_mode.md`, `complete_mode.md`, `timer.md`, `display.md`, `ntc_sensing.md`, `tasks_queues.md`, `connectivity.md`, `led_display.md`, `relay_control.md`, `audible_alert.md`, plus `persistence.md` and `arch.md`) was **agreed to but never executed** — only `persistence.md` (4.6KB) actually has content; the other 16 subsystem files are 0 bytes; `arch.md` is still 9.4KB/211 lines, far bigger than the "thin index only" it was supposed to shrink to
- User created `set_temp_mode.md` and `set_time_mode.md` directly, closing a real gap (no file existed for Set Temp/Set Time/Start Prompt/Stop Confirm content)

## UI/UX decisions made or confirmed this session
- **Up/Down live-adjust during Run** (new this session): act on whichever view is showing — setpoint in Temp view (sent to HeatTask), countdown in Time view (DisplayTask-local only, never touches the Heat queue)
- **Set Time max duration: 10:00 (10 hours)** — prototype-stage cap, expected to change after real use
- **Set Time confirming at 00:00 is rejected** — error prompt required before advancing (requirement only, not coded)
- **Down decrementing the Run countdown to 00:00** — no floor, no special-casing; treated identically to natural timeout. Considered adding protection, explicitly rejected as an unrealistic usage pattern
- **Stop mechanism confirmed sufficient for both timed and untimed runs**: Enter → "Are You Sure?" → Enter, from anywhere in Run, regardless of whether a timer is active

## Verified against external sources
- Brod & Taylor proofer product line (competitive check on max-timer conventions): flagship model uses a 198-hour cap; base model has no built-in limit at all; bundle variants ship with 10hr/24hr caps — no industry consensus number exists

## Mode vs. state clarified
- **Mode** = contains its own internal sub-state-machine with entry/exit paths (Run, Settings)
- **State** = single node, no internal structure (Idle, Set Temp, Set Time, Start Prompt, Complete)
- Stop Confirm is a sub-state nested inside Run, not a peer state
- "Adjusting temp/time during Run" isn't a state or mode at all — it's just Up/Down behavior depending on which view is showing, inside Run
- NTC reading, display updates, and button presses are not part of the Idle→Run→Complete state machine — they're the concurrent FreeRTOS tasks running in parallel to it
- Button debounce logic is correctly identified as its own separate small state machine, internal to InputTask

## Corrections made after being caught
- Wrongly introduced a 5th "On/Off" button in a flow-chart response; corrected against verified GPIO pins (4 buttons only)
- Wrote `ui_spec.md` as a new file despite it having been deliberately retired in favor of the per-subsystem split
- Proposed a narrower scope for `states_modes.md` ("transition table only") that was never actually confirmed by the user, contradicting the user's own broader original definition
- Manufactured non-issues (Down-to-zero countdown risk, sensor-fault handling, undefined-input handling) framed as urgent design gaps; user correctly identified these as unrealistic/premature and had them dropped
- Asked the user to re-verify NTC bring-up test results that were already documented as verified in the repo's own history

## Not yet done
- No code written: `app_types.h`, `app_config.h`, `freertos.c` task/queue creation, and all task bodies remain unstarted
- No documents actually written to the repo (this session produced zero committed files)
- 16 of 18 planned subsystem doc files remain empty
- Open items still unresolved: Settings Mode/Enter contradiction, setpoint-confirm visual feedback gap, SET_TIME zero-duration error prompt wording
