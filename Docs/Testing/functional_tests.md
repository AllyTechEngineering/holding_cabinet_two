# Functional Test Procedures

## Common Setup

Equipment:

- Powered NUCLEO-L476RG mock-up.
- Connected LCD and Mode, Enter, Up, and Down switches.
- Connected NTC temperature-sensing circuit.
- Connected heater relay PCBA.
- Connected Heater indicator LED.
- Independent timer for elapsed-time measurements when required.

Before testing:

- Record the technician, date, hardware configuration, firmware commit,
  any uncommitted changes, and test procedure revision.
- Begin each test from its stated starting conditions.
- A button press means press and release unless otherwise stated.
- Do not pause the debugger during countdown measurements.
- Unless a test specifically requires powered heating, the heating element
  may remain disconnected while verifying the heater relay command and
  Heater indicator LED.

## FT-001: Temperature Edit Preserves Countdown

Requirements:

- HMI.md: Active-run temperature editing.
- REQUIREMENTS.md: Timed proof operation.

Starting conditions:

- Run Active with a confirmed temperature of 95°F.
- Timed proof started at 0:15 and now displaying 0:14.

Steps:

1. Press Mode to open Set Temp Decision.
2. Press Enter. Verify the proposed temperature is 95°F.
3. Increase the proposal to 100°F.
4. Press Enter to open Set Time Decision.
5. Press Mode to skip time editing.
6. Verify that "Apply Changes?" appears.
7. Press Enter to return to Run Active.
8. Press Mode, then Enter to reopen temperature adjustment.

Expected results:

- The countdown continues without restarting or becoming untimed.
- Reopening temperature adjustment shows 100°F.

Record:

- Countdown immediately before and after applying changes.
- Temperature shown when adjustment is reopened.
- Pass/fail and any deviations.

## FT-002: Apply a New Countdown

Requirements:

- HMI.md: Active-run time editing.
- REQUIREMENTS.md: Timed proof operation.

Starting conditions:

- Run Active with a timed proof displaying 0:10 remaining.

Steps:

1. Press Mode to open Set Temp Decision.
2. Press Mode to skip temperature editing.
3. Press Enter to open time adjustment.
4. Verify the initial proposed duration is 1:00.
5. Set the proposed duration to 0:20.
6. Press Enter to reach "Apply Changes?"
7. Press Enter to apply the proposal.

Expected results:

- Time adjustment initially shows the default 1:00.
- Run Active returns with a new 0:20 countdown.
- The new countdown starts when changes are applied.

Record:

- Initial proposed duration.
- Countdown shown immediately after applying changes.
- Pass/fail and any deviations.

## FT-003: Discard a Time Proposal

Requirements:

- HMI.md: Set Time adjustment and confirmation behavior.
- REQUIREMENTS.md: Timed proof operation.

Starting conditions:

- Run Active with a timed proof displaying 0:10 remaining.

Steps:

1. Press Mode, Mode, then Enter to open time adjustment.
2. Change the proposed duration to 0:30.
3. Press Mode to discard the proposal.
4. At Set Time Decision, press Mode to skip time editing.
5. At "Apply Changes?", press Enter.

Expected results:

- Discarding the proposal returns to Set Time Decision.
- Applying changes returns to Run Active.
- The original countdown continues without restarting.
- The discarded 0:30 proposal does not take effect.

Record:

- Countdown before entering the edit and after returning to Run Active.
- Pass/fail and any deviations.

## FT-004: Cancel to Idle

Requirements:

- HMI.md: Run Decision and active-run cancellation behavior.
- REQUIREMENTS.md: Run stop behavior.

Starting conditions:

- Run Active with a timed proof.

Steps:

1. Press Mode to open Set Temp Decision.
2. Press Mode to skip temperature editing.
3. Press Mode to skip time editing.
4. At "Apply Changes?", press Mode.
5. Observe the Idle screens.
6. Press Mode to begin setup again.
7. Press Enter to open temperature adjustment.
8. Press Enter to retain the temperature.
9. Press Mode to skip time setup.

Expected results:

