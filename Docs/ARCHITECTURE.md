# Architecture — Holding Cabinet (Proofer)

System-level view of the firmware and hardware architecture: how the
system is built, not what it does or why specific decisions
were made.

---

## 1. System Overview

Single-MCU embedded system on a shared PCBA and firmware codebase,
configured per product model at build time.

**Development platform:** NUCLEO-L476RG (STM32L476RG)
**Production target:** STM32C031C6
**RTOS:** FreeRTOS / CMSIS-RTOS v2
**Companion Wi-Fi/BLE module (Models 3/4):** ESP32-C6-MINI-1-N4

The STM32 is the real-time control brain for all models. The companion
module owns the radio stack entirely, communicating to the STM32 over
USART.

---

## 2. Peripheral / Bus Summary

See holding_cabinet_two.txt
See holding_cabinet_two.pdf for more details.
Note: these files are automatically updated when the Generate Code and 
Generate Reports are used in STM32Cube MX
---

## 3. Interfaces
See holding_cabinet_two.txt
See holding_cabinet_two.pdf for more details.
Note: these files are automatically updated when the Generate Code and 
Generate Reports are used in STM32Cube MX

### 3.1 I2C2 Bus (Shared)
See holding_cabinet_two.txt
See holding_cabinet_two.pdf for more details.
Note: these files are automatically updated when the Generate Code and 
Generate Reports are used in STM32Cube MX

### 3.2 STM32 ↔ Wi-Fi/BLE Module (UART)
Module selected (ESP32-C6-MINI-1-N4 on a breakout board). 


### 3.3 BLE ↔ Companion App
TODO: future

---

## 4. Setup Menu Entry

TODO: future

---

## 5. Data Flow (Control Loop, High-Level)


---

## 6. FreeRTOS Task Architecture



---

## 7. Build Configuration Strategy



---

## 8. Revision Notes

