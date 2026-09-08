# Holding Cabinet AKA Proofing Oven, Heated Proofing Box
Consumer bread proofing cabinet AKA heated proofing box. Single embedded system and firmware code base.

## Prototype
Holding cabinet by other (purchase from Amazon) shall provide the hardware (heating elements, housing, NTC temperature sensor, and related wiring). The goal is to replace the controller that comes with the product with the NUCLEO-L476RG dev board and use off-the-shelf relay and display breakout boards. Switches and LED indicators shall be on a solderless breadboard.


## IoT Connectivity
Once the prototype is operational, the plan is to use an off-the-shelf ESP32-C6-MINI-1-N4 on a breakout board with PCBA antenna. The STM32 shall us the USART to communicate with the ESP32.

Using Flutter, a companion app will be developed to allow a user, that is not near the proofing oven, to be able to monitor the progress of the item in the oven, change the temperature or other settings or turn the device off remotely.
