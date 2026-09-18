# Folders and Files Plan
```
App/
├── Common/
│   ├── app_config.h        # constants: temp range (65–120°F), time bounds (0:30–10:00),
│   │                        # hysteresis band, debounce interval, chord hold (5000ms),
│   │                        # toggle interval (2000ms), inactivity timeout (3min),
│   │                        # DisplayTask wake period (100ms)
│   └── app_types.h          # shared typedefs: ButtonEvent_t (incl. EVT_ENTER_SETTINGS),
│                             # HeatCommand_t, HeatStatus_t — the CubeMX queue Item Size types
│
├── Sensing/
│   ├── sensor_task.c/h       # SenseTask — periodic ADC read, pushes to qSenseToHeat
│   └── thermistor_driver.c/h # NTC Beta-equation conversion (ported from main.c bring-up)
│
├── Control/
│   ├── control_task.c/h      # HeatTask — consumes qSenseToHeat + qDisplayToHeat,
│   │                          # drives relay_driver, reports to qHeatToDisplay
│   └── heater_control.c/h    # pure bang-bang/hysteresis algorithm — no HAL/RTOS calls,
│                              # so it's testable in isolation
│
├── Input/                    # ← new, InputTask has no home yet
│   ├── input_task.c/h        # InputTask — debounce loop, chord timing, pushes to qInputToDisplay
│   └── switch_driver.c/h     # raw GPIO read + active-low debounce logic (ported from bring-up)
│
├── Display/
│   ├── display_task.c/h      # DisplayTask — the state machine, owns setpoint/mode/timer
│   ├── lcd1602_driver.c/h    # LCD1602/PCF8574 I2C driver (ported from bring-up)
│   └── settings_store.c/h    # ← new, Flash-EEPROM read/write (temp units, setpoint, timer)
│
├── Actuators/
│   └── relay_driver.c/h      # active-low relay GPIO driving (ported from bring-up)
│   └── buzzer_driver.c/h      # active-high (NPN low side switch) GPIO driving (ported from bring-up)
│
└── Connectivity/
    └── connect_task.c/h      # ConnectTask — untouched, Models 3/4 
```