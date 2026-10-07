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

`ConnectTask` owns connectivity operation on the STM32.


## 5. Connectivity Logic

Connectivity logic is implemented by:

- `App/Connectivity/connectivity_logic.c`
- `App/Connectivity/connectivity_logic.h`

Connectivity logic coordinates:

- ESP32 communication
- Provisioning
- Wi-Fi connectivity
- Network connectivity
- Firebase communications
- Remote communications
- Connectivity fault handling and recovery

`ConnectTask` provides task orchestration and delivers classified ESP-AT
events to connectivity logic.

The connectivity state model is TBD.


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

TBD.


## 10. Wi-Fi Architecture

TBD.


## 11. Network Architecture

TBD.


## 12. Firebase Communications Architecture

TBD.


## 13. Remote Command Architecture

TBD.


## 14. Connectivity State Architecture

TBD.


## 15. Fault and Recovery Architecture

UART transport errors are handled by the UART transport layer.

ESP-AT command and protocol errors are handled by the ESP-AT interface.

ESP32 availability, network connectivity, and remote-service failures are
handled by the connectivity logic.

Detailed recovery behavior is TBD.


## 16. Security Architecture

TBD.


## 17. References

Applicable ESP32-C6 and ESP-AT documentation is maintained under
`../Reference/`.