# Integration Tests

## Test Setup

Equipment:
- Powered controller with LCD, buttons, and temperature sensor.
- Debugger with firmware symbols.

Record for each execution:
- Technician and date.
- Hardware identification.
- Firmware commit and any uncommitted changes.
- Test procedure revision.
- Actual observations.
- PASS, FAIL, BLOCKED, or NOT RUN.

Use debugger breakpoints to inspect internal state. Do not use a
debugger-paused execution to measure countdown or inactivity accuracy.

For DisplayTask state checks, place a breakpoint at the beginning of
the main loop in DisplayTask_Run, after initialization. Inspect the
locals in that function. This allows the preceding loop's state
changes to finish before inspection.

Disable inspection breakpoints while operating the controls.
Enable them when ready to capture the resulting state.

## IT-001 — Button Events Reach DisplayTask

### Components
Switch driver, InputTask, input queue, DisplayTask.

### Procedure
1. In App/Display/display_task.c, set a breakpoint inside the
   successful input queue receive branch, before event processing.
2. With the firmware running, press Up and hold it until the
   breakpoint is reached.
3. Record event.
4. Resume execution, then release Up.
5. At the next breakpoint, record event.
6. Repeat for Down, Mode, and Enter.

### Expected
Each button produces its corresponding press and release events:

| Button | Press event | Release event |
|---|---|---|
| Up | EVT_UP_PRESSED | EVT_UP_RELEASED |
| Down | EVT_DOWN_PRESSED | EVT_DOWN_RELEASED |
| Mode | EVT_MODE_PRESSED | EVT_MODE_RELEASED |
| Enter | EVT_ENTER_PRESSED | EVT_ENTER_RELEASED |

### Record
Observed event sequence for each button and any missing or
unexpected events.

## IT-002 — Temperature Data Reaches the Run Display

### Components
SenseTask, sensor queue, HeatTask, status queue, DisplayTask.

### Starting Conditions
A valid sensor is connected and Run Active is displayed.

### Procedure
1. In App/Sensing/sensor_task.c, break at the call to
   sense_push_reading(tempTenthsC).
2. Record tempTenthsC.
3. Disable that breakpoint.
4. In App/Control/control_task.c, break at publish_status(&status).
5. Resume and record reading, status.currentTempTenthsC, and
   status.errorCode.
6. Disable that breakpoint.
7. In App/Display/display_task.c, break inside display_run_screen
   and inspect the HeatStatus_t object addressed by status.
8. Record its temperature and error code.
9. Disable the breakpoint and resume.
10. Observe the LCD temperature.
11. Repeat after a noticeable change in sensor temperature.

### Expected
- HeatTask copies its received reading into
  status.currentTempTenthsC.
- A valid reading has errorCode equal to zero.
- DisplayTask receives updated temperature status.
- The LCD shows that temperature converted to the selected unit
  and rounded to a whole degree.

Readings captured at different times may differ. Compare a stable
reading or capture the corresponding sample at each stage.

### Record
Sensor, HeatTask, and DisplayTask readings; error codes;
selected unit; and LCD temperature.

## IT-003 — Time Editor Passes a Proposed Duration

### Components
DisplayTask and TimeEditor.

### Starting Conditions
A timed run is active.

### Procedure
1. Open run editing with Mode.
2. Skip temperature editing with Mode.
3. Enter time adjustment with Enter.
4. Capture DisplayTask state.
5. Record time_editor.minutes and run_timer.
6. Resume and select 20 minutes.
7. Press Enter to reach Apply Changes.
8. Capture DisplayTask state again.

### Expected
- Opening time adjustment initializes time_editor.minutes to 60.
- Confirming 20 minutes leaves time_editor.minutes equal to 20.
- timed_proof is 1 at Apply Changes.
- The existing run_timer mode, duration_minutes, and start_ms
  remain unchanged before Apply Changes is confirmed.

### Record
Editor minutes, timed_proof, and timer fields at both checkpoints.

## IT-004 — Temperature Changes Preserve the Active Timer

