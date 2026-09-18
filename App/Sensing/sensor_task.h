/**
  ******************************************************************************
  * @file           : sensor_task.h
  * @brief          : Header for sensor_task.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  ******************************************************************************
  */

#ifndef __SENSOR_TASK_H
#define __SENSOR_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Real SenseTask body. main.c's CubeMX-generated StartSenseTask() (which
 * owns the task's registered signature) just delegates here -- see the
 * USER CODE section in Core/Src/main.c. */
void SenseTask_Run(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_TASK_H */