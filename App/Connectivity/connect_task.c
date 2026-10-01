/********************************************************************************
 * @file           : connect_task.c
 * @brief          : Connectivity task implementation
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 AllyTech LLC.
 * All rights reserved.
 ********************************************************************************/

#include "connect_task.h"
#include "cmsis_os.h"

void ConnectTask_Run(void *argument)
{
  (void)argument;

  for (;;) {
    osDelay(1);
  }
}