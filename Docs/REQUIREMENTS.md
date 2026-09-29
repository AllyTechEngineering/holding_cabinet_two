# Firmware Requirements

## 1. Purpose

This document is the authoritative source for the behavioral requirements of
the Holding Cabinet / Proofing Oven firmware.

The requirements are developed incrementally as the prototype is designed,
implemented, and tested. A requirement does not need to be defined before
development begins if the required behavior has not yet been determined.

Implementation details are maintained in the firmware source code,
`ARCHITECTURE.md`, and the STM32CubeMX project as appropriate.


## 2. Requirement Conventions

Each requirement is assigned a unique identifier. Requirement identifiers are
not reused or renumbered after assignment.

The following terms are used for unresolved items:

- **TBD (To Be Determined):** The requirement or value has not yet been decided.
- **TBC (To Be Confirmed):** The expected requirement or value is known but
  still requires confirmation.

TBD and TBC entries are resolved as the design matures.


## 3. System Requirements

### SYS-001 — Operating Modes

The firmware shall support both timed and untimed proofing operation.


### SYS-002 — Temperature Control

The firmware shall control the cabinet heater to maintain a user-selected
proofing temperature.


### SYS-003 — Local User Interface

The firmware shall provide a local user interface that allows the user to
configure and operate the proofing process.


### SYS-004 — Timed Proofing Completion

For a timed proofing operation, the firmware shall notify the user when the
configured proofing time has elapsed.


### SYS-005 — Power-Up State

The firmware shall enter the Idle state following power-up or reset.

The firmware shall not automatically resume a proofing run following a
power interruption or reset.


## 4. Temperature Requirements

### TEMP-001 — Setpoint Range

The firmware shall allow the user to select a proofing temperature from
65°F through 120°F, inclusive, when Fahrenheit is selected.

The firmware shall allow the user to select a proofing temperature from
18°C through 49°C, inclusive, when Celsius is selected.

The firmware shall prevent the user from selecting a temperature outside
the applicable range.


### TEMP-002 — Temperature Adjustment

The firmware shall adjust the temperature setpoint in 1-degree increments
when the user presses the Up or Down button.

The firmware shall support temperature display and adjustment in degrees
Fahrenheit and degrees Celsius.


### TEMP-003 — Setpoint Below Cabinet Temperature

The firmware shall allow a valid temperature setpoint below the current
cabinet temperature.

The heater shall remain off while the cabinet temperature is above the
setpoint.


### TEMP-004 — Temperature Units

The firmware shall support Fahrenheit and Celsius temperature units.

Fahrenheit shall be the default temperature unit.


### TEMP-005 — Temperature Regulation

The system shall maintain the cabinet temperature within ±3°F of the
user-selected temperature.

### TEMP-006 — Measured Temperature Validity

The user-selectable proofing-temperature range shall not be used as the
validity range for measured cabinet temperature.

A measured cabinet temperature may be below the minimum selectable setpoint or
above the maximum selectable setpoint without being considered a sensor fault.

Temperature-sensor faults shall be determined from the NTC sensor electrical
measurement and the applicable sensor-fault detection criteria.

## 5. Timer Requirements

### TIME-001 — Timed and Untimed Operation

The user shall be able to configure a proofing run with or without a
countdown timer.


### TIME-002 — Timer Range

The countdown timer shall support durations from 15 minutes through
10 hours, inclusive.


### TIME-003 — Initial Timer Value

When configuring a new timed proofing run, the initial timer value shall be
1 hour.


### TIME-004 — Timer Adjustment

A single Up or Down button press shall change the timer by 1 minute.

Holding the Up or Down button shall automatically repeat the adjustment.

The hold acceleration shall operate as follows:

- Less than 0.6 seconds: no automatic repeat.
- 0.6 seconds through less than 2 seconds: 1 minute every 200 ms.
- 2 seconds through less than 4 seconds: 5 minutes every 400 ms.
- 4 seconds or longer: 5 minutes every 200 ms.

