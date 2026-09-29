# Human-Machine Interface Specification

## 1. Purpose

This document defines the Human-Machine Interface (HMI) for the Holding
Cabinet / Proofing Oven firmware.

This document is the authoritative source for:

- LCD screen wording.
- LCD character placement.
- HMI states.
- User input behavior.
- HMI state transitions.
- Automatic screen changes.
- HMI timing.
- Run-edit interaction.
- Completion interaction.
- Error displays.
- Audible completion indication.
- Firmware-controlled Heater indication.

The LCD is a 16-column by 2-row character display.

Position 1 is the leftmost LCD character position.

Unless explicitly defined as a variable field, spaces shown in the screen
definitions are intentional.


## 2. User Controls

The front panel provides four user controls:

- Mode
- Enter
- Up
- Down

The physical switches are active-low.

A normal button press produces one HMI action unless the button supports an
intentional hold function.

Up and Down are also used together to enter Settings.

Holding Up and Down simultaneously for 5 seconds while in either Idle screen
opens `Settings-Splash`.


## 3. Global HMI Behavior

### 3.1 Screen Alternation

The following screen pairs automatically alternate:

| Screen Pair | Interval |
|---|---:|
| `Idle-Splash` / `Idle-Prompt` | 4 seconds |
| `SetTemp-Adjust` / `SetTemp-Confirm` | 4 seconds |
| `SetTime-Adjust` / `SetTime-Confirm` | 2 seconds |
| `Complete-DisplayA` / `Complete-DisplayB` | 4 seconds |
| `Settings-Adjust` / `Settings-Confirm` | 2 seconds |

Screen alternation is cosmetic and does not prevent user input.

Up and Down operate identically on either member of an Adjust/Confirm pair.

Pressing Up or Down does not restart the screen-alternation timer.

`Settings-Splash` is not part of a toggle pair. It automatically advances to
`Settings-Adjust` after 2 seconds.


### 3.2 Inactivity Timeout

The HMI inactivity timeout is 3 minutes.

The timeout does not apply to:

- `Idle-Splash`
- `Idle-Prompt`
- `Run-Active`
- `Complete-Decision`

When the inactivity timeout occurs on another applicable screen:

- The HMI returns to `Idle-Splash`.
- Any proposed changes are discarded.
- If a proofing run was active, the run is stopped and heating is disabled.


### 3.3 Active-Run Editing

Mode from `Run-Active` enters the run-edit sequence at
`SetTemp-Decision`.

During a run edit:

- The currently confirmed temperature remains active.
- The existing countdown continues.
- Temperature and timer changes are proposed values only.
- Proposed changes do not affect the active run until confirmed at
  `Run-Decision`.

If a proposed timer duration is confirmed at Run-Decision, a new countdown
begins using that duration, even if it equals the previously confirmed
duration. Elapsed time from the previous countdown is ignored.

Skipping timer adjustment or discarding the proposed timer value leaves
the existing countdown running without restarting.

If the active countdown reaches 0:00 during an edit:

- Heating stops.
- Proposed temperature and timer changes are discarded.
- The HMI immediately enters `Complete-Decision`.


## 4. HMI State Transition Table

“New proof” means setup was entered from Idle.

“Run edit” means setup was entered by pressing Mode during `Run-Active`.

