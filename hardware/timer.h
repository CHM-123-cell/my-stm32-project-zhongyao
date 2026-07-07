/**
 * TIM2 舵机 PWM + 系统延时 (SPL风格)
 * PA0 = TIM2_CH1 → 上舵机
 * PA1 = TIM2_CH2 → 下舵机
 */

#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void Delay_ms(uint32_t ms);
uint32_t SysTick_GetTick(void);

extern __IO uint32_t g_sys_tick;

#endif
