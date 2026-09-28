# Contributing

## Commit Messages

This project uses the Conventional Commits 1.0.0 specification:

https://www.conventionalcommits.org/en/v1.0.0/

Use the following format:

<type>(<scope>): <description>

Common types used by this project:

- `feat`: Add or change functionality
- `fix`: Correct a defect
- `docs`: Documentation-only changes
- `refactor`: Code changes that do not add functionality or fix a defect
- `test`: Add or modify tests
- `build`: Changes affecting the build system
- `chore`: Maintenance work that does not modify product functionality

Examples:

feat(control): add heater command interface

fix(display): correct inactivity timeout handling

docs(requirements): update maximum proofing time

refactor(display): simplify run edit state handling

test(control): add heater hysteresis functional tests

build(cmake): add control task source file