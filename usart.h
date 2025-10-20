/**
  ******************************************************************************
  * @file    usart.h
  * @author  User
  * @version V1.0
  * @date    2025-10-20
  * @brief   USART配置头文件
  ******************************************************************************
  */

#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>

// USART1引脚定义
#define USART1_GPIO_PORT      GPIOA
#define USART1_TX_PIN         GPIO_Pin_9
#define USART1_RX_PIN         GPIO_Pin_10
#define USART1_GPIO_CLK       RCC_APB2Periph_GPIOA
#define USART1_CLK            RCC_APB2Periph_USART1

// USART配置参数
#define USART1_BAUDRATE       115200

// 函数声明
void USART_Config(void);
void USART1_GPIO_Config(void);
void USART1_Config(void);
void USART_SendByte(USART_TypeDef* USARTx, uint8_t data);
void USART_SendString(USART_TypeDef* USARTx, char* str);
void USART_SendData(USART_TypeDef* USARTx, uint8_t* data, uint16_t length);

#endif /* __USART_H */