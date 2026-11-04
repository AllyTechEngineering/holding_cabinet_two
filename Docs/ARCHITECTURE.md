# Firmware Architecture

## 1. Purpose

This document defines the firmware architecture for the Holding Cabinet /
Proofing Oven.

`REQUIREMENTS.md` defines required behavior.

`HMI.md` defines the front-panel HMI.

STM32 peripheral configuration, GPIO assignments, and MCU pin assignments are
maintained by the STM32CubeMX project.


## 2. Platform

The current firmware targets:

- NUCLEO-L476RG
- STM32L476RG
- FreeRTOS
- CMSIS-RTOS2
- STM32 HAL
- STM32CubeMX


## 3. Application Tasks

The application uses five FreeRTOS tasks:

- `SenseTask`
- `InputTask`
- `DisplayTask`
- `HeatTask`
- `ConnectTask`


## 4. Task Responsibilities

### 4.1 SenseTask

`SenseTask`:

- Reads the NTC ADC input.
- Converts the ADC measurement to cabinet temperature.
- Detects NTC electrical faults.
- Provides temperature information to `HeatTask`.

The current sensing period is 2500 ms.


### 4.2 InputTask

`InputTask`:

- Samples the front-panel buttons.
- Debounces button input.
- Detects press and release events.
- Detects required hold events.
- Detects the Settings-entry button combination.
- Reports button events to `DisplayTask`.

`DisplayTask` determines whether an event is valid for the current HMI state.


### 4.3 DisplayTask

`DisplayTask` owns the application HMI state machine.

It manages:

- HMI states and transitions.
- Proposed and confirmed temperature values.
- Proposed and confirmed timer values.
- Timed and untimed run state.
- Proofing countdown.
- Active-run editing.
- HMI inactivity timing.
- LCD rendering.
- Run-control commands to `HeatTask`.

`DisplayTask` wakes approximately every 100 ms.


### 4.4 HeatTask

`HeatTask` owns heater-control operation.

It:

- Receives cabinet temperature from `SenseTask`.
- Receives run, stop, and setpoint commands from `DisplayTask`.
- Determines the heater command.
- Drives the heater output.
- Drives the Heater indicator.
- Reports heater status to `DisplayTask`.

Local heater control does not depend on connectivity.


### 4.5 ConnectTask

`ConnectTask` owns STM32-side connectivity operation.

The current implementation initializes the ESP32-C6 UART transport and starts
interrupt-driven UART reception.

ESP-AT command processing and higher-level connectivity behavior remain to be
implemented.


## 5. Inter-Task Communication

### 5.1 qInputToDisplay

Producer: `InputTask`

Consumer: `DisplayTask`

Depth: 8

Payload: `uint8_t` / `ButtonEvent_t`

Semantics: FIFO


### 5.2 qSenseToHeat

Producer: `SenseTask`

Consumer: `HeatTask`

Depth: 1

Payload: `uint16_t`

Semantics: latest value wins

The producer uses drain-then-put behavior.


### 5.3 qDisplayToHeat

Producer: `DisplayTask`

Consumer: `HeatTask`

Depth: 4

Payload: `HeatCommand_t`

Semantics: FIFO


### 5.4 qHeatToDisplay

Producer: `HeatTask`

Consumer: `DisplayTask`

Depth: 1

Payload: `HeatStatus_t`

Semantics: latest value wins


### 5.5 qUartRxToConnect

Producer: UART receive callback

Consumer: `ConnectTask`

Depth: 128

Payload: `uint8_t`

Semantics: FIFO

Received UART bytes shall not use latest-value-wins behavior.


### 5.6 Application / Connectivity Interface

The application-level interface between `ConnectTask` and the rest of the
application is TBD.


## 6. HMI Architecture

`DisplayTask` owns the HMI state machine.

Exact states, transitions, controls, screen content, and display timing are
defined in `HMI.md`.


## 7. Run Timer

`run_timer.c/.h` manages active timed and untimed run timing.

Timed runs use the RTOS tick count and a configured duration.

Remaining time is calculated from elapsed time rather than by decrementing a
stored minute counter.

Countdown expiration is processed independently of the currently displayed
setup or run-edit screen.


## 8. Time Editor

`time_editor.c/.h` manages proposed countdown duration and Up/Down hold
acceleration.

It does not own the active run timer.


## 9. Temperature Sensing

Cabinet temperature is measured using an NTC thermistor connected to an STM32
ADC input.

The current divider uses:

