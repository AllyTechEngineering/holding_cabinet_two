# Functional Test Procedures

## Common Setup

Equipment:
- Powered NUCLEO-L476RG mock-up.
- Connected LCD and Mode, Enter, Up, and Down switches.
- Independent timer for elapsed-time measurements.

Before testing:
- Record the technician, date, hardware configuration, firmware commit,
  any uncommitted changes, and test procedure revision.
- Begin each test from its stated starting conditions.
- A button press means press and release unless otherwise stated.
- Do not pause the debugger during countdown measurements.

## FT-001: Temperature Edit Preserves Countdown

Requirement:
- ../states_modes.md: Heat control lifecycle.
- ../set_temp_mode.md: Editing an active run.

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

Requirement:
- ../set_time_mode.md: Editing an active run.
- ../run_mode.md: Run-Decision.

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

Requirement:
- ../states_modes.md: SetTime-Adjust and SetTime-Confirm.
- ../set_time_mode.md: Editing an active run.

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

Requirement:
- ../states_modes.md: Run-Decision.

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

Requirement:
- ../states_modes.md: Automatic screen changes.

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

Requirement:
- ../set_temp_mode.md: Editing an active run.
- ../run_mode.md: Run-Active (untimed variant).

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