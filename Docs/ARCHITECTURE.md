# Firmware Architecture

## 1. Purpose

This document defines the firmware architecture for the Holding Cabinet /
Proofing Oven.

`REQUIREMENTS.md` defines required behavior.

`HMI.md` defines the front-panel HMI.

This document defines how the firmware is organized to satisfy those
requirements.

STM32 peripheral configuration and MCU pin assignments are maintained by the
STM32CubeMX project.


## 2. Platform

The current firmware targets:

- NUCLEO-L476RG
- STM32L476RG
- FreeRTOS
- CMSIS-RTOS2
- STM32 HAL
- STM32CubeMX


## 3. Application Tasks

The application uses:

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


### 4.2 InputTask

`InputTask`:

- Samples the front-panel buttons.
- Debounces button input.
- Detects press, release, and required hold events.
- Detects the Settings-entry button combination.
- Reports button events to `DisplayTask`.

`DisplayTask` determines whether an input is valid for the current HMI state.


### 4.3 DisplayTask

`DisplayTask` owns the front-panel HMI state machine.

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
- Persistent-setting save requests.
- Cabinet-side connectivity HMI interaction.

`DisplayTask` currently wakes approximately every 100 ms.


### 4.4 HeatTask

`HeatTask` owns heater control.

It:

- Receives cabinet temperature.
- Receives run, stop, and setpoint commands.
- Determines the heater command.
- Drives the heater output.
- Drives the Heater indicator.
- Reports heater status to `DisplayTask`.

Connectivity state does not control whether local proofing may operate.


### 4.5 ConnectTask

`ConnectTask` owns STM32-side connectivity operation.

It manages communications with the ESP32-C6 and reports connectivity status
to the application.

Connectivity failure does not stop local proofing operation.


## 5. Inter-Task Communication

### 5.1 qInputToDisplay

Producer: `InputTask`

Consumer: `DisplayTask`

Depth: 8

Semantics: FIFO

Transfers discrete button events.


### 5.2 qSenseToHeat

Producer: `SenseTask`

Consumer: `HeatTask`

Depth: 1

Semantics: latest value wins

The producer uses drain-then-put behavior so the queue contains the newest
temperature value.


### 5.3 qDisplayToHeat

Producer: `DisplayTask`

Consumer: `HeatTask`

Depth: 4

Semantics: FIFO

Transfers `HeatCommand_t` commands.


### 5.4 qHeatToDisplay

Producer: `HeatTask`

Consumer: `DisplayTask`

Depth: 1

Semantics: latest value wins

Transfers `HeatStatus_t`.


### 5.5 qUartRxToConnect

Consumer: `ConnectTask`

Purpose: ESP32 UART receive data.

UART data shall not use latest-value-wins semantics.

The final queue configuration may be revised during connectivity development.


### 5.6 Connectivity Application Interface

Communication between `DisplayTask` and `ConnectTask` is TBD.


## 6. HMI State Machine

`DisplayTask` owns the HMI state machine.

Exact HMI states, transitions, controls, screen content, and display timing
are defined in `HMI.md`.


## 7. Proposed and Confirmed Values

Configuration changes are maintained as proposed values until confirmed.

During active-run editing:

- The confirmed temperature remains active.
- The existing countdown continues.
- Proposed changes do not affect the active run.
- Confirming the edit applies the proposed values.


## 8. Countdown

`DisplayTask` manages the proofing countdown using elapsed RTOS time.

The countdown continues during active-run editing.

Countdown expiration is processed independently of the displayed HMI screen.


## 9. Temperature Sensing

The cabinet temperature is measured using an NTC thermistor connected to an
STM32 ADC input.

The current divider uses:

- 10 kΩ fixed resistor from 3.3 V to the ADC node.
- NTC thermistor from the ADC node to ground.

The current NTC is nominally:

- 10 kΩ at 25°C
- Beta approximately 3950 K

Temperature conversion uses the thermistor Beta equation.


### 9.1 Internal Temperature Representation

Celsius is the internal temperature representation.

Application temperature, setpoint, and heater-control values use whole degrees
Celsius.

`DisplayTask` converts between internal Celsius and the selected display unit.

Conversions are rounded to the nearest whole degree.


## 10. Heater Control

The firmware uses on/off control with hysteresis.

The confirmed setpoint is the upper control limit.

The lower control limit is:

    lower_control_limit = setpoint_c - hysteresis_c

The initial hysteresis is 2°C.

Control behavior:

- Temperature >= upper limit: heater off.
- Temperature <= lower limit: heater on.
- Between limits: retain the existing heater command.

