# Audible Alert

The system has one fixed-frequency buzzer controlled by an active-high
GPIO. GPIO HIGH sounds the buzzer; GPIO LOW turns it off.

On each entry to `Complete-Decision`, sound one sequence of three beeps.
This applies whether the user entered `Complete-Decision` manually or
the countdown reached 0:00. The sequence is triggered by entering the
state, not by each display refresh.

- Duration of each beep: 200ms
- Silence between beeps: 300ms

The buzzer is off at power-up. No other state currently requests an
audible alert.