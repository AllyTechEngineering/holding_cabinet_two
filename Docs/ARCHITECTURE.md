# Firmware Architecture

## 1. Purpose

This document defines the firmware architecture for the Holding Cabinet /
Proofing Oven.

`REQUIREMENTS.md` defines what the firmware is required to do.

`HMI.md` defines the front-panel human-machine interface, including HMI
states, state transitions, user interactions, LCD content, screen sequencing,
and display timing.

This document defines how the firmware is organized to satisfy those
requirements and implement the HMI.

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

`DisplayTask` also receives heater status from `HeatTask` and owns LCD/HMI
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


### 4.2 InputTask

`InputTask` is responsible for physical front-panel button acquisition.

The physical front-panel switches are active-low. A GPIO low level represents
a pressed switch.

`InputTask` performs the low-level processing necessary to convert physical
button activity into discrete application button events.

This includes:

- Button sampling.
- Button debouncing.
- Press and release detection.
- Button-hold processing where required.
- Detection of multi-button input combinations required by the HMI.

`InputTask` reports button events to `DisplayTask`.

`InputTask` does not determine whether a button event is valid for the current
HMI state. That decision belongs to `DisplayTask`, which owns the HMI state
machine.

The required controls and their HMI behavior are defined in `HMI.md`.


### 4.3 DisplayTask

`DisplayTask` owns the front-panel application HMI state machine.

Its responsibilities include:

- Processing button events received from `InputTask`.
- Managing HMI states and state transitions.
- Managing proposed and confirmed temperature settings.
- Managing proposed and confirmed timer settings.
- Managing timed and untimed run state.
- Managing the proofing countdown.
- Managing active-run editing.
- Managing HMI inactivity timing.
- Rendering information to the LCD.
- Sending confirmed run-control information to `HeatTask`.
- Receiving heater status for display purposes.
- Initiating persistence of confirmed settings when required.

`DisplayTask` operates using a periodic wake rather than waiting indefinitely
for a button event.

The current periodic wake interval is approximately 100 ms.

This allows `DisplayTask` to perform time-dependent application processing
without requiring a button event, including:

- HMI timing.
- Inactivity timeout processing.
- Dynamic display refresh.
- Countdown processing.
- Countdown-expiration processing.

The required HMI behavior is defined in `HMI.md`.


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

`HeatTask` does not own the front-panel HMI state machine.


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

Transfers discrete button events to the HMI state machine.

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

Its final payload, depth, and data-handling semantics are TBD.

UART byte streams will not use latest-value-wins semantics because discarded
bytes could corrupt a communications message.


## 6. User-Interface State Machine

`DisplayTask` owns the application HMI state machine.

The HMI state definitions, state transitions, user interactions, screen
sequencing, and display timing are defined in `HMI.md`.

The firmware architecture distinguishes between:

- Configuring a new proofing run.
- Editing an active proofing run.

During active-run editing, the proof continues operating using its previously
confirmed settings until proposed changes are confirmed.


## 7. Proposed and Confirmed Values

The firmware architecture separates proposed user settings from confirmed
active settings.

During configuration or active-run editing, user input modifies proposed
values.

The proposed values do not immediately modify an active proofing run.

When the user confirms the run configuration, the proposed values become the
confirmed values.

For an active-run edit:

- The existing temperature setpoint remains active during editing.
- The existing countdown continues during editing.
- Proposed temperature changes do not affect heater control.
- Proposed timer changes do not affect the current countdown.
- Confirming the edit applies the proposed values.

The exact user interaction used to propose, confirm, or cancel changes is
defined in `HMI.md`.


## 8. Countdown Architecture

The proofing countdown is managed by `DisplayTask`.

The countdown is based on elapsed RTOS time rather than relying on the LCD
update rate.

This allows the countdown to continue independently of which HMI state is
currently displayed.

During an active-run edit, the countdown continues in the background.

Countdown expiration is processed independently of HMI display activity.

The required behavior resulting from countdown expiration is defined in
`REQUIREMENTS.md`, with the associated HMI transition defined in `HMI.md`.


## 9. Temperature-Sensing Architecture

Cabinet temperature is measured using an NTC thermistor connected to an STM32
ADC input.

The current divider topology places:

- A fixed 10 kΩ resistor between 3.3 V and the ADC sense node.
- The NTC thermistor between the ADC sense node and ground.

With this topology, increasing ADC voltage corresponds to increasing
thermistor resistance and therefore decreasing temperature.

The current NTC is an MF52B-type thermistor with a nominal resistance of
10 kΩ at 25°C and a Beta value of approximately 3950 K.

The firmware converts the ADC measurement to thermistor resistance and then
to temperature using the thermistor Beta equation.

The final sensor calibration shall be conducted after PCBA final assembly using an ATE production fixture.
No firmware shall be used for the calibration.

### 9.1 Internal Temperature Representation

Celsius is the firmware's internal temperature representation.

Application-level temperature measurements, confirmed temperature setpoints,
and heater-control temperatures are represented as whole degrees Celsius.

`DisplayTask` is responsible for converting between the internal Celsius
representation and the temperature unit selected by the user.

Fahrenheit and Celsius selection affects the HMI representation of temperature
but does not change the internal Celsius representation used by the firmware.

Temperature conversions between the selected HMI unit and the internal Celsius representation are rounded to the nearest whole degree. Conversion rounding of up to 0.5°C (0.9°F) is acceptable for this product and does not constitute a temperature-control error.

## 10. Heater-Control Architecture

The proof-of-concept uses on/off temperature control with hysteresis.

Heater control operates using the internal whole-degree Celsius temperature
representation regardless of the temperature unit selected for the HMI.

The confirmed temperature setpoint is the upper control limit.

The lower control limit is calculated as:

    lower_control_limit = setpoint_c - hysteresis_c

The initial hysteresis is 2°C.

The heater-control decision is:

- If the measured temperature is greater than or equal to the upper control
  limit, command the heater off.
- If the measured temperature is less than or equal to the lower control
  limit, command the heater on.
- If the measured temperature is between the lower and upper control limits,
  retain the existing heater command state.

The configured hysteresis is an initial control value and may be adjusted
during thermal testing with representative hardware. Actual cabinet
temperature overshoot and undershoot will be characterized during that
testing.

A stop condition, inactive run, or firmware-detected condition that prohibits
heating overrides the temperature-control decision and commands the heater
off.

Independent hardware overtemperature protection is external to the firmware
control architecture.


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

The Heater indicator indicates that firmware is requesting heat. It does not
independently verify heater current or actual heater operation.


## 12. Completion Architecture

Completion can be initiated by either:

- A user request during an active proof.
- Expiration of a timed proofing countdown.

The application retains the reason that completion was initiated so that
`DisplayTask` can apply the appropriate completion behavior.

The exact completion-state transitions and user interactions are defined in
`HMI.md`.


## 13. Audible Alert Architecture

The buzzer is controlled by firmware.

Completion-alert generation is initiated by the HMI state machine.

The required audible-alert behavior and timing are defined in
`REQUIREMENTS.md` and the associated operator interaction is defined in
`HMI.md`.


## 14. Persistence Architecture

The application distinguishes between persistent configuration and active-run
state.

Persistent configuration includes:

- Confirmed temperature setpoint.
- Confirmed temperature units.
- Confirmed timer duration.

Active-run state is not persistent.

A power interruption or reset does not automatically restore or resume a
proofing run.

Persistence occurs at defined confirmation points rather than on every user
adjustment.

The final nonvolatile-storage implementation is TBD.


## 15. Fault-Handling Architecture

Fault detection is divided between faults that can be detected directly from
electrical measurements and faults that require observation of system
behavior over time.


### 15.1 NTC Fault Detection

The temperature-sensing subsystem detects open-circuit and short-circuit NTC
faults using the ADC measurement.

The NTC voltage divider is arranged so that:

0–5       NTC SHORT
6–4089    Valid ADC measurement
4090–4095 NTC OPEN

Refer to test_plan.md TBD section for production line ATE testing procedures.

Stop nagging about this until the final system is in beta testing!

The assigned NTC fault codes are:

- Error 10: NTC open.
- Error 11: NTC short.

When an NTC fault is detected, the heater command is disabled and the
application enters the Error state.

### 15.1.1 Sensor Validity and Temperature Setpoint Limits

