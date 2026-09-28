# Firmware Architecture

## 1. Purpose

This document defines the firmware architecture for the Holding Cabinet /
Proofing Oven.

`REQUIREMENTS.md` defines what the firmware is required to do.

This document defines how the firmware is organized to satisfy those
requirements.

Implementation details that are adequately represented by the source code do
not need to be duplicated here.

STM32 peripheral configuration, GPIO assignments, and MCU pin assignments are
maintained by the STM32CubeMX project and are not duplicated in this document.


## 2. Platform

### 2.1 Development Platform

The current proof-of-concept firmware targets:

- NUCLEO-L476RG development board
- STM32L476RG microcontroller
- FreeRTOS
- CMSIS-RTOS2
- STM32 HAL
- STM32CubeMX generated initialization code


### 2.2 Firmware Structure

The firmware uses multiple FreeRTOS tasks to separate sensing, user input,
user-interface operation, and heater control.

The primary application tasks are:

- `SenseTask`
- `InputTask`
- `DisplayTask`
- `HeatTask`
- `ConnectTask`

`ConnectTask` is reserved for future connectivity functionality and is not
required for the current proof-of-concept operation.


## 3. Architectural Responsibilities

The firmware is divided into functional responsibilities rather than placing
all application behavior in a single task.

The general data flow is:

    +-------------+
    |  InputTask  |
    +------+------+
           |
           | Button events
           v
    +-------------+      +-------------+
    | DisplayTask |----->|  HeatTask   |
    +------+------+      +------+------+
           ^                    ^
           |                    |
           |                    |
    +------+------+
    |  SenseTask  |-------------+
    +-------------+
       Temperature

`DisplayTask` also receives heater status from `HeatTask` and owns LCD/UI
operation.

The exact queue relationships are described in Section 5.


## 4. Task Architecture

### 4.1 SenseTask

`SenseTask` is responsible for cabinet temperature acquisition.

Its responsibilities include:

- Reading the NTC temperature-sensing ADC input.
- Converting the ADC measurement into cabinet temperature.
- Detecting temperature-sensor conditions required by the fault-handling
  design.
- Providing the latest temperature information to the heater-control
  subsystem.

The current NTC is an MF52B-type thermistor with:

- Nominal resistance: 10 kΩ at 25°C
- Beta value: approximately 3950 K

The thermistor is used in a resistor-divider circuit.

The temperature conversion currently uses the thermistor Beta equation.

The final sensor calibration strategy and NTC fault thresholds are TBD.


### 4.2 InputTask

`InputTask` is responsible for physical front-panel button acquisition.

The four user controls are:

- Mode
- Enter
- Up
- Down

`InputTask` performs the low-level processing necessary to convert physical
button activity into discrete application button events.

This includes:

- Button sampling.
- Button debouncing.
- Press and release detection.
- Button-hold processing where required.
- Detection of the Up+Down Settings-entry chord.

The Up+Down Settings-entry chord is detected by `InputTask`.

When both buttons remain held for the required duration, `InputTask` generates
a Settings-entry event.

`InputTask` does not determine whether the current application state permits
entry into Settings. That decision belongs to `DisplayTask`, which owns the
user-interface state machine.


### 4.3 DisplayTask

`DisplayTask` owns the front-panel application state machine.

Its responsibilities include:

- Processing button events received from `InputTask`.
- Managing UI states and state transitions.
- Managing proposed and confirmed temperature settings.
- Managing proposed and confirmed timer settings.
- Managing timed and untimed run state.
- Managing the proofing countdown.
- Managing active-run editing.
- Managing UI inactivity timing.
- Rendering information to the LCD.
- Sending confirmed run-control information to `HeatTask`.
- Receiving heater status for display purposes.
- Initiating persistence of confirmed settings when required.

`DisplayTask` operates using a periodic wake rather than waiting indefinitely
for a button event.

The current periodic wake interval is approximately 100 ms.

This allows `DisplayTask` to perform time-dependent application processing
without requiring a button event, including:

- Automatic display changes.
- Inactivity timeout processing.
- Active-run display refresh.
- Countdown processing.
- Countdown-expiration processing.


### 4.4 HeatTask

`HeatTask` owns heater-control operation.

Its responsibilities include:

- Receiving the latest cabinet temperature.
- Receiving run, stop, and setpoint commands from `DisplayTask`.
- Determining whether the heater should be commanded on or off.
- Driving the heater-control output.
- Driving the Heater indicator consistently with the heater command.
- Reporting heater status to `DisplayTask`.
- Removing the heater command when heating is not permitted.

