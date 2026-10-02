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

Persistent configuration is required for:

- Confirmed temperature setpoint.
- Temperature unit.
- Confirmed timer duration.
- Wi-Fi configuration.

Active-run state is not persistent.

`settings_store.c/.h` currently exist as placeholders.

The final nonvolatile-storage implementation is TBD.

The STM32 shall be the authoritative persistent store for cabinet Wi-Fi
configuration.


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

### 17.1 System Responsibilities

The MVP connectivity system consists of:

- STM32 cabinet firmware.
- ESP32-C6 connectivity processor.
- Flutter mobile application.
- Firebase backend.

The STM32 remains the cabinet application controller.

The ESP32-C6 provides wireless connectivity.

The Flutter application handles user-facing connectivity configuration,
cabinet association, Firebase interaction, and remote cabinet interaction.


### 17.2 ESP32-C6

The selected development module is the ESP32-C6-DEVKITC-1-N8.

The ESP32-C6 is intended to run Espressif ESP-AT firmware for the MVP.

No custom ESP32 application firmware is currently planned for the MVP.


### 17.3 STM32 / ESP32 Interface

The STM32 communicates with the ESP32-C6 over USART2.

Current USART2 configuration:

- 115200 baud
- 8 data bits
- No parity
- 1 stop bit
- No hardware flow control

The STM32 initiates ESP-AT commands.

The ESP32-C6 operates as the connectivity peripheral.


### 17.4 UART Transport

`uart_transport.c/.h` implements the current STM32 UART transport.

Transmit uses blocking HAL UART transmission.

Receive uses interrupt-driven, one-byte reception.

Received bytes are placed into `qUartRxToConnect`.

UART errors are recorded and receive operation is rearmed.

DMA and hardware flow control are not currently used.


### 17.5 ESP-AT Layer

The ESP-AT command/response layer is not yet implemented.

Command framing, response parsing, timeouts, retries, startup handling, and
recovery behavior are TBD.


### 17.6 Cabinet Identity

The cabinet uses a numeric serial number.

The STM32 is the cabinet-side source of the serial number.

The serial-number format and storage implementation are TBD.


### 17.7 Wi-Fi Provisioning

The mobile application handles:

- Cabinet association.
- Wi-Fi network selection.
- Wi-Fi credential entry.
- Connectivity configuration.

BLE is the intended local provisioning transport.

The exact ESP-AT BLE provisioning mechanism remains TBD pending verification
with the selected ESP32-C6 ESP-AT firmware.

Wi-Fi configuration received during provisioning is retained by the STM32.

The ESP32-C6 uses the supplied configuration to connect to the Wi-Fi network.


### 17.8 Wi-Fi Configuration Changes

Existing Wi-Fi configuration is changed through the cabinet Settings workflow
and mobile application.

The configuration replacement and failure-recovery transaction is TBD.


### 17.9 Cloud

Firebase is the MVP cloud backend.

MQTT is not used for the MVP.

The Firebase data model, synchronization behavior, telemetry, and remote
command interface are TBD.


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