# LED Indicators

## System power LED

The system power LED is powered from the switched 3 V rail. Turning on
the electrical power switch powers this LED. Firmware does not control
it. The LED indicates that the rail is powered.

## Heater LED

The heater LED is controlled by an active-high GPIO. It is ON only
while the heater relay is commanded to energize, and OFF when the
heater relay is commanded off. It is OFF at startup, when a run stops,
when a timed run reaches 0:00, and when an error turns off the heater.

The heater LED indicates the firmware's heater command. It does not
independently prove that current is flowing through the heating element.
