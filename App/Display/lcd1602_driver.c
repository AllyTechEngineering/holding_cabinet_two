/********************************************************************************
  * @file           : lcd1602_driver.c
  * @brief          : LCD1602 character display driver via PCF8574 I2C backpack
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 AllyTech LLC.
  * All rights reserved.
  *
  *******************************************************************************/

#include "lcd1602_driver.h"
#include "main.h"

#define LCD_I2C_ADDR   (0x27u << 1)
#define LCD_BACKLIGHT  0x08u
#define LCD_ENABLE_BIT 0x04u
#define LCD_RS_BIT     0x01u

extern I2C_HandleTypeDef hi2c1;

static void lcd_i2c_write(uint8_t data)
{
  if (HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDR,
                              &data, 1u, 10u) != HAL_OK)
  {
    /* An unusable display must not leave the heater energized. */
    HAL_GPIO_WritePin(HeatRelay_GPIO_Port, HeatRelay_Pin, GPIO_PIN_SET);
    Error_Handler();
  }
}

static void lcd_pulse_enable(uint8_t data)
{
  lcd_i2c_write(data | LCD_ENABLE_BIT);
  HAL_Delay(1);
  lcd_i2c_write(data & (uint8_t)~LCD_ENABLE_BIT);
  HAL_Delay(1);
}

static void lcd_write4(uint8_t nibble, uint8_t rs)
{
  uint8_t data = (uint8_t)((nibble << 4) | LCD_BACKLIGHT | rs);

  lcd_i2c_write(data);
  lcd_pulse_enable(data);
}

static void lcd_send(uint8_t value, uint8_t rs)
{
  lcd_write4((uint8_t)(value >> 4), rs);
  lcd_write4((uint8_t)(value & 0x0Fu), rs);
}

static void lcd_command(uint8_t command)
{
  lcd_send(command, 0u);
}

static void lcd_data(uint8_t character)
{
  lcd_send(character, LCD_RS_BIT);
}

static void lcd_set_cursor(uint8_t row)
{
  lcd_command((uint8_t)(0x80u | (row == 0u ? 0x00u : 0x40u)));
}

void LCD1602_Init(void)
{
  HAL_Delay(50);

  lcd_write4(0x03u, 0u);
  HAL_Delay(5);
  lcd_write4(0x03u, 0u);
  HAL_Delay(1);
  lcd_write4(0x03u, 0u);
  HAL_Delay(1);
  lcd_write4(0x02u, 0u);
  HAL_Delay(1);

  lcd_command(0x28u); /* 4-bit interface, two rows */
  lcd_command(0x0Cu); /* Display on, cursor off */
  lcd_command(0x06u); /* Advance cursor after each character */
  lcd_command(0x01u); /* Clear display */
  HAL_Delay(2);
}

void LCD1602_WriteLines(const char *row1, const char *row2)
{
  lcd_set_cursor(0u);
  for (uint8_t col = 0u; col < 16u; ++col)
  {
    lcd_data((uint8_t)row1[col]);
  }

  lcd_set_cursor(1u);
  for (uint8_t col = 0u; col < 16u; ++col)
  {
    lcd_data((uint8_t)row2[col]);
  }
}