`HeatTask` does not own the front-panel state machine.


### 4.5 ConnectTask

`ConnectTask` is reserved for future external connectivity.

The intended architecture is for the STM32 to remain responsible for
real-time cabinet control while an external connectivity module provides
network communications.

The connectivity protocol and detailed `ConnectTask` architecture are TBD.

Connectivity is not required for the current proof-of-concept firmware.


## 5. Inter-Task Communication

FreeRTOS message queues are used to pass information between application
tasks.


### 5.1 qInputToDisplay

Producer:

`InputTask`

Consumer:

`DisplayTask`

Purpose:

Transfers discrete button events to the user-interface state machine.

Current queue depth:

8

Semantics:

FIFO.

Button events are discrete events and must be processed in order. They are not
treated as "latest value wins" data.


### 5.2 qSenseToHeat

Producer:

`SenseTask`

Consumer:

`HeatTask`

Purpose:

Transfers the latest cabinet temperature measurement to the heater-control
task.

Current queue depth:

1

Semantics:

Latest value wins.

Temperature is continuously updated state. Processing an old backlog of
temperature measurements would be undesirable for heater control.

Because CMSIS-RTOS2 does not provide queue-overwrite behavior equivalent to
the required semantics, the producer uses a drain-then-put operation when
necessary so that the queue contains the newest temperature value.


### 5.3 qDisplayToHeat

Producer:

`DisplayTask`

Consumer:

`HeatTask`

Purpose:

Transfers heater-control commands from the application state machine to the
heater-control task.

Current queue depth:

4

Semantics:

FIFO.

Commands must be processed in order and therefore do not use latest-value-wins
semantics.

The command data structure is represented by `HeatCommand_t`.


### 5.4 qHeatToDisplay

Producer:

`HeatTask`

Consumer:

`DisplayTask`

Purpose:

Transfers current heater status to the display subsystem.

Current queue depth:

1

Semantics:

Latest value wins.

The status data structure is represented by `HeatStatus_t`.

As with temperature data, a newer heater status supersedes an unread older
status.


### 5.5 qUartRxToConnect

This queue is reserved for future connectivity work.

Its final payload, depth, and data-handling semantics shall be reviewed when
the connectivity protocol is designed.

UART byte streams shall not automatically use latest-value-wins semantics
because discarded bytes could corrupt a communications message.


## 6. User-Interface State Machine

`DisplayTask` owns the application state machine.

The current primary state flow is:

    Idle-Splash
         |
         v
    Idle-Prompt
         |
         v
    SetTemp-Decision
         |
         v
    SetTemp-Adjust <--> SetTemp-Confirm
         |
         v
    SetTime-Decision
         |
         v
    SetTime-Adjust <--> SetTime-Confirm
         |
         v
    Run-Decision
         |
         v
    Run-Active
         |
         v
    Complete-Decision

Additional completion and Settings states are entered as required.

The state machine distinguishes between:

- Configuring a new proofing run.
- Editing an active proofing run.

This distinction is important because an active proof continues operating
using its previously confirmed settings while proposed changes are being
edited.


## 7. Proposed and Confirmed Values

The firmware architecture separates proposed user settings from confirmed
active settings.

During configuration or active-run editing, Up and Down modify proposed
values.

The proposed values do not immediately modify an active proofing run.

When the user reaches `Run-Decision` and confirms the changes, the proposed
values become the confirmed values.

For an active-run edit:

- The existing temperature setpoint remains active during editing.
- The existing countdown continues during editing.
- Proposed temperature changes do not affect heater control.
- Proposed timer changes do not affect the current countdown.
- Confirming the edit applies the proposed values.
- Cancelling the run from `Run-Decision` stops the active run.

If a changed timer duration is confirmed, a new countdown begins using the
newly confirmed duration.

Elapsed time from the previous countdown is not applied to the new duration.

If the timer duration was not changed, the existing countdown continues
without restarting.


## 8. Countdown Architecture

The proofing countdown is managed by `DisplayTask`.

The countdown is based on elapsed RTOS time rather than relying on the LCD
update rate.

This allows the countdown to continue independently of which UI screen is
currently displayed.

During an active-run edit, the countdown continues in the background.

If the countdown reaches 0:00 while the user is editing the run:

