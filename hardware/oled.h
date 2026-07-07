/**
 * OLED SSD1306 0.96寸 I2C 驱动 (软件I2C)
 * PB8 = SCL, PB9 = SDA
 * 基于 OLED驱动函数模块 4针脚I2C版本 适配
 */

#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);

#endif
