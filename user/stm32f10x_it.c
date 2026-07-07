/**
 * 中断服务函数 (SPL风格)
 * SysTick_Handler 中递增系统计数器
 * 光电中断在 exti.c，串口中断在 usart.c
 */

#include "stm32f10x_it.h"
#include "timer.h"

void NMI_Handler(void) {}
void HardFault_Handler(void) { while (1); }
void MemManage_Handler(void) { while (1); }
void BusFault_Handler(void) { while (1); }
void UsageFault_Handler(void) { while (1); }
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}

/**
 * @brief  SysTick 1ms 中断 → 递增系统计数器
 */
void SysTick_Handler(void)
{
    g_sys_tick++;
}