1. The active proof takes precedence over the unconfirmed edit.
2. Proposed changes are discarded.
3. Heating is stopped.
4. The state machine enters the completion sequence.

This prevents an edit screen from delaying completion of a timed proof.


## 9. Temperature-Sensing Architecture

Cabinet temperature is measured using an NTC thermistor connected to an STM32
ADC input.

The current divider topology places:

- A fixed resistor between the supply and the ADC sense node.
- The NTC thermistor between the ADC sense node and ground.

With this topology, increasing ADC voltage corresponds to increasing
thermistor resistance and therefore decreasing temperature.

The firmware converts the ADC measurement to thermistor resistance and then
to temperature using the thermistor Beta equation.

Temperature is converted into an application representation suitable for
inter-task communication and heater control.

The final production calibration method is TBD.


## 10. Heater-Control Architecture

The proof-of-concept uses on/off temperature control rather than PID control.

This is commonly referred to as bang-bang control with hysteresis.

The control concept is:

    Cabinet temperature
            |
            v
    +-------------------+
    | Temperature       |
    | comparison        |
    +---------+---------+
              |
              v
    +-------------------+
    | Heater ON / OFF   |
    | decision          |
    +---------+---------+
              |
              v
    +-------------------+
    | Heater command    |
    +-------------------+

Hysteresis is used to prevent rapid heater switching near the temperature
setpoint.

The architecture intentionally does not use PID control for the current
proof-of-concept.

The exact heater-on and heater-off thresholds are TBD and will be established
during hot-cabinet testing.

Those thresholds shall be selected so that the implemented control behavior
satisfies the temperature-regulation requirement in `REQUIREMENTS.md`.

The independent hardware overtemperature protection is external to this
firmware-control architecture and is not implemented by `HeatTask`.


## 11. Heater Command Architecture

`DisplayTask` determines whether the application permits an active proofing
run.

`HeatTask` determines the heater command based on:

- Whether heating is enabled for the current run.
- The confirmed temperature setpoint.
- The current measured cabinet temperature.
- Any firmware condition that prohibits heating.

The heater command defaults to off.

Heating is enabled only after a proofing run has been confirmed.

When a stop condition occurs, `DisplayTask` commands the run to stop and
`HeatTask` removes the heater command.

The Heater indicator follows the firmware heater command.

The Heater indicator therefore indicates that firmware is requesting heat. It
does not independently verify heater current, relay contact operation, or
actual heater operation.


## 12. Completion Architecture

Completion can be entered by either:

- A user request during an active proof.
- Expiration of a timed proofing countdown.

The completion path retains information about why completion was entered.

This allows `Complete-Decision` to behave differently depending on whether it
was entered manually or because the countdown expired.

For a manually requested completion, cancelling completion returns to the
active run.

For timer-driven completion, heating has already stopped and the completion
path does not resume the expired run.

Entry into `Complete-Decision` also initiates the completion audible alert.


## 13. Audible Alert Architecture

The buzzer is controlled by firmware.

The completion alert is generated when the state machine enters
`Complete-Decision`, rather than continuously while that screen is refreshed.

This prevents periodic display processing from repeatedly retriggering the
alert.

The current completion pattern is:

    Beep 1 -> silence -> Beep 2 -> silence -> Beep 3

The required beep and silence durations are defined in `REQUIREMENTS.md`.


## 14. Persistence Architecture

The application distinguishes between persistent configuration and active-run
state.

Persistent configuration includes:

- Confirmed temperature setpoint.
- Confirmed temperature units.
- Confirmed timer duration.

Active-run state is not persistent.

A power interruption or reset therefore does not automatically restore or
resume a proofing run.

Persistence occurs at defined confirmation points rather than on every
Up or Down button press.

This avoids unnecessary nonvolatile-memory writes while the user is merely
adjusting a proposed value.

The final nonvolatile-storage implementation shall satisfy the persistence
requirements in `REQUIREMENTS.md`.

The detailed persistence implementation is TBD until this portion of the
firmware is finalized.


## 15. Fault-Handling Architecture

Fault detection is divided between faults that can be detected directly from
electrical measurements and faults that require observation of system
behavior over time.

### 15.1 NTC Fault Detection

The temperature-sensing subsystem detects open-circuit and short-circuit NTC
faults using the ADC measurement.

The NTC voltage divider is arranged so that:

- An NTC short circuit produces an ADC value near 0.
- An NTC open circuit produces an ADC value near the ADC full-scale value.
- A normally operating NTC produces an ADC value between these two extremes.

