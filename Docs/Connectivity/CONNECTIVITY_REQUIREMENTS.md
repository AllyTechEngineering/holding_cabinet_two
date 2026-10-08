# Connectivity Subsystem Requirements

## 1. Purpose

This document defines the behavioral requirements of the Holding Cabinet
connectivity subsystem.

Connectivity subsystem requirements are derived from and traceable to
system-level requirements in `../REQUIREMENTS.md`.

Requirements shall not be duplicated between the system and subsystem
requirement documents.


## 2. Requirement Conventions

Each connectivity subsystem requirement shall have a unique identifier.

Each connectivity subsystem requirement shall identify its applicable parent
system requirement or requirements.

The following terms are used for unresolved items:

- **TBD (To Be Determined):** The requirement or value has not been defined.
- **TBC (To Be Confirmed):** The requirement or value requires confirmation.


## 3. ESP32 Device Management Requirements

### ESP-001 — ESP32 Communication Establishment

Parent: `CONN-001`

The connectivity subsystem shall establish valid ESP-AT communication with
the ESP32-C6 during initialization.


### ESP-002 — ESP32 Communication Monitoring

Parent: `CONN-001`, `CONN-002`

The connectivity subsystem shall detect loss of established ESP-AT
communication with the ESP32-C6.


### ESP-003 — ESP32 Communication Failure

Parent: `CONN-001`, `CONN-002`

Failure to establish or maintain valid ESP-AT communication with the
ESP32-C6 shall be reported as an ESP32 communication failure.


### ESP-004 — Connectivity Service Availability

Parent: `CONN-001`

The connectivity subsystem shall not be considered operational until valid
ESP-AT communication with the ESP32-C6 has been established.


### ESP-005 — Communication Health Verification

Parent: `CONN-001`, `CONN-002`

During normal operation, ESP32 communication health shall be determined from
ESP-AT operations that require a response.

Periodic ESP32 heartbeat commands are not required.


## 4. Provisioning Requirements

### PROV-001 — Provisioning Initiation

Parent: `SETTINGS-003`, `CONN-003`

Wi-Fi provisioning shall begin only after the user selects and confirms the
Wi-Fi configuration option from Settings.


### PROV-002 — Provisioning Interface

Parent: `CONN-003`

Wi-Fi provisioning data shall be received from the mobile application through
a BLE provisioning connection.


### PROV-003 — Provisioning Data Entry

Parent: `CONN-003`

Wi-Fi network selection and password entry shall be performed by the mobile
application.

The cabinet HMI shall not provide SSID or Wi-Fi password entry.


### PROV-004 — Provisioning Duration

Parent: `SETTINGS-003`, `CONN-003`

An active provisioning session shall remain available for up to 5 minutes.


### PROV-005 — Provisioning Success

Parent: `CONN-003`, `CONN-007`

Wi-Fi provisioning shall be considered successful when the ESP32-C6
successfully connects to the selected Wi-Fi network and obtains network
connectivity.

Internet access and Firebase access shall not be required to declare Wi-Fi
provisioning successful.


### PROV-006 — Successful Provisioning Completion

Parent: `CONN-003`, `CONN-005`, `PERSIST-005`

After successful Wi-Fi provisioning:

- The new Wi-Fi configuration shall become the active configuration.
- The ESP32-C6 shall retain the accepted Wi-Fi credentials.
- BLE provisioning shall end.
- Normal connectivity operation shall resume.
- A reboot shall not be required.


### PROV-007 — Failed Credential Validation

Parent: `CONN-003`, `CONN-006`, `PERSIST-005`

If the cabinet cannot connect to the selected Wi-Fi network using candidate
credentials:

- The candidate credentials shall not replace the existing stored Wi-Fi
  configuration.
- The provisioning session shall remain active while the 5-minute provisioning
  window remains valid.
- The provisioning client shall be able to provide corrected credentials.


### PROV-008 — Existing Credential Retention

Parent: `CONN-006`, `PERSIST-005`