The hysteresis may be adjusted after thermal testing.

A stop condition or firmware fault requiring shutdown overrides temperature
control and commands the heater off.


## 11. Completion

Completion may result from:

- User-requested completion.
- Timed countdown expiration.

`DisplayTask` retains the completion reason so the HMI can apply the behavior
defined in `HMI.md`.


## 12. Persistence

Persistent proofing configuration includes:

- Temperature setpoint.
- Temperature unit.
- Timer duration.

Persistent connectivity configuration includes the Wi-Fi information required
for reconnection.

The STM32 is the authoritative persistent store for cabinet configuration.

Active-run state is not persistent.

The final nonvolatile-storage implementation is TBD.


## 13. Fault Handling

### 13.1 NTC Faults

NTC open- and short-circuit conditions are detected from the ADC measurement.

Final production thresholds are TBD.

Assigned codes:

- Error 10: NTC open.
- Error 11: NTC short.

An NTC fault disables heating and enters the Error state.


### 13.2 Heater Faults

Assigned codes:

- Error 20: Heater open.
- Error 21: Heater short.

Detection criteria are TBD.


### 13.3 Connectivity Conditions

Connectivity failures are not safety faults and do not enter the cabinet
Error state.

Connectivity status is managed by `ConnectTask`.


## 14. Settings

`DisplayTask` owns Settings HMI behavior.

Temperature-unit selection is currently implemented.

Wi-Fi configuration will be initiated through Settings.

Exact Wi-Fi Settings HMI behavior is TBD.


## 15. FreeRTOS Scheduling

Current priorities:

| Task | Priority |
|---|---|
| `HeatTask` | `osPriorityHigh3` |
| `ConnectTask` | `osPriorityHigh` |
| `InputTask` | `osPriorityAboveNormal` |
| `SenseTask` | `osPriorityNormal` |
| `DisplayTask` | `osPriorityLow` |

Current task stack allocations are 128 words per application task.

Current FreeRTOS heap allocation is 8192 bytes.

RTOS configuration is maintained by STM32CubeMX.


## 16. Connectivity Architecture

### 16.1 Responsibilities

The connectivity system uses:

- STM32 cabinet firmware.
- ESP32-C6.
- Flutter mobile application.
- Firebase.

STM32 responsibilities:

- Cabinet control.
- Cabinet identity.
- Persistent cabinet configuration.
- Connectivity coordination.

ESP32-C6 responsibilities:

- BLE connectivity.
- Wi-Fi connectivity.
- Network communications.

Flutter responsibilities:

- User account interaction.
- Cabinet association.
- Wi-Fi network selection.
- Wi-Fi credential entry.
- Connectivity configuration.
- Firebase interaction.
- Remote cabinet interaction.

Firebase is the MVP cloud backend.


### 16.2 STM32 / ESP32-C6

The ESP32-C6 runs Espressif ESP-AT firmware for the MVP.

No custom ESP32 application firmware is planned for the MVP.

The STM32 communicates with the ESP32-C6 using UART and ESP-AT commands.

The STM32 is the application controller.

The ESP32-C6 is the connectivity peripheral.


### 16.3 Wi-Fi Provisioning

The intended provisioning flow is:

    Flutter application
        |
        | BLE
        v
    ESP32-C6
        |
        | connectivity data
        v
    STM32
        |
        | stored Wi-Fi configuration
        v
    ESP32-C6
        |
        v
    Wi-Fi network

The Flutter application handles network selection and credential entry.

The STM32 retains the Wi-Fi configuration.

The ESP32-C6 uses the configuration to establish the Wi-Fi connection.

The exact BLE/ESP-AT provisioning mechanism is TBD pending verification on
the selected ESP32-C6 hardware.


### 16.4 Cabinet Identity

The cabinet uses a numeric serial number.

The STM32 is the cabinet-side source of the serial number.

The serial-number format and storage implementation are TBD.


### 16.5 Cloud

Firebase is the MVP cloud platform.

MQTT is not used for the MVP.

The Firebase data model and remote command interface are TBD.


## 17. Open Architecture Items

- Nonvolatile-storage implementation.
- Wi-Fi Settings HMI.
- `DisplayTask` / `ConnectTask` interface.
- ESP-AT command set and recovery behavior.
- BLE provisioning mechanism.
- Wi-Fi credential replacement behavior.
- Cabinet serial-number format and storage.
- Firebase data model and remote command interface.
- Heater fault-detection algorithm.
- Production NTC fault thresholds.