The timer shall stop at its minimum and maximum values and shall not wrap.


### TIME-005 — Countdown

A timed proofing countdown shall begin when the user confirms the start of
the proofing run.

The countdown shall continue while the user is editing an active run.


### TIME-006 — Countdown Expiration

When an active countdown reaches 0:00, the firmware shall immediately stop
heating and enter the proof-completion sequence.

Countdown expiration shall take precedence over an unconfirmed run edit.

Any proposed but unconfirmed changes shall be discarded if the countdown
expires during an edit.


### TIME-007 — Timer Display

The countdown shall be displayed in hours and minutes.

Leading zeroes shall not be required for the hour field.


## 6. User Input Requirements

### UI-001 — Controls

The local user interface shall support the following four user inputs:

- Mode
- Enter
- Up
- Down


### UI-002 — Button Response

A normal button press shall result in one user-interface action unless the
button supports an intentional hold function.


### UI-003 — Settings Entry

Holding Up and Down simultaneously for 5 seconds while the system is Idle
shall enter the Settings mode.

The Settings-entry button combination shall not enter Settings while a
proofing run is active.


### UI-004 — Inactivity Timeout

Applicable configuration and editing screens shall return to Idle after
3 minutes without user activity.

If an active proof is being edited when the inactivity timeout occurs, the
proofing run shall stop and the heater shall be turned off.

The inactivity timeout shall not apply while the system is in Idle,
Run-Active, or Complete-Decision.


## 7. Proof Setup Requirements

### SETUP-001 — New Proof Setup

A new proofing run shall allow the user to configure temperature before
starting the run.


### SETUP-002 — Timer Selection

The user shall be given the option to configure a countdown timer.

Skipping timer configuration shall result in an untimed proofing run.


### SETUP-003 — Start Confirmation

Heating shall not begin while the user is configuring a new proofing run.

The firmware shall begin the proofing run and enable temperature control only
after the user confirms the run from the Run-Decision state.


### SETUP-004 — Cancel Setup

The user shall be able to cancel setup before starting a proofing run.

Cancelling setup shall return the system to Idle without energizing the
heater.


## 8. Active Run Requirements

### RUN-001 — Active Run Display

During an active run, the firmware shall display the current cabinet
temperature.

During a timed run, the firmware shall also display the remaining proofing
time.

During an untimed run, the firmware shall indicate that the countdown timer
is not being used.


### RUN-002 — Run Editing

The user shall be able to enter the setup sequence while a proofing run is
active to propose changes to the temperature and/or timer.


### RUN-003 — Control During Editing

The currently confirmed temperature setpoint and countdown shall remain
active while the user edits a running proof.

Proposed changes shall not affect the active run until confirmed.


### RUN-004 — Apply Edited Temperature

A proposed temperature change shall become active only when the user confirms
the changes at Run-Decision.


### RUN-005 — Apply Edited Time

When the user confirms a proposed timer duration at Run-Decision during an
active-run edit, a new countdown shall begin using that duration, even if
it equals the previously confirmed duration.

Elapsed time from the previous countdown shall not be subtracted from the
new duration.


### RUN-006 — Preserve Countdown When No Timer Proposal Is Confirmed

During an active-run edit, skipping timer adjustment or discarding the
proposed timer value shall leave the existing countdown running without
restarting.

Confirming a proposed timer duration at Run-Decision shall start a new
countdown using that duration, even if it equals the previously confirmed
duration.

### RUN-007 — Cancel Active Run

The user shall be able to stop an active proofing run.

Stopping the run shall turn the heater off and return the firmware to Idle.


## 9. Proof Completion Requirements

### COMPLETE-001 — Manual Completion Request

The user shall be able to request completion of an active proofing run.

The firmware shall request confirmation before manually ending the proof.


### COMPLETE-002 — Cancel Manual Completion

