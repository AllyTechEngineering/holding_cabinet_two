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

The Flutter application is outside the firmware implementation boundary.

The firmware interacts with the Flutter application only through the defined
BLE provisioning behavior and remote network interfaces.


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
    USART2
        |
        v
    ESP32-C6


## 4. ConnectTask

`ConnectTask` owns connectivity operation on the STM32.


## 5. Connectivity Logic

Connectivity logic coordinates:

- ESP32 communication
- Provisioning
- Wi-Fi connectivity
- Network connectivity
- Firebase communications
- Remote communications
- Connectivity fault handling and recovery

The connectivity state model is TBD.


## 6. ESP-AT Interface

The ESP-AT interface translates connectivity operations into ESP-AT commands
and processes ESP-AT responses and unsolicited result codes.

Project-specific ESP-AT interface behavior is defined in
`ESP_AT_INTERFACE.md`.


## 7. UART Transport

USART2 provides the transport between the STM32 and ESP32-C6.

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

The STM32 owns the authoritative persistent Wi-Fi configuration.

The ESP32-C6 provides communications services and does not control cabinet
operation.

The ESP32-C6 shall not be relied upon as the authoritative persistent Wi-Fi
credential store.


## 9. Provisioning Architecture

Wi-Fi provisioning is initiated from the cabinet Settings mode.

The cabinet HMI does not provide SSID or password entry.

The Flutter application provides the user interface for connectivity
configuration, including Wi-Fi network selection and password entry.

The Flutter application communicates with the ESP32-C6 through BLE using
Espressif BluFi.

The provisioning architecture is:

    Cabinet Settings
          |
          v
    STM32 starts provisioning
          |
          v
    ESP32-C6 enables BluFi
          |
          v
    Flutter application connects over BLE
          |
          v
    User selects Wi-Fi network and enters password
          |
          v
    ESP32-C6 attempts Wi-Fi connection
          |
          +---- failure ----> provisioning remains active
          |
          v
    Wi-Fi connected / network obtained
          |
          v
    STM32 accepts and persists new credentials
          |
          v
    BluFi stops
          |
          v
    Normal connectivity operation

The same path is used for initial provisioning and replacement of an existing
Wi-Fi configuration.

Provisioning remains active for a maximum of 5 minutes.

The 5-minute timer begins when the firmware starts the provisioning session.

Failed Wi-Fi attempts do not restart the provisioning timer.

Provisioning cancellation or timeout retains the previously stored Wi-Fi
credentials.

Provisioning success requires successful Wi-Fi connection and network
acquisition.

Internet and Firebase connectivity are not part of provisioning success.

A reboot is not required following successful provisioning.


## 10. Wi-Fi Architecture

During normal operation, the STM32 supplies the authoritative stored Wi-Fi
configuration for connection use.

The ESP32-C6 performs Wi-Fi communication through ESP-AT.

Failure to connect to Wi-Fi or loss of Wi-Fi connectivity is treated as a
connectivity-status condition rather than a cabinet fault.

Local cabinet operation continues without Wi-Fi.


## 11. Network Architecture

The ESP32-C6 provides IP network communication for the STM32 connectivity
subsystem.

Loss of Internet connectivity does not affect proofing control or local HMI
operation.


## 12. Firebase Communications Architecture

Firebase is a remote service used by the connectivity subsystem.

The firmware implementation is responsible only for the Firebase-facing
network/API behavior required by the cabinet.

Flutter application account management, cabinet-account association, and
Firestore application architecture are outside the firmware architecture.

Loss of Firebase connectivity does not affect local cabinet operation.

The detailed Firebase transport and API implementation is TBD.


## 13. Remote Command Architecture

Remote commands originate outside the cabinet connectivity subsystem.

The STM32 remains authoritative for determining whether a remote command is
valid and whether it may affect cabinet operation.

The ESP32-C6 does not directly modify cabinet operating state.

Detailed remote command behavior is TBD.


## 14. Connectivity State Architecture

The connectivity subsystem shall distinguish at minimum between:

- ESP32 communication availability
- Provisioning active
- Wi-Fi connected / disconnected
- Internet connectivity availability
- Firebase connectivity availability

The detailed state machine is TBD.


## 15. Fault and Recovery Architecture

UART transport errors are handled by the UART transport layer.

ESP-AT command and protocol errors are handled by the ESP-AT interface.

ESP32 availability, network connectivity, and remote-service failures are
handled by the connectivity logic.

Loss of valid ESP-AT communication with the ESP32-C6 results in the defined
ESP32 communication fault.

Wi-Fi failure, Internet failure, and Firebase failure do not by themselves
enter the cabinet Error state.

Detailed retry and recovery behavior is TBD.


## 16. Security Architecture

The STM32 owns the authoritative persistent Wi-Fi credentials.

Candidate credentials received during provisioning do not replace the stored
configuration until provisioning succeeds.

Credential storage implementation and protection are TBD.


## 17. References

Applicable ESP32-C6 and ESP-AT documentation is maintained under
`../Reference/`.