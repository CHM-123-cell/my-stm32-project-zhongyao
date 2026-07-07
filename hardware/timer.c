/**
 * 系统节拍与毫秒延时 (SPL风格)
 * 依赖 SysTick 1ms 中断递增 g_sys_tick
 * 舵机 PWM 由 servo.c (Servo_Init) 负责, 本文件不重复实现
 */

#include "timer.h"

/* ========== 毫秒延时（SysTick 1ms 节拍） ========== */
/* 前提：系统初始化时已配置 SysTick 为 1ms 中断 */
__IO uint32_t g_sys_tick = 0;  // 系统1ms计数器

void Delay_ms(uint32_t ms)
{
    uint32_t start = g_sys_tick;
    while ((g_sys_tick - start) < ms)
    {
        __WFI();  // 等待中断，降低功耗
    }
}

/* 获取系统tick（供消抖等使用） */
uint32_t SysTick_GetTick(void)
{
    return g_sys_tick;
}
