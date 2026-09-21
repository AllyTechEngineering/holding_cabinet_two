/********************************************************************************
  * @file           : switch_driver.h
  * @brief          : Header for switch_driver.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  *******************************************************************************/
#ifndef __SWITCH_DRIVER_H
#define __SWITCH_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SWITCH_UP_MASK     (1u << 0)
#define SWITCH_DOWN_MASK   (1u << 1)
#define SWITCH_MODE_MASK   (1u << 2)
#define SWITCH_ENTER_MASK  (1u << 3)

typedef struct
{
  uint8_t pressed_mask;   /* Switches currently held after debounce */
  uint8_t pressed_edges;  /* Switches newly pressed on this poll */
  uint8_t released_edges; /* Switches newly released on this poll */
} SwitchSnapshot_t;

void SwitchDriver_Init(void);
SwitchSnapshot_t SwitchDriver_Poll(void);

#ifdef __cplusplus
}
#endif

#endif /* __SWITCH_DRIVER_H */