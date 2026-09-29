# Functional Test Results

## Test Execution — 2026-09-29

### Test Information

Technician: Bob Taylor

Date: 2026-09-29

Hardware configuration:

- Powered NUCLEO-L476RG development mock-up.
- LCD and Mode, Enter, Up, and Down switches connected.
- NTC temperature-sensing circuit connected.
- Heater relay PCBA connected.
- Heater indicator LED connected.
- Heating element disconnected.

Firmware:

- Heater-control firmware implementing whole-degree Celsius internal
  temperature control.
- On/off heater control with 2°C hysteresis.
- Heater relay is active-low.
- Heater indicator LED follows the firmware heater command.
- Firmware commit: heater-control implementation committed before testing;
  record the exact commit hash from Git history if required for traceability.

Test procedure revision:

- Heater-control tests were executed interactively during firmware
  development.
- FT-007 through FT-011 were documented immediately afterward from the
  procedures actually executed.
- The test-procedure commit therefore postdates this test execution.
- No claim is made that FT-007 through FT-011 existed as committed procedures
  before the tests were performed.

## Results Summary

| Test | Description | Result |
| --- | --- | --- |
| FT-007 | Heater Turns On Below Lower Control Limit | PASS |
| FT-008 | Heater Remains Off At or Above Setpoint | PASS |
| FT-009 | Stopping an Active Run Turns Heater Off | PASS |
| FT-010 | Timed Proof Expiry Turns Heater Off | PASS |
| FT-011 | NTC Fault Turns Heater Off | PASS |

## FT-007: Heater Turns On Below Lower Control Limit

Result: PASS

Observations:

- An untimed proof was started with the confirmed temperature setpoint
  sufficiently above the measured cabinet temperature.
- The system entered Run Active normally.
- The heater relay energized.
- The Heater indicator LED turned on.
- Relay and Heater indicator behavior matched the expected heater command.

Deviations:

- None observed.

## FT-008: Heater Remains Off At or Above Setpoint

Result: PASS

Observations:

- An untimed proof was started with the confirmed temperature setpoint below
  the measured cabinet temperature.
- The system entered Run Active normally.
- The heater relay remained de-energized.
- The Heater indicator LED remained off.

Deviations:

- None observed.

## FT-009: Stopping an Active Run Turns Heater Off

Result: PASS

Observations:

- Testing began with an active untimed proof and the heater commanded on.
- The normal HMI run-edit path was entered.
- Mode was used at Run Decision to stop the active run.
- The heater relay de-energized.
- The Heater indicator LED turned off.
- The system returned to Idle.
- Heating remained off after returning to Idle.

Deviations:

- None observed.

## FT-010: Timed Proof Expiry Turns Heater Off

Result: PASS

Observations:

- A short timed proof was operated with the heater commanded on.
- The countdown was allowed to expire normally.
- At 0:00 the system transitioned to Complete Decision.
- The heater relay de-energized.
- The Heater indicator LED turned off.
- Heating remained off at Complete Decision.

Deviations:

- None observed.

## FT-011: NTC Fault Turns Heater Off

Result: PASS

Open-circuit observations:

- Testing began with an active proof and the heater commanded on.
- The NTC was disconnected.
- The heater relay turned off.
- The Heater indicator LED turned off.
- The disconnected ADC input appeared susceptible to electrical noise.
- The observed ADC behavior is a hardware characteristic and does not change
  the firmware test result: when the firmware detected the NTC fault, heating
  was inhibited.

Short-circuit observations:

- The NTC short-circuit condition was tested.
- The heater relay turned off as expected.
- The Heater indicator LED turned off as expected.
- No abnormal firmware behavior was observed during the short-circuit test.

Deviations:

- The open-circuit ADC input appeared susceptible to noise when the NTC was
  physically disconnected.
- Hardware design and hardware fault-detection circuitry are outside the
  authority of this firmware repository and are maintained in the separate
  hardware project.

## Overall Result

PASS

The normal heater-control use cases exercised during this test session behaved
as expected.

Verified behavior included:

- Heater ON when the measured temperature was below the lower control limit.
- Heater OFF when the measured temperature was above the confirmed setpoint.
- Heater OFF when an active proof was stopped.
- Heater OFF when a timed proof expired.
- Heater OFF when an NTC open-circuit fault was detected.
- Heater OFF when an NTC short-circuit fault was detected.
- Heater indicator LED state followed the firmware heater command during the
  tested operating conditions.

The heating element was disconnected during these tests. These results verify
firmware command behavior, relay operation, Heater indicator behavior, and
fault response on the development mock-up. They do not constitute thermal
performance or temperature-regulation characterization of a completed cabinet.