| State | Up/Down | Enter | Mode | Timeout |
|---|---|---|---|---|
| `Idle-Splash` | — | — | `SetTemp-Decision` | — |
| `Idle-Prompt` | — | — | `SetTemp-Decision` | — |
| `SetTemp-Decision` | — | `SetTemp-Adjust` | New proof: `Idle-Splash`; run edit: `SetTime-Decision` | `Idle-Splash`; stop active run |
| `SetTemp-Adjust` | Change proposed temperature | `SetTime-Decision`, retain proposal | New proof: `Idle-Splash`; run edit: `SetTime-Decision`, discard temperature proposal | `Idle-Splash`; stop active run |
| `SetTemp-Confirm` | Change proposed temperature | `SetTime-Decision`, retain proposal | New proof: `Idle-Splash`; run edit: `SetTime-Decision`, discard temperature proposal | `Idle-Splash`; stop active run |
| `SetTime-Decision` | — | `SetTime-Adjust` | `Run-Decision`; new proof becomes untimed | `Idle-Splash`; stop active run |
| `SetTime-Adjust` | Change proposed time | `Run-Decision`, retain proposal | `SetTime-Decision`, discard time proposal | `Idle-Splash`; stop active run |
| `SetTime-Confirm` | Change proposed time | `Run-Decision`, retain proposal | `SetTime-Decision`, discard time proposal | `Idle-Splash`; stop active run |
| `Run-Decision` | — | New proof: apply settings and start run; run edit: apply changes and continue run | Stop run, heater off, `Idle-Splash` | `Idle-Splash`; stop active run |
| `Run-Active` | — | `Complete-Decision` | `SetTemp-Decision` | — |
| `Complete-Decision` | — | Heater off, `Complete-DisplayA` | Manual entry: `Run-Active`; timer expiration: `SetTemp-Decision` | — |
| `Complete-DisplayA` | — | — | `SetTemp-Decision` | `Idle-Splash` |
| `Complete-DisplayB` | — | — | `SetTemp-Decision` | `Idle-Splash` |
| `Settings-Splash` | — | — | — | — |
| `Settings-Adjust` | Change proposed setting | Save setting, `Idle-Splash` | `Settings-Confirm` | `Idle-Splash` |
| `Settings-Confirm` | Change proposed setting | Save setting, `Idle-Splash` | `Settings-Adjust` | `Idle-Splash` |


## 5. Idle Screens

### 5.1 Idle-Splash

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | | | | | T | a | y | l | o | r | | | | | |
| Row 2 | | P | r | o | o | f | i | n | g | | O | v | e | n | | |

As displayed:

    "     Taylor     "
    " Proofing Oven  "


### 5.2 Idle-Prompt

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | T | o | | S | t | a | r | t | | | | | | | |
| Row 2 | | P | r | e | s | s | | M | o | d | e | | | | | |

As displayed:

    " To Start       "
    " Press Mode     "


## 6. Set Temperature Screens

### 6.1 SetTemp-Decision

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | T | o | | S | e | t | | T | e | m | p | | | | |
| Row 2 | | E | n | t | e | r | | Y | | M | o | d | e | | N | |

As displayed:

    " To Set Temp    "
    " Enter Y Mode N "


### 6.2 SetTemp-Adjust

### 6.2 SetTemp-Adjust

`XXX` represents the proposed temperature.

The displayed value is not padded with leading zeroes.

The unit character reflects the selected temperature unit.

The temperature range depends on the selected temperature unit.

When Fahrenheit is selected, the range is 65°F through 120°F.

When Celsius is selected, the range is 18°C through 49°C.

Up increases the proposed temperature by 1 degree.

Down decreases the proposed temperature by 1 degree.

The value stops at the minimum and maximum limits and does not wrap.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | S | e | t | | T | e | m | p | : | | X | X | X | F/C | |
| Row 2 | | U | p | + | | o | r | | D | o | w | n | - | | | |

Display format:

    " Set Temp: XXXF "
    " Up+ or Down-   "

or:

    " Set Temp: XXXC "
    " Up+ or Down-   "


### 6.3 SetTemp-Confirm

Row 1 uses the same value format as `SetTemp-Adjust`.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | S | e | t | | T | e | m | p | : | | X | X | X | F/C | |
| Row 2 | E | n | t | e | r | | Y | | M | o | d | e | | N | | |

Display format:

    " Set Temp: XXXF "
    "Enter Y Mode N  "

or:

    " Set Temp: XXXC "
    "Enter Y Mode N  "


## 7. Set Time Screens

### 7.1 SetTime-Decision

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | C | o | u | n | t | d | o | w | n | | T | i | m | e | r | |
| Row 2 | E | n | t | e | r | | Y | | M | o | d | e | | N | | |

As displayed:

    "Countdown Timer "
    "Enter Y Mode N  "


### 7.2 SetTime-Adjust

`HH:MM` represents the proposed countdown duration.

The display does not use a seconds field.

The hour field does not use leading-zero padding.

For a new proof, timer adjustment begins at 1:00.

The valid range is 0:15 through 10:00.

A single Up or Down press changes the duration by 1 minute.