If the user declines a manually requested completion, the firmware shall
return to the active run without changing the confirmed temperature or
remaining countdown time.


### COMPLETE-003 — Timed Completion

When a timed proof reaches 0:00, the firmware shall enter the completion
sequence and the heater shall remain off.


### COMPLETE-004 — Completion Confirmation

When the user confirms completion, the firmware shall stop heating and
display proof-complete information.


### COMPLETE-005 — Start Another Proof

Following completion, the user shall be able to return to the proof setup
sequence to configure another proof.


## 10. Audible Alert Requirements

### AUD-001 — Completion Alert

Each entry into the Complete-Decision state shall generate one audible
sequence of three beeps.

This shall apply to both manually requested completion and automatic
countdown expiration.


### AUD-002 — Beep Timing

Each completion beep shall sound for 200 ms.

The silence between completion beeps shall be 300 ms.


### AUD-003 — Buzzer Default State

The buzzer shall be off following power-up.

No audible alerts other than the proof-completion alert are currently
required.


## 11. Display Requirements

### DISP-001 — Display

The firmware shall provide the required local operating information using
the cabinet LCD.


### DISP-002 — HMI Specification

The firmware shall implement the LCD screens, screen sequencing, display
timing, and user interaction defined in `HMI.md`.


## 12. Persistence Requirements

### PERSIST-001 — Persistent Configuration

The firmware shall retain the following confirmed configuration values across
power cycles:

- Temperature setpoint
- Temperature unit
- Timer duration


### PERSIST-002 — Save Point

Temperature setpoint and timer duration shall be saved when the user confirms
the run configuration.

Temperature units shall be saved when the user confirms the Settings
selection.


### PERSIST-003 — Proposed Values

Unconfirmed temperature or timer adjustments shall not be stored as persistent
configuration.


### PERSIST-004 — Active Run State

Active proofing state and elapsed or remaining run time shall not be restored
following power loss or reset.


## 13. Heater Output Requirements

### HEAT-001 — Heater Default State

The firmware heater command shall be off following power-up or reset.


### HEAT-002 — Heater Run Enable

The firmware shall not command the heater on unless a proofing run has been
started and temperature control is active.


### HEAT-003 — Heater Stop Conditions

The firmware shall command the heater off when:

- A proofing run is stopped.
- A timed proof reaches 0:00.
- A firmware-detected fault requires heating to stop.
- The system returns to Idle from an active run.


### HEAT-004 — Heater Indicator

The firmware-controlled Heater indicator shall be on only while the firmware
is commanding the heater to energize.

The Heater indicator shall be off when the heater command is off.

The Heater indicator represents the firmware command and does not verify
actual heater current.


## 14. Fault Handling Requirements

### FAULT-001 — NTC Fault Detection

The firmware shall detect an open or shorted cabinet temperature sensor.

The detection thresholds are TBD.


### FAULT-002 — NTC Fault Codes

The firmware shall report the following fault codes:

- Error 10: NTC open
- Error 11: NTC short


### FAULT-003 — Fault Response

When a firmware-detected fault requiring shutdown occurs, the firmware shall
command the heater off and enter the Error state.


### FAULT-004 — Error Display

The Error state shall display the active error code and instruct the user to
turn the proofer off and contact support.


### FAULT-005 — Error-State Inputs

While in the Error state, normal user-interface commands shall not resume
operation.

Recovery behavior from a firmware fault is TBD.


## 15. Settings Requirements

### SETTINGS-001 — Temperature Units

The Settings mode shall allow the user to select Fahrenheit or Celsius.

Fahrenheit shall be the default setting.


### SETTINGS-002 — Settings Confirmation

A changed setting shall become persistent when the user confirms the setting.


### SETTINGS-003 — Additional Settings

Additional Settings-mode functionality is TBD.


## 16. Connectivity Requirements

### CONN-001 — Connectivity Status

Remote connectivity is not required for the current proof-of-concept firmware.