# ESP-AT Interface Specification

## 1. Purpose

This document defines the ESP-AT interface used between the Holding Cabinet
STM32 firmware and the ESP32-C6.

The authoritative definitions of ESP-AT commands and responses are provided
by the applicable Espressif ESP-AT documentation.


## 2. Interface Boundary

    STM32L476
        |
        | USART1
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

    STM32 USART1 TX (PA9)  ---> ESP32-C6 GPIO6 / UART1 RX
    STM32 USART1 RX (PA10) <--- ESP32-C6 GPIO7 / UART1 TX
    STM32 GND              ----- ESP32-C6 GND

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

A response-dependent command transaction shall remain active until a terminal
response is received or the transaction ends due to timeout.

A terminal `OK` response shall complete the active transaction successfully.

A terminal `ERROR` response shall complete the active transaction
unsuccessfully.

Non-terminal response lines shall not complete the active transaction.

A second response-dependent command transaction shall not begin while another
response-dependent transaction is active.

ESP-AT receive processing shall assemble incoming UART bytes into response
lines terminated by CR-LF (`\r\n`).

Empty CR-LF sequences between response lines shall be ignored.

Completed non-empty lines shall be passed to the ESP-AT response-processing
logic for classification.

Line assembly shall not determine command completion. Command completion is
determined separately from terminal ESP-AT responses such as `OK` and `ERROR`.

An ESP-AT command response may contain zero or more non-terminal response
lines before the terminal response.

Each completed response line shall be processed independently. Receipt of a
non-terminal response line shall not by itself complete the active command.

During ESP-AT initialization, the STM32 shall disable command echo using
`ATE0`.

Before command echo has been successfully disabled, received command-echo
lines shall be recognized and ignored by the ESP-AT interface.

After `ATE0` has completed successfully, command echo is not expected during
normal ESP-AT operation.

Additional response parsing behavior is defined as the corresponding ESP-AT
interface functions are implemented.


## 5. Startup and Synchronization

During connectivity initialization, the STM32 shall establish valid ESP-AT
communication with the ESP32-C6 before connectivity is considered operational.

UART reception shall be active before ESP-AT startup synchronization begins.

The startup synchronization sequence shall be:

1. Wait for the ESP32-C6 `ready` unsolicited result code.
2. When `ready` is received, send `ATE0`.
3. Require terminal `OK`.
4. Declare the ESP-AT interface initialized.

`ATE0` disables command echo for subsequent normal operation and provides
command/response confirmation before the ESP-AT interface is considered
initialized.


## 6. ESP-AT Configuration

The ESP32-C6 shall operate using released Espressif ESP-AT firmware.

The MVP shall not require custom ESP32 application firmware or a custom
ESP-AT build.

The STM32 shall configure only ESP-AT functions required by the Holding
Cabinet MVP.

ESP-AT persistent configuration shall not be treated as the authoritative
source of cabinet Wi-Fi configuration.

Detailed initialization commands are TBD.


## 7. Provisioning Commands

Wi-Fi provisioning shall use the standard BLE/GATT capabilities provided by
the released ESP32-C6 ESP-AT firmware.

The STM32 shall control BLE operation through ESP-AT commands.

The BLE interface shall provide bidirectional data transport between the
Flutter provisioning application and the STM32 connectivity subsystem.

Provisioning control shall include the ability to:

- Enable BLE operation.
- Advertise the cabinet for provisioning.
- Detect BLE connection activity.
- Receive provisioning data from the application.
- Send provisioning status and results to the application.
- Disable BLE operation when provisioning ends.

Provisioning shall be stopped when:

- Wi-Fi provisioning succeeds.
- The user cancels provisioning.
- The 5-minute provisioning timeout expires.

The MVP shall use the standard GATT services and characteristics available in
the released ESP32-C6 ESP-AT firmware.

The exact ESP-AT BLE command sequence and application data format are TBD and
shall be defined from the applicable Espressif ESP-AT documentation.


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

The ESP-AT interface shall support SSL/TLS network connections required for
remote cabinet communications.

The STM32 shall provide application-layer network data to the ESP32-C6 through
ESP-AT.

Received network data shall be delivered by ESP-AT to the STM32 through the
UART interface.

The exact ESP-AT network command sequence is TBD.


## 10. Firebase Transport Commands

Firebase Realtime Database communications shall use its HTTPS/REST interface
through the ESP32-C6 SSL/TLS network transport.

The STM32 shall construct and process the Firebase application-layer
communications.

The ESP32-C6 shall provide the network transport and shall not own Firebase
application logic.

The interface shall support outbound Firebase data operations and a persistent
connection for asynchronous Firebase data reception.

The exact Firebase REST requests and ESP-AT transport command mapping are TBD.

Flutter application account-management behavior is outside this interface.


## 11. Unsolicited Result Codes

The ESP-AT interface shall process unsolicited result codes and asynchronous
data required to detect at minimum:

- BLE connection state
- BLE provisioning data reception
- Wi-Fi connection
- Wi-Fi disconnection
- Network address acquisition
- Network connection closure
- Incoming network data
- Other asynchronous events required by Firebase communications

Recognized unsolicited result codes shall be classified before command-response
transaction processing.

A recognized unsolicited result code shall not complete or otherwise change
the state of an active command transaction.

The exact unsolicited result code set is TBD.


## 12. Command Timeouts

Each ESP-AT operation that requires a response shall have a defined timeout.

The timeout duration for a response-dependent command transaction shall be
provided when the transaction is started.

The ESP-AT interface shall determine whether an active transaction has timed
out using the STM32 system tick.

A timed-out transaction shall end with a timeout result distinct from terminal
`OK` and `ERROR` responses.

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
- BLE/GATT provisioning
- Wi-Fi station connection
- Wi-Fi connection status
- SSL/TLS network transport
- Firebase-facing communications
- Required connection recovery

The exact command list is TBD.


## 15. References

Applicable ESP32-C6 and ESP-AT documentation is maintained under
`../Reference/`.