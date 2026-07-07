/**
 * USART3 串口驱动
 * PB10 = TX, PB11 = RX
 */
#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

#define RX_BUF_SIZE  16
extern char     rx_buf[RX_BUF_SIZE];
extern uint8_t  rx_index;
extern volatile uint8_t rx_complete;

void USART3_Init(uint32_t baudrate);
void USART3_SendByte(uint8_t ch);
void USART3_SendString(char *str);

#define USART1_SendByte(c)   USART3_SendByte(c)
#define USART1_SendString(s) USART3_SendString(s)

#endif
