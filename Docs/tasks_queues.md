# Tasks & Queues — FreeRTOS Architecture

Content owner for the FreeRTOS task and queue design. Moved out of
`arch.md` §6 to give it a focused home; the review against the locked
state machine (`states_modes.md` + the six mode-content files) is
resolved below. `arch.md` §6 now just points here.

---

## Tasks

| Task        | Priority              | Stack (words) | Entry            | Responsibility                                              |
|-------------|------------------------|---------------|------------------|----------------------------------------------------------------|
| SenseTask   | osPriorityNormal      | 128           | StartSenseTask   | Read ADC1 every 2–3s, Steinhart/Beta convert, push temp     |
| InputTask   | osPriorityAboveNormal | 128           | StartInputTask   | Poll/debounce 4 switches, detect the Settings-entry chord, push button events |
| DisplayTask | osPriorityLow         | 128           | StartDisplayTask | Own setpoint/mode/timer state, render LCD, forward commands, persist settings to flash |
| HeatTask    | osPriorityHigh3       | 128           | StartHeatTask    | Bang-bang + hysteresis control, drive relay, report status  |
| ConnectTask | osPriorityHigh        | 128           | StartConnectTask | USART2 link to ESP32 — Models 3/4 only, not yet implemented |

**DisplayTask must run on a periodic wake, not a pure blocking wait** —
`xQueueReceive(qInputToDisplay, &evt, pdMS_TO_TICKS(100))`, not
`portMAX_DELAY`. It needs to act with no external trigger: 2s toggle
pairs, the 3-minute inactivity timeout, the ~1s `Run-Active` refresh,
and the background countdown reaching 0:00.

## Queues

| Queue | Producer → Consumer | Payload (placeholder type) | Depth / semantics |
|---|---|---|---|
| qSenseToHeat | SenseTask → HeatTask | temperature (uint16_t, tenths °C; or a `THERMISTOR_FAULT_*` sentinel) | 1, drain-then-put (latest value wins — continuous state) |
| qInputToDisplay | InputTask → DisplayTask | button event enum (incl. `EVT_ENTER_SETTINGS`) | 8, normal FIFO (`xQueueSendToBack` / `xQueueReceive`) — discrete edge events must not be dropped |
| qDisplayToHeat | DisplayTask → HeatTask | `HeatCommand_t` (setpoint / run / stop) | 4, FIFO; process commands in order |
| qHeatToDisplay | HeatTask → DisplayTask | `HeatStatus_t` (temp, relay on/off, error code) | 1, drain-then-put (latest value wins) |
| qUartRxToConnect | USART2 RX ISR → ConnectTask | placeholder, protocol TBD | 1, drain-then-put from ISR (latest value wins) — not yet implemented |

**Heap sizing lesson (carried over):** CubeMX validates dynamic-allocation
task stacks against `TOTAL_HEAP_SIZE` and sets an internal `FootprintOK`
flag — a hard failure shows as a red circle-X on the FREERTOS tree node,
not just a warning. Adding the 5th task (InputTask) pushed usage over
the previous 3000-byte budget; fixed by raising `TOTAL_HEAP_SIZE` to
8192 bytes. Recheck this budget first if it reappears on the
lower-SRAM production MCU (C031) — the deeper `qInputToDisplay` (depth
8 vs. 1) adds a small amount of additional queue storage too.

---

## Resolved design decisions (from review against the locked V1 state machine)

1. **`qInputToDisplay` is depth 8, normal FIFO, not `xQueueOverwrite`.**
   Overwrite semantics are correct for a continuously-updated value
   (temperature) but silently destroy an unprocessed discrete event
   (a button press) if two arrive before DisplayTask services the
   queue — a real, user-visible "I pressed Enter and nothing happened"
   bug. Depth 8 costs a few bytes of RAM and removes the failure mode
   entirely.