The user-selectable proofing-temperature range is not a sensor-validity range.

The minimum and maximum proofing-temperature setpoints are defined in
`REQUIREMENTS.md`. These limits restrict the temperature that the user may
select for a proofing run. They shall not be used to determine whether a
measured cabinet temperature is valid.

A valid measured cabinet temperature may be below the minimum selectable
setpoint or above the maximum selectable setpoint.

NTC sensor validity is determined from the electrical ADC measurement before
temperature conversion.

For the STM32L476RG 12-bit ADC:

- An ADC measurement at or near 0 indicates an NTC short circuit.
- An ADC measurement at or near full scale (4095) indicates an NTC open circuit.
- ADC measurements between the defined open- and short-circuit thresholds are
  treated as valid sensor measurements and are converted to temperature using
  the NTC model.

Practical open- and short-circuit detection thresholds may be placed slightly
inside the ADC endpoints to provide tolerance for ADC measurement variation and
the physical sensor circuit.

Once a measurement has passed the NTC electrical fault checks, the calculated
cabinet temperature is considered a valid temperature measurement. Application
code shall not reject that temperature solely because it is outside the
user-selectable proofing-temperature range.

Sensor fault detection and user-selectable temperature limits therefore serve
separate purposes:

- NTC fault thresholds determine whether the temperature sensor measurement is
  electrically valid.
- Proofing-temperature limits determine what temperature setpoint the user is
  permitted to select.
  
### 15.2 Heater Fault Detection

Heater fault detection requires evaluation of the thermal response after the
heater has been commanded on.

The intended approach is to determine whether cabinet temperature responds as
expected after the heater has been commanded on for a defined period.

The following detection parameters are TBD:

- Required heater-on observation time.
- Required temperature increase or other acceptable thermal response.
- Conditions under which the test is considered valid.
- Heater fault classification criteria.

These parameters require testing with near-production hardware.

The assigned heater fault codes are:

- Error 20: Heater open.
- Error 21: Heater short.


### 15.3 Fault Response

When a firmware-detected fault requires heating to stop, the heater command is
disabled and the application enters the Error state.

The Error state prevents normal user-interface commands from resuming
operation.

Detailed recovery behavior is TBD.

The Error-state operator interface is defined in `HMI.md`.


## 16. Display Architecture

`DisplayTask` owns LCD rendering.

The display driver is responsible for low-level communication with the LCD.

`DisplayTask` renders the LCD according to the current HMI state and associated
application data.

Dynamic screens are periodically refreshed by `DisplayTask`.

The exact LCD content, character placement, display sequencing, and
HMI-controlled display timing are defined in `HMI.md`.


## 17. Settings Architecture

Settings entry events are generated by `InputTask`.

`DisplayTask` determines whether a Settings-entry event is valid for the
current application state.

Settings values are managed as proposed values until confirmed.

The current Settings functionality is primarily temperature-unit selection.

The Settings user interaction and display behavior are defined in `HMI.md`.

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

The current FreeRTOS heap allocation is 8192 bytes.

These are prototype configuration values and may change based on measured
system resource usage.

RTOS configuration values are controlled by the STM32CubeMX project.


## 19. Hardware Configuration Ownership

MCU hardware configuration is maintained in the STM32CubeMX project.

This includes:

- GPIO assignments and electrical configuration.
- ADC configuration.
- Timer configuration.
- UART configuration.
- I2C configuration.
- FreeRTOS-generated objects.
- Peripheral instances.

Detailed hardware design is maintained in the separate
`AllyTechEngineering/holding-cabinet-hardware` KiCad repository.

This document may describe firmware-relevant hardware interfaces but does not
duplicate authoritative MCU configuration or hardware design information.


## 20. Connectivity Architecture

Connectivity is outside the current proof-of-concept firmware scope.

The planned architecture reserves `ConnectTask` for communications between the
STM32 and an external connectivity module.

The STM32 remains responsible for local real-time cabinet control.

The external module handles network connectivity.

The connectivity architecture is TBD.


## 21. Open Architecture Items

The following architecture items remain unresolved:

- Final persistence implementation.
- Final Settings-mode architecture.
- Connectivity protocol and implementation.
- Heater fault-detection parameters and algorithm.