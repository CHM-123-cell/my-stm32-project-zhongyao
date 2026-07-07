/**
 * 光电传感器 — EXTI下降沿 + 60ms消抖
 */
#include "sensor.h"
#include "timer.h"  /* g_sys_tick */

#define SENSOR_DEBOUNCE_MS 60    /* 计数消抖窗口(ms): 防振铃误计; 按药袋下落节拍标定.
                                     下限勿<20ms(单次遮挡振铃), 上限勿>药袋最小间隔(否则漏计) */

volatile uint16_t Sensor_Up=0, Sensor_Lo=0;
static volatile uint32_t dbU=0, dbL=0;

void Sensor_Init(void){
    GPIO_InitTypeDef g;EXTI_InitTypeDef e;NVIC_InitTypeDef n;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
    g.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;g.GPIO_Mode=GPIO_Mode_IPU;GPIO_Init(GPIOB,&g);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
    e.EXTI_Line=EXTI_Line0;e.EXTI_Mode=EXTI_Mode_Interrupt;e.EXTI_Trigger=EXTI_Trigger_Falling;e.EXTI_LineCmd=ENABLE;EXTI_Init(&e);
    e.EXTI_Line=EXTI_Line1;EXTI_Init(&e);
    EXTI->PR=EXTI_Line0|EXTI_Line1;
    n.NVIC_IRQChannel=EXTI0_IRQn;n.NVIC_IRQChannelPreemptionPriority=1;n.NVIC_IRQChannelSubPriority=0;n.NVIC_IRQChannelCmd=ENABLE;NVIC_Init(&n);
    n.NVIC_IRQChannel=EXTI1_IRQn;NVIC_Init(&n);
}

void EXTI0_IRQHandler(void){
    if((EXTI->PR & EXTI_Line0) && g_sys_tick>dbU){EXTI->PR=EXTI_Line0;dbU=g_sys_tick+SENSOR_DEBOUNCE_MS;Sensor_Up++;}
    else EXTI->PR=EXTI_Line0;
}
void EXTI1_IRQHandler(void){
    if((EXTI->PR & EXTI_Line1) && g_sys_tick>dbL){EXTI->PR=EXTI_Line1;dbL=g_sys_tick+SENSOR_DEBOUNCE_MS;Sensor_Lo++;}
    else EXTI->PR=EXTI_Line1;
}
