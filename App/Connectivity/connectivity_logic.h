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

#ifdef __cplusplus
extern "C" {
#endif

void ConnectivityLogic_HandleUrc(EspAtUrcType urc);

#ifdef __cplusplus
}
#endif

#endif /* __CONNECTIVITY_LOGIC_H */