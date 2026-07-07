/**
 * 舵机驱动 — TIM2_CH1=PA0 CH2=PA1, 50Hz PWM
 * 使用: Servo_Init() → Servo_Up(angle) / Servo_Lo(angle)
 */
#ifndef __SERVO_H
#define __SERVO_H
#include "stm32f10x.h"

/* ===== 闸门角度定义 (MG996R: 500~2500us 对应 0~180°) ===== */
#define GATE_OPEN_ANGLE   90   /* 闸门开. 行程起点=闭180°→开90°, 行程90°.
                                  提速技巧: 若药袋能顺利通过, 增大到此值更小(120~135)可缩短开关行程,
                                  关门更快(行程与角度差成正比). 必须实物验证药袋不卡料. */
#define GATE_CLOSE_ANGLE  180  /* 闸门闭 */
#define GATE_MID_ANGLE    ((GATE_OPEN_ANGLE + GATE_CLOSE_ANGLE) / 2)  /* 自检中位 */

void Servo_Init(void);
void Servo_Up(uint8_t deg);   /* 上闸: GATE_OPEN_ANGLE=开 GATE_CLOSE_ANGLE=闭 */
void Servo_Lo(uint8_t deg);   /* 下闸: 同上 */
#endif
