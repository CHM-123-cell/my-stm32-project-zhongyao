/**
 * TIM2 PWM 驱动 + 舵机控制
 * PA0 = TIM2_CH1 → 上舵机
 * PA1 = TIM2_CH2 → 下舵机
 * 频率 50Hz, 周期 20ms
 * MG996R 角度对应脉宽：0°≈0.5ms, 90°≈1.5ms, 180°≈2.5ms
 */

#include "timer.h"

/* ========== TIM2 PWM 初始化 ========== */
void TIM2_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* PA0/PA1 推挽复用输出 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 时基: 自适应 SystemCoreClock, 目标1MHz计数 (1us步进) */
    /* APB1定时器时钟始终等于HCLK (分频加倍效应抵消) */
    /* ARR=20000 → 20ms周期 → 50Hz */
    TIM_TimeBaseStructure.TIM_Prescaler         = (uint16_t)(SystemCoreClock / 1000000) - 1;
    TIM_TimeBaseStructure.TIM_Period            = 20000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision     = 0;
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* CH1 (PA0) PWM1 模式 */
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = 1500;  // 默认 1.5ms = 90°
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;  	//计数低于比较值输出高
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    /* CH2 (PA1) PWM1 模式 */
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    /* 预装载 */
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
}

/* ========== 上舵机角度 ========== */
/* 0°≈500us, 90°≈1500us, 180°≈2500us */
void SERVO_Upper_SetAngle(uint8_t angle)
{
    uint16_t pulse;
    if (angle > 180) angle = 180;
    pulse = 500 + (uint32_t)angle * 2000 / 180;  // 500~2500us
    TIM_SetCompare1(TIM2, pulse);
}

/* ========== 下舵机角度 ========== */
void SERVO_Lower_SetAngle(uint8_t angle)
{
    uint16_t pulse;
    if (angle > 180) angle = 180;
    pulse = 500 + (uint32_t)angle * 2000 / 180;
    TIM_SetCompare2(TIM2, pulse);
}

/* ========== 毫秒延时（SysTick 1ms 节拍） ========== */
/* 前提：系统初始化时已配置 SysTick 为 1ms 中断 */
/* SysTick_GetTick() 需在 system_stm32f10x.c 或 main.c 中实现 */
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