- Cancellation returns to the alternating Idle screens.
- Beginning setup again follows the new-proof path.
- Run Decision displays "To Start Proof".

Record:

- Screens observed after cancellation and during the new setup.
- Pass/fail and any deviations.

## FT-005: Countdown Expiry Interrupts an Edit

Requirements:

- HMI.md: Countdown expiry and active-run editing behavior.
- REQUIREMENTS.md: Timed proof completion behavior.

Starting conditions:

- Run Active with a timed proof displaying 0:01 remaining.

Steps:

1. Press Mode, Mode, then Enter to open time adjustment.
2. Change the proposal to 0:30.
3. Leave the proposal unconfirmed.
4. Observe the display until the original countdown expires.
5. At "Proof Complete?", press Mode.

Expected results:

- Expiry interrupts time adjustment and opens "Proof Complete?"
- The proposed 0:30 countdown does not start.
- Mode opens Set Temp Decision.

Record:

- Edit screen displayed before expiry.
- Screen displayed at expiry and after pressing Mode.
- Pass/fail and any deviations.

## FT-006: Discard Temperature During an Untimed Proof

Requirements:

- HMI.md: Active-run temperature editing.
- REQUIREMENTS.md: Untimed proof operation.

Starting conditions:

- Run Active with a confirmed temperature of 95°F.
- Countdown timer not used.

Steps:

1. Press Mode, then Enter to open temperature adjustment.
2. Change the proposal to 100°F.
3. Press Mode to discard the temperature proposal.
4. Press Mode to skip time editing.
5. At "Apply Changes?", press Enter.
6. Observe the Run Active display.
7. Press Mode, then Enter to reopen temperature adjustment.

Expected results:

- Run Active alternates "Countdown Timer" and "Not Used".
- Reopening temperature adjustment shows 95°F.
- The discarded 100°F proposal does not take effect.

Record:

- Untimed display behavior.
- Temperature shown when adjustment is reopened.
- Pass/fail and any deviations.

## FT-007: Heater Turns On Below Lower Control Limit

Requirements:

- REQUIREMENTS.md: TEMP-005 — Temperature Regulation.
- REQUIREMENTS.md: HEAT-002 — Heating permitted only during an active run.
- REQUIREMENTS.md: HEAT-004 — Heater indicator follows the heater command.

Starting conditions:

- System is at Idle.
- NTC is connected and reporting a valid temperature.
- Heater relay PCBA and Heater indicator LED are connected.
- Heating element may remain disconnected.

Steps:

1. Note the displayed cabinet temperature.
2. Start an untimed proof.
3. Select a temperature setpoint sufficiently above the measured cabinet
   temperature to place the measured temperature below the lower control
   limit.
4. Confirm the proof and enter Run Active.
5. Allow sufficient time for a new temperature measurement and heater-control
   decision.
6. Observe the heater relay PCBA and Heater indicator LED.

Expected results:

- The system remains in Run Active.
- The heater relay energizes.
- The Heater indicator LED turns on.
- The heater remains commanded on while the measured temperature remains at
  or below the lower control limit.

Record:

- Measured temperature.
- Confirmed temperature setpoint.
- Heater relay state.
- Heater indicator LED state.
- Pass/fail and any deviations.

## FT-008: Heater Remains Off At or Above Setpoint

Requirements:

- REQUIREMENTS.md: TEMP-003 — Setpoint below cabinet temperature.
- REQUIREMENTS.md: TEMP-005 — Temperature Regulation.
- REQUIREMENTS.md: HEAT-004 — Heater indicator follows the heater command.

Starting conditions:

- System is at Idle.
- NTC is connected and reporting a valid temperature.
- Heater relay PCBA and Heater indicator LED are connected.
- Heating element may remain disconnected.

Steps:

1. Note the displayed cabinet temperature.
2. Start an untimed proof.
3. Select a temperature setpoint below the measured cabinet temperature.
4. Confirm the proof and enter Run Active.
5. Allow sufficient time for a new temperature measurement and heater-control
   decision.
6. Observe the heater relay PCBA and Heater indicator LED.

Expected results:

- The system remains in Run Active.
- The heater relay remains de-energized.
- The Heater indicator LED remains off.

