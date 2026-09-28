# Holding Cabinet / Proofing Oven

Firmware for a consumer heated proofing cabinet proof of concept.

The current prototype uses an STM32 NUCLEO-L476RG development board with
FreeRTOS and CMSIS-RTOS2.

The firmware provides:

- Timed and untimed proofing operation
- Cabinet temperature sensing and control
- Four-button local user interface
- 16x2 LCD user interface
- Proofing countdown timer
- Active-run editing
- Completion notification
- Firmware fault handling

## Hardware

The current proof of concept uses an existing heated proofing cabinet with its
original enclosure, heater, and temperature sensor.

Detailed hardware design is maintained separately in:

`AllyTechEngineering/holding-cabinet-hardware`

See `Docs/HARDWARE.md` for the firmware-project hardware documentation
boundary.

## Documentation

- `Docs/REQUIREMENTS.md` — firmware behavioral requirements
- `Docs/ARCHITECTURE.md` — firmware architecture, tasks, queues, control, and data flow
- `Docs/HMI.md` — front-panel HMI behavior, LCD screens, controls, and state transitions
- `Docs/HARDWARE.md` — hardware documentation ownership and repository reference

## STM32 Configuration

The STM32CubeMX project is maintained in:

`holding_cabinet_two.ioc`

The CubeMX project is the authoritative source for MCU peripheral, GPIO, and
pin configuration.

## Development Environment

- STM32 NUCLEO-L476RG
- STM32L476RG
- FreeRTOS
- CMSIS-RTOS2
- STM32 HAL
- STM32CubeMX
- VS Code with STM32 extensions