Holding Up or Down repeats the adjustment as follows:

| Time Since Press | Adjustment |
|---|---|
| Less than 0.6 seconds | No repeat |
| 0.6 to less than 2 seconds | 1 minute every 200 ms |
| 2 to less than 4 seconds | 5 minutes every 400 ms |
| 4 seconds or longer | 5 minutes every 200 ms |

Repeat timing is measured from the previous repeat.

Crossing a hold threshold does not itself cause an additional adjustment.

Releasing the button stops repetition.

Pressing the opposite direction begins a new hold ramp.

The timer stops at its minimum and maximum limits and does not wrap.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | S | e | t | | T | i | m | e | : | | H | H | : | M | M |
| Row 2 | | U | p | + | | o | r | | D | o | w | n | - | | | |

Display format:

    " Set Time: HH:MM"
    " Up+ or Down-   "


### 7.3 SetTime-Confirm

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | S | e | t | | T | i | m | e | : | | H | H | : | M | M |
| Row 2 | | E | n | t | e | r | | Y | | M | o | d | e | | N | |

Display format:

    " Set Time: HH:MM"
    " Enter Y Mode N "


## 8. Run Screens

### 8.1 Run-Decision — New Proof

For a new proof, heating remains off until Enter is pressed.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | T | o | | S | t | a | r | t | | P | r | o | o | f | |
| Row 2 | | E | n | t | e | r | | Y | | M | o | d | e | | N | |

As displayed:

    " To Start Proof "
    " Enter Y Mode N "


### 8.2 Run-Decision — Active-Run Edit

The same `Run-Decision` state uses different Row 1 wording during an
active-run edit.

As displayed:

    " Apply Changes? "
    " Enter Y Mode N "

Enter applies the proposed changes and returns to `Run-Active`.

Mode stops the proof, turns heating off, and returns to `Idle-Splash`.


### 8.3 Run-Active — Timed

`XXX` represents the live cabinet temperature.

The temperature value is not padded with leading zeroes.

`HH:MM` represents the remaining countdown time.

The hour field is not padded with leading zeroes.

The displayed temperature and countdown are refreshed approximately once per
second.

Display format:

    " Temp: XXXF     "
    " Time: HH:MM    "
    
    " Temp:  XXF     "
    " Time:  H:MM    "

    " Temp:  XXF     "
    " Time:  H:MM    "

    " Temp:  XXF     "
    " Time:  0:MM    "

or when Celsius is selected:

    " Temp: XXXC     "
    " Time: HH:MM    "
    
    " Temp:  XXC     "
    " Time:  H:MM    "

    " Temp:  XXC     "
    " Time:  H:MM    "

    " Temp:  XXC     "
    " Time:  0:MM    "

The complete LCD row must be refreshed or cleared as necessary when a
variable-width value becomes shorter so that characters from a previous
value are not left on the display.


### 8.4 Run-Active — Untimed

Row 1 uses the same temperature format as the timed variant.

Row 2 alternates every 2 seconds between:

    "Countdown Timer "

and:

    "Not Used        "

Example:

    " Temp:  98F      "
    "Countdown Timer "

alternating with:

    " Temp:  98F      "
    "Not Used        "

This alternation continues while the untimed run remains active.


### 8.5 Run-Active User Actions

During either timed or untimed `Run-Active`:

- Mode opens `SetTemp-Decision` to edit the active run.
- Enter opens `Complete-Decision`.


## 9. Completion Screens

### 9.1 Complete-Decision

`Complete-Decision` is entered when:

- The user presses Enter during `Run-Active`.
- A timed countdown reaches 0:00.

Entering `Complete-Decision` initiates the three-beep completion alert.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | P | r | o | o | f | | C | o | m | p | l | e | t | e | ? |
| Row 2 | | E | n | t | e | r | | Y | | M | o | d | e | | N | |

As displayed:

    " Proof Complete?"
    " Enter Y Mode N "

If completion was entered manually:

- Enter confirms completion.
- Mode returns directly to `Run-Active`.
- The confirmed temperature and remaining countdown are unchanged.

If completion was entered because the countdown reached 0:00:

- Heating has already stopped.
- Enter proceeds to `Complete-DisplayA`.
- Mode proceeds to `SetTemp-Decision`.


