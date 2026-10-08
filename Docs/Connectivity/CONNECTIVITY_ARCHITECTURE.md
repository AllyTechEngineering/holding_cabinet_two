# Connectivity Subsystem Architecture

## 1. Purpose

This document defines the firmware architecture of the Holding Cabinet
connectivity subsystem.

The architecture implements the requirements defined in
`CONNECTIVITY_REQUIREMENTS.md`.

System-level firmware architecture and subsystem boundaries are defined in
`../ARCHITECTURE.md`.


## 2. Architectural Boundary

The STM32L476 is the Holding Cabinet system controller and is authoritative
for cabinet operation.

The ESP32-C6 is an external communications device controlled by the STM32.

The ESP32-C6 runs Espressif ESP-AT firmware.

The connectivity subsystem does not own cabinet temperature control, proof
timing, local HMI operation, or other real-time cabinet functions.


## 3. Connectivity Software Layers

    ConnectTask
        |
        v
    Connectivity Logic
        |
        v
    ESP-AT Interface
        |
        v
    UART Transport
        |
        v
    USART1
        |
        v
    ESP32-C6


## 4. ConnectTask

`ConnectTask` owns STM32-side connectivity operation.

`ConnectTask`:

- Initializes the UART transport.
- Starts interrupt-driven UART reception.
- Processes received ESP-AT data.
- Delivers classified ESP-AT unsolicited events to connectivity logic.
- Processes ESP-AT command transactions.
- Calls connectivity logic processing.


## 5. Connectivity Logic

Connectivity logic is implemented by:

- `App/Connectivity/connectivity_logic.c`
- `App/Connectivity/connectivity_logic.h`

Connectivity logic coordinates:

- ESP32 communication.
- Provisioning.
- Wi-Fi connectivity.
- Network connectivity.
- Firebase communications.
- Remote communications.
- Connectivity fault handling and recovery.

`ConnectTask` provides task orchestration and delivers classified ESP-AT
events to connectivity logic.


## 6. ESP-AT Interface

The ESP-AT interface translates connectivity operations into ESP-AT commands
and processes ESP-AT responses and unsolicited result codes.

Project-specific ESP-AT interface behavior is defined in
`ESP_AT_INTERFACE.md`.


## 7. UART Transport

USART1 provides the transport between the STM32 and ESP32-C6.

UART configuration:

- 115200 baud
- 8 data bits
- No parity
- 1 stop bit
- No hardware flow control

Transmit operations use blocking UART transmission.

Receive operations use interrupt-driven UART reception.

Received bytes are passed to `ConnectTask` through the FreeRTOS
`qUartRxToConnect` queue.

The receive queue contains 128 `uint8_t` entries.

DMA is not used.


## 8. STM32 and ESP32 Roles

The STM32 is the master cabinet controller.

The ESP32-C6 operates as a slave communications device.

The STM32 initiates ESP-AT commands.

The STM32 determines how communications state and received remote commands
affect cabinet operation.

The ESP32-C6 provides communications services and does not control cabinet
operation.


## 9. Provisioning Architecture

Wi-Fi provisioning is initiated through the cabinet Settings workflow.

The mobile application performs:

- Cabinet association.
- Wi-Fi network selection.
- Wi-Fi credential entry.
- Provisioning data transfer.

BLE/GATT provided by ESP-AT is the local provisioning transport.

The STM32 controls BLE provisioning through ESP-AT.

Accepted Wi-Fi credentials are retained persistently by the ESP32-C6.

The detailed BLE/GATT command sequence and provisioning data format are TBD.


## 10. Wi-Fi Architecture

The ESP32-C6 provides Wi-Fi station connectivity.

Accepted Wi-Fi credentials are stored persistently by the ESP32-C6 using
ESP-AT persistent storage.

The STM32 controls Wi-Fi operation through ESP-AT commands.

Existing accepted Wi-Fi credentials remain valid until replacement
credentials have been successfully provisioned.

The detailed Wi-Fi command sequence and replacement-credential transaction
are TBD.


## 11. Network Architecture

The ESP32-C6 provides network and SSL/TLS transport for remote cabinet
communications.

The STM32 owns cabinet application data and application-layer processing.

Detailed network connection behavior is TBD.


## 12. Firebase Communications Architecture

Firebase Realtime Database is the MVP cloud backend.

The STM32 owns Firebase-facing cabinet communication logic.

The ESP32-C6 provides the required SSL/TLS network transport.

Detailed Firebase authentication, data synchronization, and remote
communication behavior are TBD.


## 13. Remote Command Architecture

The mobile application may issue supported remote cabinet commands through
Firebase.

The STM32 remains authoritative for cabinet operation and determines whether
a received remote command is valid and may be applied.

The detailed remote-command interface and arbitration behavior are TBD.


## 14. Connectivity State Architecture

The MVP connectivity logic uses three top-level states:

- `STARTUP` — ESP-AT startup synchronization is in progress.
- `READY` — ESP-AT communication is established and normal connectivity
  operations may be performed.
- `PROVISIONING` — Wi-Fi provisioning is active.

Wi-Fi, Internet, and Firebase availability are represented as status
conditions rather than separate top-level connectivity states.

State transitions are:

- `STARTUP` → `READY` after successful ESP-AT startup synchronization.
- `READY` → `PROVISIONING` when Wi-Fi provisioning is initiated.
- `PROVISIONING` → `READY` after successful provisioning, cancellation, or
  provisioning timeout.

Connectivity state changes are driven by:

- ESP-AT `ready` URC.
- Wi-Fi provisioning start request.
- Wi-Fi provisioning success.
- Wi-Fi provisioning cancellation.
- Wi-Fi provisioning timeout.

ESP-AT command results remain ESP-AT interface results and are processed by
connectivity logic as needed.

No separate generic connectivity event framework is required for the MVP.


## 15. Fault and Recovery Architecture

UART transport errors are handled by the UART transport layer.

ESP-AT command and protocol errors are handled by the ESP-AT interface.

ESP32 availability, Wi-Fi connectivity, network connectivity, and
remote-service failures are handled by connectivity logic.

Wi-Fi, Internet, or Firebase unavailability does not disable local cabinet
operation.

Detailed recovery behavior shall be defined as the corresponding connectivity
functions are implemented and verified.


## 16. Security Architecture

Wi-Fi credentials are transferred during the provisioning process and stored
persistently by the ESP32-C6.

Firebase authentication and production device-credential handling remain TBD.


## 17. References

Applicable ESP32-C6 and ESP-AT documentation is maintained under
`../Reference/`.