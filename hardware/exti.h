/**
 * 外部中断驱动 (SPL风格)
 * PB0 = 上光电 EXTI0, PB1 = 下光电 EXTI1
 * 下降沿触发, 中断只设标志, 主循环中消抖
 */

#ifndef __EXTI_H
#define __EXTI_H

#include "stm32f10x.h"

void PhotoSensor_Init(void);
void PhotoSensor_Poll(void);

/* 计数变量 */
extern volatile uint16_t upper_cnt;
extern volatile uint16_t lower_cnt;

/* 触发标志 */
extern volatile uint8_t upper_triggered;
extern volatile uint8_t lower_triggered;

#endif