For the STM32L476RG 12-bit ADC, the nominal endpoints are approximately:

- NTC short: ADC = 0.
- NTC open: ADC = 4095.

Practical fault-detection thresholds near these endpoints shall be established
during hardware testing.

The assigned NTC fault codes are:

- Error 10: NTC open.
- Error 11: NTC short.

When an NTC fault is detected, the heater command is disabled and the
application enters the Error state.


### 15.2 Heater Fault Detection

Heater fault detection requires evaluation of the thermal response after the
heater has been commanded on.

The intended approach is to determine whether the cabinet temperature responds
as expected after the heater has been commanded on for a defined period.

The following detection parameters are TBD:

- Required heater-on observation time.
- Required temperature increase or other acceptable thermal response.
- Conditions under which the test is considered valid.
- Heater fault classification criteria.

These parameters require testing with near-production hardware because heater
power, cabinet thermal characteristics, NTC placement, ambient temperature,
and thermal mass affect the expected temperature response.

The assigned heater fault codes are:

- Error 20: Heater open.
- Error 21: Heater short.

The heater-fault detection algorithm shall remain TBD until near-production
hardware testing provides sufficient data to define reliable detection
criteria.


### 15.3 Fault Response

When a firmware-detected fault requires heating to stop, the heater command is
disabled and the application enters the Error state.

The Error state prevents normal user-interface commands from resuming
operation.

The detailed recovery behavior from a firmware-detected fault remains TBD.

## 16. Display Architecture

`DisplayTask` owns LCD rendering.

The display driver is responsible for low-level communication with the LCD.

Application code determines what information is displayed based on the
current state.

This separation prevents low-level LCD driver code from owning application
state or proofing behavior.

Dynamic screens are periodically refreshed by `DisplayTask`.

Screen transitions that occur because of elapsed time are also processed by
`DisplayTask` during its periodic execution.


## 17. Settings Architecture

Settings entry is initiated by an event generated by `InputTask`.

`DisplayTask` determines whether the event is valid for the current
application state.

This keeps physical button detection separate from application-state
decisions.

The current Settings functionality is limited primarily to temperature-unit
selection.

Additional Settings functionality is TBD.


## 18. FreeRTOS Scheduling

The current application task priorities are:

| Task | Priority |
|---|---|
| `HeatTask` | `osPriorityHigh3` |
| `ConnectTask` | `osPriorityHigh` |
| `InputTask` | `osPriorityAboveNormal` |
| `SenseTask` | `osPriorityNormal` |
| `DisplayTask` | `osPriorityLow` |

Current task stack allocations are 128 words per application task.

These values are prototype configuration values and may be changed as stack
usage and system behavior are measured.

The current FreeRTOS heap allocation is 8192 bytes.

RTOS configuration values remain controlled by the STM32CubeMX project.


## 19. Hardware Configuration Ownership

MCU hardware configuration is maintained in the STM32CubeMX project.

This includes items such as:

- GPIO assignments.
- GPIO electrical configuration.
- ADC configuration.
- Timer configuration.
- UART configuration.
- I2C configuration.
- FreeRTOS-generated objects.
- Peripheral instances.

`ARCHITECTURE.md` may describe why a peripheral or interface is used, but it
does not duplicate the authoritative MCU pin-assignment table.

This avoids maintaining the same hardware configuration in multiple places.


## 20. Connectivity Architecture

Connectivity is outside the current proof-of-concept firmware scope.

The planned architecture reserves `ConnectTask` for communications between the
STM32 and an external connectivity module.

The STM32 will remain responsible for local real-time cabinet control.

The external module is expected to handle network connectivity rather than
moving temperature-control responsibility away from the STM32.

The following connectivity architecture remains TBD:

- Physical communications protocol details.
- Application message protocol.
- Command structure.
- Remote monitoring.
- Remote control.
- Authentication and security.
- Communications fault handling.
- Offline behavior.

These items shall be defined before connectivity functionality is implemented.


## 21. Open Architecture Items

The following architecture items remain intentionally unresolved:

- Final heater hysteresis thresholds.
- Final NTC calibration method.
- NTC open-circuit detection threshold.
- NTC short-circuit detection threshold.
- Final persistence implementation.
- Final Settings-mode architecture.
- Connectivity protocol and implementation.
- Heater fault-detection parameters and algorithm.

These items are left TBD rather than defining implementation decisions before
the prototype provides enough information to make those decisions.
