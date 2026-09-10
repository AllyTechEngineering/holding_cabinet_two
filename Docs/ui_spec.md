# Holding Cabinet — UI/UX Specification
**Version:** v4
**Status:** Draft
**Last updated:** 2026-09-10

## Changelog
- v2: Consolidated original hand-written draft (ui_spec_v1.docx) with
  prior chat decisions; resolved leading-zero, timer format, and
  power-loss persistence conflicts.
- v3: Removed the On/Off button; Enter absorbs start/confirm/stop.
  Physical control count reduced from 5 to 4.
- v4: Added state-transition table (Section 13), Enter-overload risk
  note (Section 14), queue cross-reference (Section 15). Fixed AM/PM
  correctness gap in Section 6. Surfaced and proposed a fix for a
  Mode-button contradiction found while building Section 13.

---

## 1. Physical Controls

Four inputs total:
- **Up** — increment value on current screen
- **Down** — decrement value on current screen
- **Mode** — cycles between top-level screens; also used to open a
  highlighted Settings sub-item (see Section 13 for a proposed fix
  to a contradiction found in this role)
- **Enter** — confirms/advances the flow: starts the system from
  idle, confirms values in Temp/Time/Settings, triggers the stop
  confirmation during Run mode, and confirms the stop itself

Separately, there is a **main power switch** controlling electrical
power to the board. This is not a GPIO-sensed button — it's real
power. There is no active "shutdown routine" that runs as power is
cut.

## 2. Persistence Across Power Loss

- **Persisted:** temperature setpoint, unit selection (F/C), timer
  duration setting
- **NOT persisted / not resumed:** active heating state, run-mode
  progress, or countdown-in-progress
- On power restore, the system **always** boots to the idle screen
  (Section 3) — it never auto-resumes heating. The user must press
  Enter to start a new run, using the remembered setpoint/units/
  timer as defaults.
