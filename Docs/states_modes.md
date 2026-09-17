# States & Modes — Transition Table (Mechanics Only)

This file defines the state machine: states, transitions, triggers, and
timeouts. It does **not** define screen text, wording, or formatting —
that content lives in each mode's own file (see "Content owner" column).

---

## Global rules (apply across all states unless noted)

- **Toggle pairs**: some states are actually a pair of screens that
  auto-alternate every 2 seconds, forever, until an input or timeout ends
  them. Up/Down input works identically on either half of the pair — the
  2s toggle is purely cosmetic display behavior and never gates input,
  and pressing Up/Down does **not** reset or interrupt the toggle timing.
- **3-minute inactivity timeout**: applies to every state except
  `Idle-Splash`/`Idle-Prompt` (no timeout, infinite toggle) and
  `Run-Active`/`Complete-Decision` (no timeout — an active or
  undecided proof is never auto-abandoned). On timeout, always return to
  `Idle-Splash` **and** stop heat/relay control if a proof was active.
- **Heat control lifecycle**: heat never turns on until `Enter` is
  pressed at `Run-Decision` (entering `Run-Active`). Once on, heat
  keeps running in the background regardless of manual Mode-button
  navigation through `SetTemp-Decision` / `SetTemp-Adjust` /
  `SetTemp-Confirm` / `SetTime-Decision` / `SetTime-Adjust` /
  `SetTime-Confirm` / `Run-Decision`. Heat stops **only** via:
  (a) `Enter` at `Complete-Decision`, or (b) the 3-minute timeout.
- **Settings-entry combo**: Up+Down held simultaneously for 5 seconds,
  detected only from `Idle-Splash`/`Idle-Prompt`.

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

| State               | Toggles with                               | Up/Down                           | Enter →                         | Mode →                      | Timeout →     |
|---------------------|--------------------------------------------|-----------------------------------|---------------------------------|-----------------------------|---------------|
| `Idle-Splash`       | `Idle-Prompt` (2s)                         | —                                 | —                               | `SetTemp-Decision`          | — (none)      |
| `Idle-Prompt`       | `Idle-Splash` (2s)                         | —                                 | —                               | `SetTemp-Decision`          | — (none)      |
| `SetTemp-Decision`  | —                                          | —                                 | `SetTemp-Adjust`                | `Idle-Splash`               | `Idle-Splash` |
| `SetTemp-Adjust`    | `SetTemp-Confirm` (2s)                     | adjust temp                       | `SetTime-Decision` (commit)     | `Idle-Splash`               | `Idle-Splash` |
| `SetTemp-Confirm`   | `SetTemp-Adjust` (2s)                      | adjust temp                       | `SetTime-Decision` (commit)     | `Idle-Splash`               | `Idle-Splash` |
| `SetTime-Decision`  | —                                          | —                                 | `SetTime-Adjust`                | `Run-Decision` (skip timer) | `Idle-Splash` |
| `SetTime-Adjust`    | `SetTime-Confirm` (2s)                     | adjust time | `Run-Decision` (commit)         | `SetTime-Decision`          | `Idle-Splash` |
| `SetTime-Confirm`   | `SetTime-Adjust` (2s)                      | adjust time                       | `Run-Decision` (commit)         | `SetTime-Decision`          | `Idle-Splash` |
| `Run-Decision`      | —                                          | —                                 | `Run-Active` (heat on / resume) | `Idle-Splash`               | `Idle-Splash` |
| `Run-Active`        | (untimed variant only, internal 2s toggle) | —                                 | —                               | `Complete-Decision`         | — (none)      |
| `Complete-Decision` | —                                          | —                                 | `Complete-DisplayA` (heat off)  | `SetTemp-Decision`          | — (none)      |
| `Complete-DisplayA` | `Complete-DisplayB` (2s)                   | —                                 | —                               | `SetTemp-Decision`          | `Idle-Splash` |
| `Complete-DisplayB` | `Complete-DisplayA` (2s)                   | —                                 | —                               | `SetTemp-Decision`          | `Idle-Splash` |
| `Settings-Splash`   | —                                          | —                                 | —                               | —                           | —             |
| `Settings-Adjust`   | `Settings-Confirm` (2s)                    | toggle setting value              | save, `Idle-Splash`             | `Settings-Confirm`          | `Idle-Splash` |
| `Settings-Confirm`  | `Settings-Adjust` (2s)                     | toggle setting value              | save, `Idle-Splash`             | `Settings-Adjust`           | `Idle-Splash` |

Also from `Idle-Splash`/`Idle-Prompt`: **Up+Down held 5s** → `Settings-Splash`.

## Auto transitions (not user-triggered)

- `Settings-Splash`: shown once for 2s → `Settings-Adjust` automatically.
- `Run-Active` (timed variant): countdown reaching 0:00 → `Complete-Decision` (identical to a manual Mode press).
