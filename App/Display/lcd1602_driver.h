/**
  ******************************************************************************
  * @file           : lcd1602_driver.h
  * @brief          : Header for lcd1602_driver.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  ******************************************************************************
  */

#ifndef __LCD1602_DRIVER_H
#define __LCD1602_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Call once from DisplayTask before writing either row. */
void LCD1602_Init(void);

/* Each string must contain exactly 16 characters. */
void LCD1602_WriteLines(const char *row1, const char *row2);

#ifdef __cplusplus
}
#endif

#endif /* __LCD1602_DRIVER_H */