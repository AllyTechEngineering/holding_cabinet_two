# ESP32-C6 ESP-AT Bring-Up

## Purpose

This document records the ESP32-C6 development-board setup and ESP-AT bring-up used during development of the Holding Cabinet Two project.

The board is a low-cost ESP32-C6 development board based on an ESP32-C6-WROOM-1 module. The part number on Amazon is ESP32-C6-DevKitM-1 and sold by Dorhea. It is a Chinese knock-off with no documentation that was purchased on Amazon.

## Development Board

Observed hardware:

- Module: ESP32-C6-WROOM-1 (not really sure)
- Flash: 4 MB
- Board designation when purchased: ESP32-C6-1-N4 Development Board
- USB-C connector labeled `COM`
- USB-C connector labeled `USB`
- Onboard CH343 USB-to-UART device
- BOOT button
- RST button
- GPIO headers
- GPIO4, GPIO5, GPIO6, and GPIO7 available on headers

The exact board schematic has not yet been identified.
Dorhea suggested looking for documents related to nanoESP32-C6. I found: https://github.com/wuxx/nanoESP32-C6/blob/master/README_en.md

## PC Development Environment

- Windows 11
- VS Code
- Microsoft Serial Monitor extension
- Python 3.14
- esptool v5.4.0

Working directory used for ESP32-C6 flashing:

    C:\esp_32_c6


## AT Command Set

https://docs.espressif.com/projects/esp-at/en/latest/esp32c6/AT_Command_Set/index.html


## ESP-AT Source

ESP-AT repository:

    https://github.com/espressif/esp-at

ESP32-C6 ESP-AT documentation:

    https://docs.espressif.com/projects/esp-at/en/latest/esp32c6/

## ESP-AT Test Build

A prebuilt ESP-AT artifact from the Espressif GitHub Actions workflow was used for initial bring-up.

Build information:

- Workflow: Build ESP-AT Project
- Run number: 1071
- Run ID: 35314587747
- Build date: 2026-09-18
- Commit: `c540d0168d746bbd2272714bb1021a7ce9fb6472`
- Artifact: `esp32c6-4MB-at`
- Artifact ID: `10533829343`

The downloaded artifact contained:

- `build/`
- `NOTES`
- `sdkconfig`

The `build/factory/` directory contained:

- `factory_ESP32C6-4MB.bin`
- `factory_ESP32C6-4MB_unfilled.bin`

The image used for bring-up was:

    factory_ESP32C6-4MB_unfilled.bin

## Installing esptool

Install:

    py -m pip install esptool

Verify:

    py -m esptool version

Installed version:

    esptool v5.4.0

## COM USB Connection

Connecting the development board through its `COM` USB-C connector produced:

    USB-Enhanced-SERIAL CH343 (COM5)

The STM32 NUCLEO board was also connected during testing as:

    STMicroelectronics STLink Virtual COM Port (COM3)

## ESP32-C6 Identification

Command:

    py -m esptool --port COM5 chip-id

Observed:

- ESP32-C6
- QFN40
- Revision v0.2
- 40 MHz crystal
- Wi-Fi 6
- Bluetooth 5 LE
- IEEE 802.15.4
- 160 MHz

The command initially reported:

    Unknown Embedded Flash

Flash identification was therefore checked separately.

## Flash Identification

Command:

    py -m esptool --port COM5 flash-id

Observed:

- Manufacturer: `68`
- Device: `4016`
- Flash size: 4 MB

## Flashing ESP-AT

The factory image was copied to:

    C:\esp_32_c6

Flash command:

    py -m esptool --port COM5 write-flash 0x0 factory_ESP32C6-4MB_unfilled.bin

The flash operation completed successfully.

esptool reported:

    Hash of data verified

The image size written was 2,224,384 bytes.

## ESP-AT Boot Test

VS Code Microsoft Serial Monitor settings:

- Port: COM5
- Baud: 115200
- View: Text
- Line ending: CRLF

Pressing the development board `RST` button produced the ESP-AT boot log on COM5.

Relevant boot information:

    ESP-ROM:esp32c6-20220919
    Build:Sep 19 2022

    I (23) boot: ESP-IDF v5.4.4-dirty 2nd stage bootloader
    I (24) boot: compile time Sep 18 2026 06:28:05
    I (24) boot: chip revision: v0.2
    I (35) boot.esp32c6: SPI Flash Size : 4MB

ESP-AT loaded successfully:

    I (465) boot: Loaded app from partition at offset 0x60000

ESP-AT reported:

    I (969) at-init: at param mode: 1
    I (1059) at-uart: AT cmd port:uart1 tx:7 rx:6 cts:5 rts:4 baudrate:115200
    I (1059) at-uart: AT log port:uart0 baudrate:115200
    I (1060) at-init: module_name: ESP32C6-4MB
    I (1064) at-init: max tx power=78, ret=0
    I (1068) at-init: v5.1.0.0-dev

## UART Configuration Reported by ESP-AT

ESP-AT command UART:

- UART1
- TX: GPIO7
- RX: GPIO6
- CTS: GPIO5
- RTS: GPIO4
- Baud: 115200

ESP-AT log UART:

- UART0
- Baud: 115200

COM5 receives the UART0 ESP-AT boot/log output.

An `AT` command sent through COM5 with CRLF did not return `OK`.

## Native USB Connection

Connecting the development board through the USB-C connector labeled `USB` produced:

    USB Serial Device (COM4)

Pressing `RST` while VS Code Serial Monitor was connected to COM4 caused the serial port to disconnect as the ESP32-C6 reset.

No further testing of COM4 was completed.

## Current Status

Verified:

- Development board powers normally.
- CH343 interface operates through the `COM` USB connector.
- esptool communicates with the ESP32-C6 through COM5.
- ESP32-C6 revision is v0.2.
- Flash size is 4 MB.
- ESP-AT factory image was successfully flashed.
- Flash contents passed esptool hash verification.
- ESP-AT boots successfully.
- ESP-AT identifies the module configuration as `ESP32C6-4MB`.
- Installed test build reports ESP-AT `v5.1.0.0-dev`.
- ESP-AT boot/log output is available on COM5.
- ESP-AT reports its AT command interface as UART1 on GPIO7/GPIO6.
- GPIO7 and GPIO6 are available on the development-board headers.
- The board's second USB connector enumerates as COM4.

## Next Bring-Up Step

Identify documentation or a schematic for this specific ESP32-C6 development-board/clone and verify the onboard CH343 UART connections.

Then continue testing access to the ESP-AT command UART and verify a basic:

    AT

response:

    OK