Starting Wi-Fi provisioning shall not erase the currently stored Wi-Fi
credentials.

Existing stored credentials shall remain valid until a new Wi-Fi
configuration has been successfully provisioned.


### PROV-009 — Provisioning Cancellation

Parent: `SETTINGS-003`, `CONN-006`, `PERSIST-005`

The user shall be able to cancel an active Wi-Fi provisioning session from
the cabinet HMI.

When provisioning is cancelled:

- BLE provisioning shall stop.
- Unaccepted candidate credentials shall be discarded.
- Existing stored Wi-Fi credentials shall be retained.
- The firmware shall return to Settings.


### PROV-010 — Provisioning Timeout

Parent: `SETTINGS-003`, `CONN-003`, `PERSIST-005`

If the 5-minute provisioning window expires before successful completion:

- BLE provisioning shall stop.
- Unaccepted candidate credentials shall be discarded.
- Existing stored Wi-Fi credentials shall be retained.
- The firmware shall return to Settings.

Provisioning timeout shall not be treated as a cabinet fault.


### PROV-011 — Initial and Replacement Configuration

Parent: `CONN-003`, `CONN-006`

The same provisioning process shall support both initial Wi-Fi configuration
and later replacement of an existing Wi-Fi configuration.


## 5. Wi-Fi Connectivity Requirements

### WIFI-001 — Stored Credential Use

Parent: `CONN-005`, `PERSIST-005`

The ESP32-C6 shall retain accepted Wi-Fi credentials and use them when
establishing normal Wi-Fi connectivity.


### WIFI-002 — Wi-Fi Connection Failure

Parent: `CONN-002`

Failure to connect to the configured Wi-Fi network shall not prevent normal
local cabinet operation.


### WIFI-003 — Wi-Fi Connection Loss

Parent: `CONN-002`

Loss of an established Wi-Fi connection shall not prevent normal local
cabinet operation.


## 6. Network Connectivity Requirements

### NET-001 — Local Operation Without Internet Connectivity

Parent: `CONN-002`

Loss or absence of Internet connectivity shall not prevent normal local
cabinet operation.


## 7. Firebase Communications Requirements

### FIREBASE-001 — Remote Service Independence

Parent: `CONN-002`

Loss or absence of Firebase connectivity shall not prevent normal local
cabinet operation.


## 8. Cabinet Data Reporting Requirements

TBD.


## 9. Remote Command Requirements

TBD.


## 10. Connectivity State and Status Requirements

### STATUS-001 — Provisioning Result

Parent: `CONN-007`

The connectivity subsystem shall provide the cabinet HMI with the result of
Wi-Fi provisioning.

The result shall distinguish successful provisioning from unsuccessful
provisioning.


### STATUS-002 — Connectivity Independence

Parent: `CONN-002`

Wi-Fi, Internet, or Firebase unavailability shall be represented as
connectivity status conditions and shall not by themselves place the cabinet
into the Error state.


## 11. Credentials and Security Requirements

### CRED-001 — Wi-Fi Credential Storage

Parent: `PERSIST-005`, `CONN-005`

Accepted Wi-Fi credentials shall be stored persistently by the ESP32-C6.


### CRED-002 — Wi-Fi Credential Provisioning

Parent: `CONN-003`, `CONN-006`

Wi-Fi credentials shall be provided to the ESP32-C6 through the cabinet
provisioning process.


## 12. Fault Handling and Recovery Requirements

### RECOVERY-001 — ESP32 Communication Fault

Parent: `CONN-001`, `CONN-002`, `FAULT-007`

Loss of valid ESP-AT communication with the ESP32-C6 shall be handled as an
ESP32 communication failure.

Loss of ESP32 communication shall not prevent normal local cabinet operation.


### RECOVERY-002 — Non-Fault Connectivity Failures

Parent: `CONN-002`, `FAULT-007`

Wi-Fi connection failure, Internet unavailability, and Firebase
unavailability shall not by themselves be treated as cabinet faults.