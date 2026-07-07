/**
 * USART3 串口驱动 (SPL)
 * PB10 = TX, PB11 = RX
 */
#include "usart.h"
#include <stdio.h>

char     rx_buf[RX_BUF_SIZE] = {0};
uint8_t  rx_index = 0;
volatile uint8_t rx_complete = 0;

void USART3_Init(uint32_t baudrate) {
    GPIO_InitTypeDef g;
    USART_InitTypeDef u;
    NVIC_InitTypeDef n;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    g.GPIO_Pin=GPIO_Pin_10;g.GPIO_Mode=GPIO_Mode_AF_PP;g.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&g);  /* PB10 = TX */
    g.GPIO_Pin=GPIO_Pin_11;g.GPIO_Mode=GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB,&g);  /* PB11 = RX */

    u.USART_BaudRate=baudrate;u.USART_WordLength=USART_WordLength_8b;u.USART_StopBits=USART_StopBits_1;
    u.USART_Parity=USART_Parity_No;u.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    u.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
    USART_Init(USART3,&u);

    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
    n.NVIC_IRQChannel=USART3_IRQn;n.NVIC_IRQChannelPreemptionPriority=1;n.NVIC_IRQChannelSubPriority=0;n.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&n);
    USART_Cmd(USART3,ENABLE);
}

void USART3_SendByte(uint8_t ch){while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);USART_SendData(USART3,ch);}
void USART3_SendString(char *s){while(*s)USART3_SendByte((uint8_t)*s++);}
int fputc(int ch,FILE*f){(void)f;USART3_SendByte((uint8_t)ch);return ch;}

void USART3_IRQHandler(void){
    if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET){
        uint8_t c=USART_ReceiveData(USART3);
        if(c=='\n'||c=='\r'){
            if(rx_index>0){rx_buf[rx_index]=0;rx_complete=1;rx_index=0;}
        }else if(rx_index<RX_BUF_SIZE-1)rx_buf[rx_index++]=c;
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);
    }
}
