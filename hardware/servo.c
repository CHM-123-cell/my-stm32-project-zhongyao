/**
 * 舵机驱动 — TIM2硬件PWM, 50Hz, 500~2500us
 */
#include "servo.h"
void Servo_Init(void){
    GPIO_InitTypeDef g;TIM_TimeBaseInitTypeDef tb;TIM_OCInitTypeDef oc;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    g.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;g.GPIO_Mode=GPIO_Mode_AF_PP;g.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&g);
    tb.TIM_Prescaler=7;tb.TIM_Period=19999;tb.TIM_ClockDivision=0;tb.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2,&tb);
    oc.TIM_OCMode=TIM_OCMode_PWM1;oc.TIM_OutputState=TIM_OutputState_Enable;
    oc.TIM_OCPolarity=TIM_OCPolarity_High;oc.TIM_Pulse=1500;
    TIM_OC1Init(TIM2,&oc);TIM_OC2Init(TIM2,&oc);
    TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2,ENABLE);TIM_Cmd(TIM2,ENABLE);
}
void Servo_Up(uint8_t deg){deg=deg>180?180:deg;TIM_SetCompare1(TIM2,500+(uint32_t)deg*2000/180);}
void Servo_Lo(uint8_t deg){deg=deg>180?180:deg;TIM_SetCompare2(TIM2,500+(uint32_t)deg*2000/180);}
