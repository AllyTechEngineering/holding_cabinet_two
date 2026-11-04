# Folders and Files Plan

```
App/
├── Common/
│   ├── app_config.h         # application constants
│   └── app_types.h          # shared application types
│
├── Sensing/
│   ├── sensor_task.c/h      # SenseTask — temperature acquisition
│   └── thermistor_driver.c/h
│                            # NTC ADC-to-temperature conversion
│
├── Control/
│   ├── control_task.c/h     # HeatTask — heater-control task
│   └── heater_control.c/h   # bang-bang/hysteresis control algorithm
│
├── Input/
│   ├── input_task.c/h       # InputTask — button processing
│   └── switch_driver.c/h    # low-level switch interface
│
├── Display/
│   ├── display_task.c/h     # DisplayTask — HMI state machine
│   ├── lcd1602_driver.c/h   # LCD1602/PCF8574 driver
│   ├── settings_store.c/h   # persistent application settings
│   └── time_editor.c/h      # countdown-duration editor and hold ramp
│
├── Actuators/
│   ├── relay_driver.c/h     # heater output driver
│   └── buzzer_driver.c/h    # buzzer output driver
│
└── Connectivity/
    └── connect_task.c/h     # ConnectTask — connectivity task

```