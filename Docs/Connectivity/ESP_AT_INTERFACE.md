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

TBD.


## 5. Startup and Synchronization

TBD.


## 6. ESP-AT Configuration

TBD.


## 7. Provisioning Commands

TBD.


## 8. Wi-Fi Commands

TBD.


## 9. Network Commands

TBD.


## 10. Firebase Transport Commands

TBD.


## 11. Unsolicited Result Codes

TBD.


## 12. Command Timeouts

TBD.


## 13. ESP-AT Error Handling

TBD.


## 14. ESP-AT Command Set

TBD.


## 15. References

Applicable Espressif ESP-AT documentation is maintained under
`../Reference/`.