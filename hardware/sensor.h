/**
 * 光电传感器 — PB0=上(EXTI0) PB1=下(EXTI1), 下降沿触发
 * 80ms硬件消抖, 直接读取: extern volatile uint16_t Sensor_Up, Sensor_Lo
 */
#ifndef __SENSOR_H
#define __SENSOR_H
#include "stm32f10x.h"
extern volatile uint16_t Sensor_Up, Sensor_Lo;
void Sensor_Init(void);
#endif
