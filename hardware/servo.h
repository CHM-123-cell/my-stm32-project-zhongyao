/**
 * 舵机驱动 — TIM2_CH1=PA0 CH2=PA1, 50Hz PWM
 * 使用: Servo_Init() → Servo_Up(angle) / Servo_Lo(angle)
 */
#ifndef __SERVO_H
#define __SERVO_H
#include "stm32f10x.h"
void Servo_Init(void);
void Servo_Up(uint8_t deg);   /* 0°=开 90°=闭 */
void Servo_Lo(uint8_t deg);
#endif
