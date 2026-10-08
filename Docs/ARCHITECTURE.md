## 13. Persistence

Persistent proofing configuration is required for:

- Confirmed temperature setpoint.
- Temperature unit.
- Confirmed timer duration.

Wi-Fi configuration shall also persist across power cycles.

Active-run state is not persistent.

`settings_store.c/.h` currently exist as placeholders for STM32-owned
persistent proofing configuration.

The final STM32 nonvolatile-storage implementation is TBD.

Accepted Wi-Fi credentials are stored persistently by the ESP32-C6 using
ESP-AT persistent storage.