### 9.2 Complete-DisplayA

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | P | r | o | o | f | | C | o | m | p | l | e | t | e | |
| Row 2 | | P | r | e | s | s | | M | o | d | e | | t | o | | |

As displayed:

    " Proof Complete "
    " Press Mode to  "


### 9.3 Complete-DisplayB

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | P | r | o | o | f | | C | o | m | p | l | e | t | e | |
| Row 2 | | S | t | a | r | t | | A | g | a | i | n | | | | |

As displayed:

    " Proof Complete "
    " Start Again    "

`Complete-DisplayA` and `Complete-DisplayB` alternate every 4 seconds.

Mode from either display enters `SetTemp-Decision` to configure another
proof.

The 3-minute inactivity timeout applies to these displays and returns the HMI
to `Idle-Splash`.


## 10. Settings Screens

Settings is entered by holding Up and Down simultaneously for 5 seconds while
the HMI is in either Idle screen.

The current Settings function selects Fahrenheit or Celsius.


### 10.1 Settings-Splash

`Settings-Splash` is displayed for 2 seconds and then automatically advances
to `Settings-Adjust`.

Row 2 shows the current temperature-unit selection.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | | S | e | t | t | i | n | g | s | | M | e | n | u | |
| Row 2 | | | | | | T | e | m | p | : | F/C | | | | | |

Example:

    "  Settings Menu "
    "     Temp:F     "


### 10.2 Settings-Adjust

Up selects Fahrenheit.

Down selects Celsius.

Row 2 displays the currently selected value.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | U | p | | F | | o | r | | D | o | w | n | | C | |
| Row 2 | | | | | | T | e | m | p | : | F/C | | | | | |

Examples:

    " Up F or Down C "
    "     Temp:F     "

or:

    " Up F or Down C "
    "     Temp:C     "


### 10.3 Settings-Confirm

Enter saves the selected temperature unit and returns to `Idle-Splash`.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | E | n | t | e | r | | Y | | M | o | d | e | | N | |
| Row 2 | | | | | | T | e | m | p | : | F/C | | | | | |

Examples:

    " Enter Y Mode N "
    "     Temp:F     "

or:

    " Enter Y Mode N "
    "     Temp:C     "


## 11. Error HMI

When a firmware-detected error places the system in the Error state:

- Heating is disabled.
- Normal button input is ignored.
- Communications input does not resume operation.
- The Error state has no inactivity timeout.

The Error display alternates its second row every 2 seconds.


### 11.1 Error Display A

`XX` represents the active two-digit error code.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | E | r | r | : | | X | X | | | | | | | | | |
| Row 2 | T | u | r | n | | P | r | o | o | f | e | r | | O | f | f |

As displayed:

    "Err: XX         "
    "Turn Proofer Off"


### 11.2 Error Display B

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | E | r | r | : | | X | X | | | | | | | | | |
| Row 2 | C | o | n | t | a | c | t | | S | u | p | p | o | r | t | |

As displayed:

    "Err: XX         "
    "Contact Support "


### 11.3 Error Codes

| Error | Code |
|---|---:|
| NTC Open | 10 |
| NTC Short | 11 |
| Heater Open | 20 |
| Heater Short | 21 |

Codes 30 through 90 remain reserved/TBD.


## 12. Audible HMI

Entering `Complete-Decision` generates one audible completion sequence.

The sequence consists of three beeps:

    Beep 1 -> silence -> Beep 2 -> silence -> Beep 3

Each beep duration is 200 ms.

The silence between beeps is 300 ms.

The sequence occurs once each time `Complete-Decision` is entered.


## 13. LED Indicators

### 13.1 System Power Indicator

The System Power indicator is hardware controlled.

It indicates that the switched system power rail is energized.

Firmware does not control this indicator.


### 13.2 Heater Indicator

The Heater indicator is firmware controlled.

The indicator is active-high.

The Heater indicator is on only while firmware is commanding the heater to
energize.

The indicator is off:

- At startup.
- When a proofing run stops.
- When a timed run reaches 0:00.
- When a fault disables heating.
- Whenever the heater command is off.

The Heater indicator represents the firmware heater command. It does not
independently indicate or verify actual heater current.