2. **The Settings-entry chord (Up+Down held 5s, idle-only) is detected
   inside InputTask**, which already polls both pins on a debounce
   cadence and can time the hold directly. On a continuous 5s hold it
   emits one `EVT_ENTER_SETTINGS` event (one-shot — latched until
   release, so it doesn't refire every poll while held). InputTask
   stays blind to app state; **DisplayTask decides whether to honor
   the event** based on whether it's currently on `Idle-Splash` /
   `Idle-Prompt` — keeping all state ownership in one place.

3. **DisplayTask wakes on a bounded timeout (~100ms), not
   `portMAX_DELAY`.** This drives the 2s toggle pairs, the 3-minute
   inactivity timeout, the ~1s `Run-Active` refresh, and the
   background countdown. **When the countdown reaches 0:00, DisplayTask
   forces a jump to `Complete-Decision` regardless of which screen is
   currently displayed** — if the user has navigated away to
   `SetTemp-Decision` etc. while a proof runs in the background (per
   the heat-lifecycle rule in `states_modes.md`), the countdown
   expiring interrupts whatever screen they're on.
   If the active countdown reaches 0:00 during a run edit, DisplayTask
   discards all proposed temperature and time changes before entering
   `Complete-Decision`. The active countdown takes precedence over an
   unconfirmed edit.

4. **Flash-EEPROM persistence is owned by DisplayTask**, written
   synchronously at the moment of Enter/confirm in Settings mode — not
   on every Up/Down tap (per `ways-of-working.md`). Verified against
   the STM32L476RG datasheet (Table 63) and ST's own reference-manual
   text for this part: a 2KB page erase/program takes ~21–24ms typical
   and, because this firmware runs as a single image in one flash
   bank (no dual-bank split configured), **the entire MCU — every
   task, every ISR, the RTOS tick — stalls for that duration**, not
   just DisplayTask. Accepted as harmless for this product: it happens
   once, at a user-initiated confirm, on a thermal control loop that
   tolerates far worse than a 24ms gap. Splitting flash into a
   code bank and a dedicated EEPROM bank (eliminating the stall
   entirely) remains possible later via the still-open Build
   Configuration Strategy decision (`arch.md` §7), but isn't worth the
   added complexity now.

5. **`qSenseToHeat`'s Queue Size, set to CubeMX's default of 16 during
   initial setup, is corrected to 1.** Temperature is
   continuously-updated state, not a discrete event — depth 16 would
   let stale readings queue up and be processed oldest-first if
   HeatTask ever falls behind, which is the opposite of what a control
   loop wants. Depth 1 with overwrite semantics guarantees HeatTask
   always acts on the single freshest reading.

6. **Current-state queues use drain-then-put.** `qSenseToHeat` and
   `qHeatToDisplay` have depth 1 because each message replaces an older
   reading or status. Before sending, the producer makes a non-blocking
   `osMessageQueueGet()` call to discard an unread value, then calls
   `osMessageQueuePut()`. A plain put to a full depth-1 queue would fail
   with `osErrorResource`.

   `qDisplayToHeat` is different: it has depth 4 and uses FIFO semantics.
   `HeatTask` must receive every setpoint, run, and stop command in order;
   its producer must not discard queued commands.

   `qUartRxToConnect` is a placeholder for future connectivity work.
   Its queue design must be reviewed before UART reception is implemented;
   received bytes must not be discarded under a "latest value wins" rule.

---

## STM32CubeMX field values

Tasks are already fully configured and need no changes — listed here
for reference only, matching what's on the Tasks and Queues screen
today:

| Task Name | Priority | Stack Size (Words) | Entry Function | Code Generation Option | Parameter | Allocation | Buffer Name | Control Block Name |
|---|---|---|---|---|---|---|---|---|
| SenseTask | osPriorityNormal | 128 | StartSenseTask | Default | NULL | Dynamic | NULL | NULL |
| DisplayTask | osPriorityLow | 128 | StartDisplayTask | Default | NULL | Dynamic | NULL | NULL |
| ConnectTask | osPriorityHigh | 128 | StartConnectTask | Default | NULL | Dynamic | NULL | NULL |
| HeatTask | osPriorityHigh3 | 128 | StartHeatTask | Default | NULL | Dynamic | NULL | NULL |
| InputTask | osPriorityAboveNormal | 128 | StartInputTask | Default | NULL | Dynamic | NULL | NULL |

The following queue settings match the current CubeMX configuration:

| Queue Name | Queue Size | Item Size | Allocation | Buffer Name | Control Block Name |
|---|---|---|---|---|---|
| qSenseToHeat | **1** (change from 16) | uint16_t | Dynamic | NULL | NULL |
| qInputToDisplay | 8 | uint8_t | Dynamic | NULL | NULL |
| qDisplayToHeat | 4 | HeatCommand_t | Dynamic | NULL | NULL |
| qHeatToDisplay | 1 | HeatStatus_t | Dynamic | NULL | NULL |
| qUartRxToConnect | 1 | uint8_t | Dynamic | NULL | NULL |

Notes on the Item Size column:
- **`qInputToDisplay` → `uint8_t`**: the button-event enum (Up, Down,
  Mode, Enter, `EVT_ENTER_SETTINGS`, etc.) fits in a byte. CubeMX just
  needs a type name here; the actual `ButtonEvent_t` enum typedef
  isn't written yet — it'll live in `App/Common/app_types.h` when
  InputTask/DisplayTask are implemented, but a plain `uint8_t` is
  enough to get the queue itself generated now.
- **`qDisplayToHeat` → `HeatCommand_t`**: a struct carrying
  setpoint/timer/run/stop, since it's more than one value. This type
  doesn't exist in code yet either — same story, define it in
  `App/Common/app_types.h` before this compiles. CubeMX doesn't
  validate the string, it just uses it verbatim in generated code.
- **`qHeatToDisplay` → `HeatStatus_t`**: same pattern, temp + relay
  on/off status.
- **`qUartRxToConnect` → `uint8_t`**: still a placeholder per
  `arch.md` — the real ESP32 AT-command protocol isn't designed yet,
  so this is deliberately provisional.