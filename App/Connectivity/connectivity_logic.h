/********************************************************************************
 * @file           : connectivity_logic.h
 * @brief          : Connectivity logic definitions
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 AllyTech LLC.
 * All rights reserved.
 ********************************************************************************/

#ifndef __CONNECTIVITY_LOGIC_H
#define __CONNECTIVITY_LOGIC_H

#include "esp_at_interface.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t ConnectivityLogic_Start(void);
void ConnectivityLogic_Process(void);
uint8_t ConnectivityLogic_IsEspAtInitialized(void);
void ConnectivityLogic_HandleUrc(EspAtUrcType urc);

#ifdef __cplusplus
}
#endif

#endif /* __CONNECTIVITY_LOGIC_H */