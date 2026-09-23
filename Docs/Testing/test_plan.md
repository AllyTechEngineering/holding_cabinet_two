# Hot Mock-Up Functional Test Plan

## Purpose

Verify firmware behavior on the powered hardware mock-up using
written procedures that a technician can execute and report against.

## Documents

- test_plan.md: Defines the testing approach.
- functional_tests.md: Defines what to test, how to test it, and
  the expected results.
- test_results.md: Records execution results for an identified build.

## Test Procedure Requirements

Each test includes:

- Test ID and title.
- Reference to the applicable requirement.
- Equipment and starting conditions.
- Numbered test steps.
- Expected results.
- Observations or measurements to record.

Expected behavior comes from the project requirements. Resolve
missing or conflicting requirements before judging the affected test.

## Test Execution

Record the technician, date, hardware configuration, firmware commit,
any uncommitted changes, and test procedure revision.

Follow the procedure and record actual observations.

Verify each claimed result using an appropriate method. A display
change alone does not prove an internal state or physical output changed.

For timing tests, specify the measurement method and acceptance
criteria before execution. Do not pause the debugger during measurement.

## Results

Use these statuses:

- PASS: All required checks meet the expected results.
- FAIL: An observed result does not meet the expected result.
- BLOCKED: A prerequisite or unresolved requirement prevents testing.
- NOT RUN: The test has not been executed.

Record deviations and supporting evidence. Preserve previous results
when retesting.

## Regression Testing

After firmware changes, repeat the tests covering the changed behavior
and other functions that could be affected.