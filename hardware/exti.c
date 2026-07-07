/**
 * E3F-5C1 NPN光电传感器驱动
 * PB0=上光电 EXTI0, PB1=下光电 EXTI1
 * PC817隔离 → STM32内部上拉 → 遮挡时拉低(下降沿触发)
 * 直接计数,不做软件消抖
 */
#include "exti.h"

volatile uint16_t upper_cnt = 0;
volatile uint16_t lower_cnt = 0;
volatile uint8_t upper_triggered = 0;
volatile uint8_t lower_triggered = 0;

void PhotoSensor_Init(void)
{
    GPIO_InitTypeDef g;
    EXTI_InitTypeDef e;
    NVIC_InitTypeDef n;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);

    /* PB0/PB1 上拉输入(PC817未导通时保持高电平) */
    g.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
    g.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_Init(GPIOB,&g);

    /* AFIO映射 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

    /* EXTI0 下降沿 */
    e.EXTI_Line=EXTI_Line0;e.EXTI_Mode=EXTI_Mode_Interrupt;
    e.EXTI_Trigger=EXTI_Trigger_Falling;e.EXTI_LineCmd=ENABLE;
    EXTI_Init(&e);
    /* EXTI1 */
    e.EXTI_Line=EXTI_Line1;
    EXTI_Init(&e);

    /* NVIC */
    n.NVIC_IRQChannel=EXTI0_IRQn;n.NVIC_IRQChannelPreemptionPriority=1;
    n.NVIC_IRQChannelSubPriority=0;n.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&n);
    n.NVIC_IRQChannel=EXTI1_IRQn;
    NVIC_Init(&n);
}

/* 中断直接计数(E3F-5C1信号足够干净,不需要软件消抖) */
void EXTI0_IRQHandler(void){
    if(EXTI_GetITStatus(EXTI_Line0)!=RESET){
        EXTI_ClearITPendingBit(EXTI_Line0);
        upper_cnt++;
    }
}
void EXTI1_IRQHandler(void){
    if(EXTI_GetITStatus(EXTI_Line1)!=RESET){
        EXTI_ClearITPendingBit(EXTI_Line1);
        lower_cnt++;
    }
}

/* 保留空函数供main.c调用(保持接口兼容) */
void PhotoSensor_Poll(void){}