- 10 kΩ fixed resistor from 3.3 V to the ADC node.
- NTC thermistor from the ADC node to ground.

The current NTC is nominally:

- 10 kΩ at 25°C
- Beta approximately 3950 K

Temperature conversion uses the thermistor Beta equation.


### 9.1 Internal Temperature Representation

Application temperature values are represented internally as whole degrees
Celsius.

`DisplayTask` converts between internal Celsius and the selected HMI unit.

Conversions are rounded to the nearest whole degree.


## 10. Heater Control

The firmware uses on/off temperature control with hysteresis.

The confirmed setpoint is the upper control limit.

The lower control limit is:

    lower_control_limit = setpoint_c - hysteresis_c

The initial hysteresis is 2°C.

Control behavior:

- Temperature at or above the upper limit: heater off.
- Temperature at or below the lower limit: heater on.
- Between the limits: retain the existing heater command.

A stop condition or detected NTC fault overrides normal temperature control
and commands the heater off.

The current heater-control logic is implemented in `control_task.c`.

`heater_control.c/.h` are currently placeholders.


## 11. Heater Output

The current development relay interface is active-low.

The Heater indicator is active-high and follows the firmware heater command.

The Heater indicator does not verify heater current or actual heater
operation.

Independent hardware overtemperature protection is external to the firmware
control architecture.


## 12. Completion

Completion may result from:

- User-requested completion.
- Timed countdown expiration.

The application retains the completion reason so the HMI can apply the
behavior defined in `HMI.md`.


## 13. Persistence

Persistent proofing configuration is required for:

- Confirmed temperature setpoint.
- Temperature unit.
- Confirmed timer duration.

Wi-Fi configuration shall also persist across power cycles.

Active-run state is not persistent.

`settings_store.c/.h` currently exist as placeholders for STM32-owned
persistent proofing configuration.

The final STM32 nonvolatile-storage implementation is TBD.

Accepted Wi-Fi credentials are stored persistently by the ESP32-C6 using
ESP-AT persistent storage.

## 14. Fault Handling

### 14.1 NTC Faults

NTC electrical fault detection is performed before temperature conversion.

The current prototype detection uses ADC endpoint thresholds.

Final production thresholds are TBD.

Assigned codes:

- Error 10: NTC open.
- Error 11: NTC short.

The current firmware also identifies an ADC read failure internally.

An NTC fault disables heating.


### 14.2 Heater Faults

Assigned codes:

- Error 20: Heater open.
- Error 21: Heater short.

Detection criteria are TBD.


### 14.3 Connectivity Conditions

Loss of ESP32, Wi-Fi, Internet, mobile-app, or cloud connectivity does not
disable local proofing operation.

Connectivity failures are not cabinet safety faults.


## 15. Settings

Settings entry is detected by `InputTask`.

`DisplayTask` owns Settings HMI behavior.

Temperature-unit selection and Wi-Fi configuration are defined as Settings
functions.

The final Wi-Fi Settings HMI is TBD.


## 16. FreeRTOS Configuration

The STM32CubeMX project currently defines:

| Task | Priority | Stack |
|---|---|---:|
| `HeatTask` | `osPriorityHigh3` | 128 words |
| `ConnectTask` | `osPriorityHigh` | 128 words |
| `InputTask` | `osPriorityAboveNormal` | 128 words |
| `SenseTask` | `osPriorityNormal` | 128 words |
| `DisplayTask` | `osPriorityLow` | 256 words |

The FreeRTOS heap is 8192 bytes.

STM32CubeMX is authoritative for generated RTOS configuration.

## 17. Connectivity Architecture

The STM32 controls cabinet operation. The ESP32-C6 provides communications services to the STM32.

See [Connectivity Subsystem Architecture](Connectivity/CONNECTIVITY_ARCHITECTURE.md) for the subsystem design and interfaces.

## 18. Hardware Configuration Ownership

The STM32CubeMX project is authoritative for MCU peripheral, GPIO, pin, and
generated RTOS configuration.

Detailed electrical and production hardware design is maintained in:

`AllyTechEngineering/holding-cabinet-hardware`


## 19. Open Architecture Items

- Nonvolatile-storage implementation.
- Wi-Fi Settings HMI.
- Application / `ConnectTask` interface.
- ESP-AT command/response layer.
- BLE provisioning mechanism.
- Wi-Fi configuration replacement behavior.
- Cabinet serial-number format and storage.
- Firebase data model and remote command interface.
- Heater fault-detection criteria.
- Production NTC fault thresholds.
