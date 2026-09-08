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


---

## 3. Interfaces

### 3.1 I2C2 Bus (Shared)
STM32 as master. See docs/reference/LCD1602_I2C_Reference.md for PCF8574
bit mapping and HD44780U init sequence.

### 3.2 STM32 ↔ Wi-Fi/BLE Module (UART)
Module selected (ESP32-C6-MINI-1-N4 on a breakout board). 


### 3.3 BLE ↔ Companion App


---

## 4. Setup Menu Entry

Entered by holding SW1 (Up) + SW2 (Down) for 3–5 seconds at power-on.
Button state sampled early in boot, before normal UI task starts.

First defined action: **Reprovision Wi-Fi** — clears stored network
credentials via `wifi_prov_mgr_reset_provisioning()`, device re-enters
BLE advertising for a new provisioning session. 

---

## 5. Data Flow (Control Loop, High-Level)


---

## 6. FreeRTOS Task Architecture



---

## 7. Build Configuration Strategy



---

## 8. Revision Notes

