/**
  ******************************************************************************
  * @file    usart.c
  * @author  User
  * @version V1.0
  * @date    2025-10-20
  * @brief   USART配置源文件
  ******************************************************************************
  */

#include "usart.h"

/**
  * @brief  USART总配置函数
  * @param  None
  * @retval None
  */
void USART_Config(void)
{
    USART1_GPIO_Config();
    USART1_Config();
}

/**
  * @brief  USART1 GPIO配置
  * @param  None
  * @retval None
  */
void USART1_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能USART1和GPIO时钟
    RCC_APB2PeriphClockCmd(USART1_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(USART1_CLK, ENABLE);
    
    // 配置USART1_TX (PA9)
    GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);
    
    // 配置USART1_RX (PA10)
    GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // 浮空输入
    GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  USART1配置
  * @param  None
  * @retval None
  */
void USART1_Config(void)
{
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // USART1参数配置
    USART_InitStructure.USART_BaudRate = USART1_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    
    // 配置USART1中断
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能USART1接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    // 使能USART1
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  发送一个字节
  * @param  USARTx: USART外设
  * @param  data: 要发送的数据
  * @retval None
  */
void USART_SendByte(USART_TypeDef* USARTx, uint8_t data)
{
    USART_SendData(USARTx, data);
    
    // 等待发送完成
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

/**
  * @brief  发送字符串
  * @param  USARTx: USART外设
  * @param  str: 要发送的字符串
  * @retval None
  */
void USART_SendString(USART_TypeDef* USARTx, char* str)
{
    while(*str != '\0')
    {
        USART_SendByte(USARTx, *str);
        str++;
    }
}

/**
  * @brief  发送数据数组
  * @param  USARTx: USART外设
  * @param  data: 要发送的数据数组
  * @param  length: 数据长度
  * @retval None
  */
void USART_SendData(USART_TypeDef* USARTx, uint8_t* data, uint16_t length)
{
    uint16_t i;
    
    for(i = 0; i < length; i++)
    {
        USART_SendByte(USARTx, data[i]);
    }
}

/**
  * @brief  重定向printf函数到USART1
  * @param  ch: 要发送的字符
  * @param  f: 文件指针
  * @retval 发送的字符
  */
int fputc(int ch, FILE *f)
{
    USART_SendByte(USART1, (uint8_t)ch);
    return ch;
}