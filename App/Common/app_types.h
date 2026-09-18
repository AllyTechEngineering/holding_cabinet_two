/**
  ******************************************************************************
  * @file           : app_types.h
  * @brief          : Shared application-level structs, enums, and typedefs
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  ******************************************************************************
  */

#ifndef __APP_TYPES_H
#define __APP_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * qInputToDisplay payload: InputTask -> DisplayTask
 * ============================================================
 * CubeMX generated this queue's item size as sizeof(uint8_t) (Item Size
 * field = "uint8_t"), so this type MUST be exactly 1 byte. A plain C
 * enum is NOT safe here -- its size is compiler-defined (commonly 4
 * bytes) and would silently truncate when copied into the queue.
 * Using #define constants over a uint8_t typedef instead avoids that
 * entirely. */
typedef uint8_t ButtonEvent_t;

#define EVT_NONE              0u
#define EVT_UP_PRESSED        1u
#define EVT_UP_RELEASED       2u
#define EVT_DOWN_PRESSED      3u
#define EVT_DOWN_RELEASED     4u
#define EVT_MODE_PRESSED      5u
#define EVT_MODE_RELEASED     6u
#define EVT_ENTER_PRESSED     7u
#define EVT_ENTER_RELEASED    8u
#define EVT_ENTER_SETTINGS    9u   /* Up+Down held 5s, idle-only -- see Docs/states_modes.md */

/* ============================================================
 * qDisplayToHeat payload: DisplayTask -> HeatTask
 * ============================================================
 * CubeMX generated this queue's item size as sizeof(HeatCommand_t), so
 * (unlike ButtonEvent_t above) there's no manual size constraint here --
 * whatever this struct's real size is, the queue was created to match. */
typedef enum {
    HEAT_CMD_SET_SETPOINT = 0,  /* setpointTenthsC is valid */
    HEAT_CMD_RUN,                 /* start bang-bang control at the last-set setpoint */
    HEAT_CMD_STOP,                 /* stop control, relay off */
} HeatCommandType_t;

typedef struct {
    HeatCommandType_t type;
    uint16_t          setpointTenthsC;  /* tenths of degC; valid for HEAT_CMD_SET_SETPOINT */
} HeatCommand_t;

/* ============================================================
 * qHeatToDisplay payload: HeatTask -> DisplayTask
 * ============================================================ */
typedef struct {
    uint16_t currentTempTenthsC;  /* tenths of degC; see App/Sensing/thermistor_driver.h
                                    * THERMISTOR_FAULT_* sentinels for the NTC-fault case */
    uint8_t  relayOn;              /* 1 = heat relay energized, 0 = off */
    uint8_t  errorCode;            /* 0 = no error; else see Docs/error_codes.md
                                     * (10/11 = NTC Open/Short, 20/21 = Heater Open/Short) */
} HeatStatus_t;

#ifdef __cplusplus
}
#endif

#endif /* __APP_TYPES_H */