# States & Modes — Transition Table (Mechanics Only)

This file defines the state machine: states, transitions, triggers, and
timeouts. It does **not** define screen text, wording, or formatting —
that content lives in each mode's own file (see "Content owner" column).

---

## Global rules (apply across all states unless noted)

- **Toggle pairs**: some states are actually a pair of screens that
  auto-alternate every 4 seconds, forever, until an input or timeout ends
  them. Up/Down input works identically on either half of the pair — the
  4s toggle is purely cosmetic display behavior and never gates input,
  and pressing Up/Down does **not** reset or interrupt the toggle timing.
- **3-minute inactivity timeout**: applies to every state except
  `Idle-Splash`/`Idle-Prompt` (no timeout, infinite toggle) and
  `Run-Active`/`Complete-Decision` (no timeout — an active or
  undecided proof is never auto-abandoned). On timeout, always return to
  `Idle-Splash` **and** stop heat/relay control if a proof was active.
- **Heat control lifecycle**: On a new proof, heating begins only when
  Enter is pressed at `Run-Decision`. During an edit of an active run,
  the existing heater setpoint and countdown continue unchanged while
  the user navigates the temperature and time screens. Up/Down changes
  proposed values only. Enter at `Run-Decision` applies the proposed
  changes and returns to `Run-Active`. If the time was changed, a new
  countdown begins at the confirmed duration; elapsed time from the
  previous countdown is ignored. Mode at `Run-Decision` stops the run,
  turns the heater off, and returns to `Idle-Splash`. A 3-minute
  inactivity timeout on an applicable screen also stops the run.
  For a timed run, reaching 0:00 stops heating immediately, regardless
  of the screen currently displayed.

---

## State name cross-reference

| State name | Content owner |
|---|---|
| `Idle-Splash` | `idle_off_mode.md` |
| `Idle-Prompt` | `idle_off_mode.md` |
| `SetTemp-Decision` | `set_temp_mode.md` |
| `SetTemp-Adjust` | `set_temp_mode.md` |
| `SetTemp-Confirm` | `set_temp_mode.md` |
| `SetTime-Decision` | `set_time_mode.md` |
| `SetTime-Adjust` | `set_time_mode.md` |
| `SetTime-Confirm` | `set_time_mode.md` |
| `Run-Decision` | `run_mode.md` |
| `Run-Active` | `run_mode.md` |
| `Complete-Decision` | `complete_mode.md` |
| `Complete-DisplayA` | `complete_mode.md` |
| `Complete-DisplayB` | `complete_mode.md` |
| `Settings-Splash` | `settings_mode.md` |
| `Settings-Adjust` | `settings_mode.md` |
| `Settings-Confirm` | `settings_mode.md` |

---

## Transition table

“New proof” means the user entered setup from Idle. “Run edit” means
the user entered setup by pressing Mode during `Run-Active`.
Temperature Adjust/Confirm screens alternate every 4 seconds.
Time Adjust/Confirm screens alternate every 2 seconds.
Enter and Up/Down work on either screen.

| State | Up/Down | Enter → | Mode → | Timeout → |
|---|---|---|---|---|
| `Idle-Splash` | — | — | `SetTemp-Decision` (new proof) | — |
| `Idle-Prompt` | — | — | `SetTemp-Decision` (new proof) | — |
| `SetTemp-Decision` | — | `SetTemp-Adjust` | New proof: `Idle-Splash`; run edit: `SetTime-Decision` (skip temperature edit) | `Idle-Splash`, stop run if active |
| `SetTemp-Adjust` | Change proposed temperature | `SetTime-Decision` (retain proposal) | New proof: `Idle-Splash`; run edit: `SetTime-Decision` (discard temperature proposal) | `Idle-Splash`, stop run if active |
| `SetTemp-Confirm` | Change proposed temperature | `SetTime-Decision` (retain proposal) | New proof: `Idle-Splash`; run edit: `SetTime-Decision` (discard temperature proposal) | `Idle-Splash`, stop run if active |
| `SetTime-Decision` | — | `SetTime-Adjust` | `Run-Decision` (skip time edit; new proof is untimed) | `Idle-Splash`, stop run if active |
| `SetTime-Adjust` | Change proposed time | `Run-Decision` (retain proposal) | `SetTime-Decision` (discard time proposal) | `Idle-Splash`, stop run if active |
| `SetTime-Confirm` | Change proposed time | `Run-Decision` (retain proposal) | `SetTime-Decision` (discard time proposal) | `Idle-Splash`, stop run if active |
| `Run-Decision` | — | New proof: apply settings and start `Run-Active`; run edit: apply proposed changes and continue `Run-Active` | Stop run, heater off, `Idle-Splash` | `Idle-Splash`, stop run if active |
| `Run-Active` | — | `Complete-Decision` (ask whether to end proof) | `SetTemp-Decision` (run edit) | — |
| `Complete-Decision` | — | Heater off, `Complete-DisplayA` | Opened manually: return to `Run-Active`, proof unchanged; reached at 0:00: `SetTemp-Decision` | — |
| `Complete-DisplayA` | — | — | `SetTemp-Decision` | `Idle-Splash` |
| `Complete-DisplayB` | — | — | `SetTemp-Decision` | `Idle-Splash` |
| `Settings-Splash` | — | — | — | — |
| `Settings-Adjust` | Change proposed setting | Save, `Idle-Splash` | `Settings-Confirm` | `Idle-Splash` |
| `Settings-Confirm` | Change proposed setting | Save, `Idle-Splash` | `Settings-Adjust` | `Idle-Splash` |

Automatic screen changes:

- `Idle-Splash` and `Idle-Prompt` alternate every 4 seconds.
- `SetTemp-Adjust` and `SetTemp-Confirm` alternate every 4 seconds.
- `SetTime-Adjust` and `SetTime-Confirm` alternate every 2 seconds.
- `Complete-DisplayA` and `Complete-DisplayB` alternate every 4 seconds.
- `Settings-Adjust` and `Settings-Confirm` alternate every 2 seconds.
- `Settings-Splash` advances to `Settings-Adjust` after 2 seconds.
- A timed countdown reaching 0:00 opens `Complete-Decision`, including
  if the user is on an edit screen.
- If the active countdown reaches 0:00 during a run edit, immediately
  discard all proposed temperature and time changes and enter
  `Complete-Decision`. Complete Mode takes precedence over the edit,
  regardless of which edit screen is displayed.
- From either Idle screen, holding Up and Down for 5 seconds opens
  `Settings-Splash`.
