# ESP-AT Interface Specification

## 1. Purpose

This document defines the ESP-AT interface used between the Holding Cabinet
STM32 firmware and the ESP32-C6.

The authoritative definitions of ESP-AT commands and responses are provided
by the applicable Espressif ESP-AT documentation.


## 2. Interface Boundary

    STM32L476
        |
        | USART2
        |
        v
    ESP32-C6
        |
        v
    Espressif ESP-AT firmware

The STM32 is the command initiator.

The ESP32-C6 is the ESP-AT slave communications device.


## 3. UART Interface

UART configuration:

- 115200 baud
- 8 data bits
- No parity
- 1 stop bit
- No hardware flow control

Connections:

    STM32 USART2 TX (PA2) ---> ESP32-C6 GPIO6 / UART1 RX
    STM32 USART2 RX (PA3) <--- ESP32-C6 GPIO7 / UART1 TX
    STM32 GND             ----- ESP32-C6 GND

ESP32-C6 ESP-AT UART1 signals:

- GPIO6 — UART1 RX
- GPIO7 — UART1 TX
- GPIO5 — CTS
- GPIO4 — RTS

CTS and RTS are not used.


## 4. Command Processing

The STM32 shall issue ESP-AT commands through the ESP-AT interface.

The ESP-AT interface shall process command responses and unsolicited result
codes independently.

Only one response-dependent ESP-AT command transaction shall be active at a
time unless a later architecture revision explicitly supports concurrent
transactions.

Detailed response parsing is TBD.


## 5. Startup and Synchronization

During connectivity initialization, the STM32 shall verify that valid ESP-AT
communication can be established with the ESP32-C6.

The initial synchronization command sequence is TBD.


## 6. ESP-AT Configuration

The ESP32-C6 shall operate using Espressif ESP-AT firmware.

The STM32 shall configure only ESP-AT functions required by the Holding
Cabinet MVP.

ESP-AT persistent configuration shall not be treated as the authoritative
source of cabinet Wi-Fi configuration.

Detailed initialization commands are TBD.


## 7. Provisioning Commands

Wi-Fi provisioning shall use Espressif BluFi support provided by ESP-AT.

Provisioning control shall include the ability to:

- Enable BluFi provisioning.
- Detect BluFi connection activity.
- Receive provisioning results.
- Detect successful Wi-Fi connection.
- Disable BluFi provisioning.

Provisioning shall be stopped when:

- Wi-Fi provisioning succeeds.
- The user cancels provisioning.
- The 5-minute provisioning timeout expires.

The exact ESP-AT command sequence and unsolicited result codes used for BluFi
provisioning are TBD and shall be defined from the applicable Espressif
ESP-AT documentation.


## 8. Wi-Fi Commands

The ESP-AT interface shall support the commands required to:

- Configure the ESP32-C6 for station Wi-Fi operation.
- Apply Wi-Fi credentials under STM32 control.
- Initiate Wi-Fi connection.
- Detect successful Wi-Fi connection.
- Detect network acquisition.
- Detect Wi-Fi disconnection.

The STM32-stored Wi-Fi credentials are authoritative.

The exact command set is TBD.


## 9. Network Commands

The ESP-AT interface shall support the network operations required for remote
cabinet communications.

The required TCP, SSL/TLS, HTTP, or other network command set is TBD.


## 10. Firebase Transport Commands

The ESP-AT interface shall provide the network transport required for the
STM32 firmware to communicate with the required Firebase service.

The exact Firebase API and ESP-AT transport command mapping is TBD.

Flutter application account-management behavior is outside this interface.


## 11. Unsolicited Result Codes

The ESP-AT interface shall process unsolicited result codes required to detect
at minimum:

- BluFi connection state
- Wi-Fi connection
- Wi-Fi disconnection
- Network address acquisition
- Network connection closure
- Other asynchronous events required by the selected Firebase transport

The exact unsolicited result code set is TBD.


## 12. Command Timeouts

Each ESP-AT operation that requires a response shall have a defined timeout.

Timeout values are TBD.

A single ESP-AT command timeout shall not automatically be classified as an
ESP32 communication failure.

The connectivity logic shall determine when command failures constitute loss
of valid ESP-AT communication.


## 13. ESP-AT Error Handling

ESP-AT command responses indicating failure shall be handled by the ESP-AT
interface.

UART framing, overrun, noise, parity, and transport-level errors are handled
by the UART transport layer.

Wi-Fi connection failure is not an ESP32 communication failure.

Internet or Firebase unavailability is not an ESP32 communication failure.

The detailed retry policy is TBD.


## 14. ESP-AT Command Set

The MVP ESP-AT command set shall be limited to commands required for:

- ESP32 communication verification
- ESP-AT initialization
- BluFi provisioning
- Wi-Fi station connection
- Wi-Fi connection status
- Network transport
- Firebase-facing communications
- Required connection recovery

The exact command list is TBD.


## 15. References

Applicable Espressif ESP-AT documentation is maintained under
`../Reference/`.