### Components
DisplayTask and RunTimer.

### Starting Conditions
A timed run is active with enough time remaining to complete the test.

### Procedure
1. Capture active_temp and all run_timer fields.
2. Resume and press Mode to edit the run.
3. Enter temperature adjustment.
4. Select a different temperature and confirm it.
5. Skip time editing with Mode.
6. Confirm Apply Changes with Enter.
7. Capture DisplayTask state.

### Expected
- screen is UI_RUN_ACTIVE.
- active_temp contains the confirmed temperature.
- editing_run is 0.
- run_timer.mode, duration_minutes, and start_ms match the
  original captured values.

### Record
Temperature and timer fields before and after applying changes.

## IT-005 — Applying a Duration Restarts the Timer

### Components
DisplayTask, TimeEditor, and RunTimer.

### Starting Conditions
A timed run is active with a duration other than 20 minutes.

### Procedure
1. Capture the existing run_timer fields.
2. Resume and open run editing.
3. Skip temperature editing.
4. Enter time adjustment and select 20 minutes.
5. Confirm the proposed time.
6. Capture run_timer at Apply Changes.
7. Resume and confirm Apply Changes.
8. Capture DisplayTask state.

### Expected
- Before Apply Changes is confirmed, run_timer retains its
  original mode, duration_minutes, and start_ms.
- After confirmation, screen is UI_RUN_ACTIVE.
- run_timer.mode is RUN_TIMER_TIMED.
- run_timer.duration_minutes is 20.
- run_timer.start_ms contains the new start tick.
- editing_run is 0.

### Record
Timer fields before editing, before applying, and after applying.

## IT-006 — Discarding a Duration Preserves the Active Timer

### Components
DisplayTask, TimeEditor, and RunTimer.

### Starting Conditions
A timed run is active.

### Procedure
1. Capture all run_timer fields.
2. Resume and open run editing.
3. Skip temperature editing.
4. Enter time adjustment and select a different duration.
5. Press Mode to discard that duration.
6. Press Mode to skip time editing.
7. Confirm Apply Changes with Enter.
8. Capture DisplayTask state.

### Expected
- screen is UI_RUN_ACTIVE.
- run_timer.mode, duration_minutes, and start_ms match the
  original captured values.
- The discarded duration has not replaced the active timer.

### Record
Timer fields before editing and after applying.

## IT-007 — Cancel and Inactivity Stop the Run Timer

### Components
DisplayTask and RunTimer.

### Procedure A — Cancel
1. Start a timed run.
2. Press Mode to open run editing.
3. Skip temperature and time editing to reach Apply Changes.
4. Press Mode to cancel.
5. Capture DisplayTask state.

### Expected A
- The display has returned to an idle screen.
- run_timer.mode is RUN_TIMER_INACTIVE.
- editing_run and timed_proof are 0.

### Procedure B — Inactivity
1. Start another timed run with more than three minutes remaining.
2. Open run editing.
3. Leave the controls untouched until the display returns to idle.
4. Capture DisplayTask state.

### Expected B
- run_timer.mode is RUN_TIMER_INACTIVE.
- editing_run and timed_proof are 0.

### Record
Timer mode and flags for each procedure.
Record a separate result for A and B.

## IT-008 — Expiry Overrides an Unconfirmed Run Edit

### Components
RunTimer, DisplayTask, and TimeEditor.

### Starting Conditions
A timed run is nearing expiry.

### Procedure
1. Capture active_temp.
2. Resume and open run editing.
3. Skip temperature editing.
4. Enter time adjustment and select a new duration.
5. Leave the new duration unconfirmed.
6. Allow the original countdown to expire.
7. When Complete Decision appears, capture DisplayTask state.

### Expected
- screen is UI_COMPLETE_DECISION.
- complete_due_to_timeout is 1.
- run_timer.mode is RUN_TIMER_INACTIVE.
- editing_run and timed_proof are 0.
- proposed_temp equals active_temp.
- The unconfirmed duration has not started a new run.

### Record
Screen, timer mode, flags, and temperature values.