- **Storage mechanism: Flash-emulated EEPROM.** One reserved Flash
  sector; written only when a value is confirmed (Enter press), not
  on every Up/Down tap. (RTC backup registers were considered and
  rejected — the Nucleo-64's VBAT pin isn't connected to VDD by
  default, so backup registers won't survive real power loss without
  adding a coin cell, which isn't currently planned.)

## 3. Idle Screen

Default state on power-up and after any stop/complete cycle.

**Sequence:**
1. Line 1: "Proofing Oven" scrolls across and clears (line 2 blank
   during this)
2. Settles to a static two-line display:

       Press Enter
          To Start

Stays here until Enter is pressed.

## 4. Set Temperature Mode (default after Enter)

- Up/Down adjust the setpoint by 1 unit per press (no padding,
  natural width — see Section 10)
- Long-press acceleration applies (see Section 9)
- Range: **65°F to 115°F** (display range; no cooling, so practical
  low end is ambient — user-manual note, not firmware-enforced
  beyond the 65°F floor)
- At the boundaries (65°F or 115°F), Up/Down **clamp** — they do
  not wrap around to the opposite end
- Internally always Celsius; converted to Fahrenheit only at the
  display layer, only when Units=F
- **Enter** confirms the setpoint, writes it to `qDisplayToHeat`
  (see Section 15), and advances to a confirmed state where:
  - **Mode** proceeds to Set Time Mode (Section 5)
  - **Enter** (pressed again) skips the timer entirely and proceeds
    directly to the Start Proofing prompt (Section 7), running
    untimed

Example display:

    Temp: 95F
    Set: 100F  HEAT

(Second line drops `HEAT`, replaced with blank padding, when the
relay is off.)

## 5. Set Time Mode

Reached via **Mode** from the confirmed Set Temperature state
(Section 4), only after temperature is confirmed with Enter.

- Countdown duration for the proofing session; reaching zero enters
  Complete mode
- Up/Down adjust duration; long-press acceleration applies
- Format: **HH:MM**, no seconds
- Maximum duration: **not yet defined.** Several reference
  fermentation processes in Section 11 run past 24 hours (natto,
  koji); whether HH rolls over past 24 or the field caps at 23:59
  is an open question — flag for a future decision, not resolved
  here
- **Enter** confirms the duration and advances to the Start Proofing
  prompt (Section 7)

Example display:

    Time: 02:00
    Temp: 95F  HEAT

## 6. Settings Mode

Reached via **Mode** again from Set Time mode (or directly from the
confirmed Set Temp state if no timer is set).

- **Top level:** Up/Down scroll a sub-item list:
  1. **Units (F/C)**
  2. **AM/PM vs 24HR** — **24HR is the only implemented format.**
     12-hour/AM-PM display is blocked until someone defines how it
     handles durations over 12 hours (a 14-hour proof cannot render
     correctly as a 12-hour clock). Enter returns to the list with
     no effect; internally always 24HR.
- **PROPOSED FIX (contradiction found in Section 13 below):**
  **Entering a sub-item** is done with **Enter**, not Mode. **Mode**
  at the top-level list always exits Settings back to Set
  Temperature mode — no exceptions, no context-dependent behavior.
  This replaces the earlier rule where Mode did both jobs
  ambiguously. **Needs your confirmation before this is final.**
- **Inside a sub-item:** Up/Down change the value; **Enter** confirms
  and returns to the list

Units sub-item display:

    Units:  F
    Up/Dn to change

Up always selects C; Down always selects F (direct selection, not a
toggle).

## 7. Run Mode

After temp (and optionally time) are confirmed:

    Start Proofing
    Press Enter

**Enter** begins active heating (Run mode).

**During Run mode:**
- Display shows current temp, setpoint, heater status, and (if set)
  the countdown
- Bang-bang control with hysteresis drives the heat relay off the
  Celsius setpoint, independent of display units
- **Mode** cycles between Temp and Time screens only — **Settings is
  skipped** while running
- **Enter** triggers a stop confirmation:

      Are You Sure?
      Stop Proofing

- **Enter again** confirms the stop, advancing to Complete mode
- **Any other button** (Up/Down/Mode) cancels back to Run mode
- A **~10 second timeout** with no input also auto-cancels back to
  Run mode

See Section 14 for a known risk with Enter's overloaded role here.

## 8. Complete Mode

Reached by:
- Countdown reaching zero (no confirmation — straight here)
- Manually stopping and confirming via Section 7's flow

    Proofing
    Complete

**Enter** returns to the idle screen (Section 3).

## 9. Long-Press Acceleration

Applies to Up/Down on **numeric fields only** — Set Temp and Set
Time. Does **not** apply to the Units toggle (Section 6), which is a
two-state selection, not a numeric range.

| Phase       | Trigger               | Behavior             |
|-------------|-----------------------|----------------------|
| Single tap  | Released before 500ms | ±1 unit, one time    |
| Slow repeat | Held past 500ms       | ±1 unit every ~400ms |
| Fast repeat | Held past ~2.5s total | ±1 unit every ~150ms |

## 10. Display Formatting Rules

- **Temperature:** no padding, natural width (95F, 115F). A
  one-character shift occurs at the 99↔100 boundary — accepted.
- **Time/countdown:** HH:MM, no seconds.
- `HEAT` indicator is blanked (space-padded) when the relay is off,
  to avoid stale characters.

## 11. Explicitly Out of Scope for the Display

Reference table of fermentation temperature ranges (sourdough,
yeasts, yogurt, kefir, kombucha, tempeh, koji, miso, natto,
fermented vegetables, cultured dairy, cheese, beer, wine) is
background/reference only — not shown on-device. Potentially useful
for a user manual later.

## 12. Resolved Design Decisions

- **Physical controls:** 4 total (Up, Down, Mode, Enter) — On/Off
  eliminated, Enter absorbs start/confirm/stop
- **GPIO impact:** none — Enter reuses the OnOffSwitch pin from the
  original CubeMX config
- **AM/PM vs 24HR:** 24HR only, implemented; 12-hour blocked on a
  correctness gap (Section 6)
- **Complete → idle:** Enter (Section 8)
- **Decline stop confirmation:** any non-Enter button, or 10s timeout
  (Section 7)
- **Persistence mechanism:** Flash-emulated EEPROM, written on
  confirm only (Section 2)
- **Open, unresolved:**
  - Front-panel silkscreen/labeling for the Enter button
  - Maximum Set Time duration and behavior past 24 hours (Section 5)
  - Settings entry mechanism — Mode vs Enter (Section 6, proposed
    fix pending confirmation)

## 13. State Transition Table

| Current State         | Input   | Next State     | Action / Notes 
| IDLE                  | Enter   | SET_TEMP       | Load persisted setpoint/units/timer as defaults 
| SET_TEMP              | Up      | SET_TEMP       | setpoint += 1, clamp at 115°F, acceleration applies 
| SET_TEMP              | Down    | SET_TEMP       | setpoint -= 1, clamp at 65°F, acceleration applies 
| SET_TEMP              | Mode    | SET_TEMP       | Ignored — must confirm with Enter first 
| SET_TEMP              | Enter   | TEMP_CONFIRMED | Setpoint written to qDisplayToHeat 
| TEMP_CONFIRMED        | Mode    | SET_TIME       | Proceed to timer entry 
| TEMP_CONFIRMED        | Enter   | START_PROMPT   | Skip timer, untimed run 
| SET_TIME              | Up      | SET_TIME       | duration += 1 min, acceleration applies, upper bound undefined (see Section 5) 
| SET_TIME              | Down    | SET_TIME       | duration -= 1 min, clamp at 00:00, acceleration applies 
| SET_TIME              | Enter   | START_PROMPT   | Duration written to qDisplayToHeat 
| START_PROMPT          | Enter   | RUN            | Heating begins 
| SETTINGS_LIST         | Up/Down | SETTINGS_LIST  | Scroll highlighted sub-item 
| SETTINGS_LIST         | Enter   | SETTINGS_ITEM  | Opens highlighted sub-item — replaces Mode in this role, pending confirmation 
| SETTINGS_LIST         | Mode    | SET_TEMP       | Always exits Settings — no longer context-dependent under proposed fix 
| SETTINGS_ITEM (Units) | Up      | SETTINGS_ITEM  | Select C 
| SETTINGS_ITEM (Units) | Down    | SETTINGS_ITEM  | Select F 
| SETTINGS_ITEM (Units) | Enter   | SETTINGS_LIST  | Confirm, write to Flash, return to list 
| SETTINGS_ITEM (AM/PM) | any     | SETTINGS_LIST  | No effect (placeholder, Section 6) 
| RUN                   | Mode    | RUN (Temp/Time view toggle) | Display view only, no state change 
| RUN                   | Enter   | STOP_CONFIRM   | — 
| RUN                   | Timer 0 | COMPLETE       | No confirmation step 
| STOP_CONFIRM          | Enter   | COMPLETE       | Stop confirmed 
| STOP_CONFIRM          | U/D/M   | RUN            | Cancelled 
| STOP_CONFIRM          | 10s T   | RUN            | Auto-cancelled 
| COMPLETE              | Enter   | IDLE           | — 

## 14. Known Risk: Enter Button Overload

Enter is the sole "affirmative" control for starting the system,
confirming every numeric/settings entry, triggering the stop
confirmation, and confirming the stop itself. The only safeguard
against an accidental stop is the two-step "Are You Sure?" /
"Stop Proofing" confirmation (Section 7) plus the 10-second
auto-cancel timeout. There is currently no additional protection
(e.g., a longer hold-to-confirm) beyond that double-press pattern.
Acceptable for now; worth revisiting if real-world use shows
accidental stops happening.

## 15. Queue Cross-Reference

Maps each user-facing action to the FreeRTOS queue that carries it,
per the task architecture established separately.

| User Action                    | Occurs In                   | Queue            | Direction                | Payload 
| Up/Down/Mode/Enter press       | Any screen                  | qInputToDisplay  | InputTask → DisplayTask  | button event enum              |
| Setpoint confirmed             | TEMP_CONFIRMED              | qDisplayToHeat   | DisplayTask → HeatTask   | setpoint (int16, tenths °C)    |
| Timer duration confirmed       | START_PROMPT (via SET_TIME) | qDisplayToHeat   | DisplayTask → HeatTask   | duration (uint32, minutes)     |
| Run started                    | START_PROMPT → RUN          | qDisplayToHeat   | DisplayTask → HeatTask   | run/enable command             |
| Stop confirmed                 | STOP_CONFIRM → COMPLETE     | qDisplayToHeat   | DisplayTask → HeatTask   | shutoff command                |
| Temperature reading            | Every 2–3s, any state       | qSenseToHeat     | SenseTask → HeatTask     | temperature (int16, tenths °C) |
| Current temp + relay status    | Continuous                  | qHeatToDisplay   | HeatTask → DisplayTask   | temp + relay on/off            |
| Future ESP32 data (Models 3/4) | ConnectTask                 | qUartRxToConnect | USART2 ISR → ConnectTask | placeholder, protocol TBD      |