Record:

- Measured temperature.
- Confirmed temperature setpoint.
- Heater relay state.
- Heater indicator LED state.
- Pass/fail and any deviations.

## FT-009: Stopping an Active Run Turns Heater Off

Requirements:

- REQUIREMENTS.md: HEAT-003 — Heater off when an active run stops.
- REQUIREMENTS.md: HEAT-004 — Heater indicator follows the heater command.
- HMI.md: Active-run stop behavior.

Starting conditions:

- Run Active with an untimed proof.
- Confirmed setpoint is sufficiently above the measured cabinet temperature
  for the heater to be commanded on.
- Heater relay is energized.
- Heater indicator LED is on.

Steps:

1. Press Mode to enter the active-run edit path.
2. Continue through the edit decisions without applying a new temperature or
   time.
3. At Run Decision, press Mode to stop the active run.
4. Observe the heater relay, Heater indicator LED, and display.

Expected results:

- The heater relay de-energizes.
- The Heater indicator LED turns off.
- The system returns to Idle.
- Heating remains off after returning to Idle.

Record:

- Heater relay state before and after stopping.
- Heater indicator LED state before and after stopping.
- Screen displayed after stopping.
- Pass/fail and any deviations.

## FT-010: Timed Proof Expiry Turns Heater Off

Requirements:

- REQUIREMENTS.md: HEAT-003 — Heater off on timer expiry.
- REQUIREMENTS.md: HEAT-004 — Heater indicator follows the heater command.
- HMI.md: Timed proof completion behavior.

Starting conditions:

- Timed proof is in Run Active.
- Remaining time is short enough to observe expiry during the test.
- Confirmed setpoint is sufficiently above the measured cabinet temperature
  for the heater to be commanded on.
- Heater relay is energized.
- Heater indicator LED is on.

Steps:

1. Allow the countdown to continue without user intervention.
2. Observe the display when the countdown reaches 0:00.
3. Observe the heater relay.
4. Observe the Heater indicator LED.
5. Leave the system at Complete Decision long enough to verify heating does
   not restart.

Expected results:

- Countdown expiry opens Complete Decision.
- The heater relay de-energizes.
- The Heater indicator LED turns off.
- Heating remains off while the system is at Complete Decision.

Record:

- Screen displayed at expiry.
- Heater relay state after expiry.
- Heater indicator LED state after expiry.
- Pass/fail and any deviations.

## FT-011: NTC Fault Turns Heater Off

Requirements:

- REQUIREMENTS.md: TEMP-006 — Measured-temperature validity and NTC faults.
- REQUIREMENTS.md: HEAT-003 — Heater off on a firmware-detected condition
  that prohibits heating.
- REQUIREMENTS.md: HEAT-004 — Heater indicator follows the heater command.

Starting conditions:

- Run Active with an untimed proof.
- NTC is initially connected and reporting a valid temperature.
- Confirmed setpoint is sufficiently above the measured cabinet temperature
  for the heater to be commanded on.
- Heater relay is energized.
- Heater indicator LED is on.
- Heating element may remain disconnected.

Steps:

1. Disconnect the NTC to create an open-circuit condition.
2. Observe the heater relay and Heater indicator LED.
3. Restore the NTC connection and verify a valid temperature is again
   available before continuing.
4. Establish Run Active with the heater commanded on.
5. Short the NTC input to create the defined NTC short-circuit condition.
6. Observe the heater relay and Heater indicator LED.

Expected results:

- A detected NTC open-circuit fault causes the heater relay to de-energize.
- A detected NTC open-circuit fault causes the Heater indicator LED to turn
  off.
- A detected NTC short-circuit fault causes the heater relay to de-energize.
- A detected NTC short-circuit fault causes the Heater indicator LED to turn
  off.
- The firmware does not command heating while either detected NTC fault is
  active.

Record:

- Heater relay and Heater indicator LED response to the open-circuit test.
- Heater relay and Heater indicator LED response to the short-circuit test.
- Any observed instability or other behavior of the temperature measurement
  during the open-circuit test.
- Pass/fail and any deviations.