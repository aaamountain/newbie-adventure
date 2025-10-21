#include "stm32f10x.h"

// 系统时钟频率 (72MHz)
uint32_t SystemCoreClock = 72000000;

// GPIO初始化函数
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{
    // 简化实现 - 实际项目中需要完整的寄存器操作
    // 这里只是为了编译通过
}

// GPIO设置高电平
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->BSRR = GPIO_Pin;
}

// GPIO设置低电平
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->BRR = GPIO_Pin;
}

// RCC时钟使能
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
    // 简化实现
}

// USART初始化
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct)
{
    // 简化实现
}

// USART使能
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState)
{
    // 简化实现
}

// USART中断配置
void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState)
{
    // 简化实现
}

// USART发送数据
void USART_SendData(USART_TypeDef* USARTx, uint16_t Data)
{
    USARTx->DR = (Data & (uint16_t)0x01FF);
}

// USART接收数据
uint16_t USART_ReceiveData(USART_TypeDef* USARTx)
{
    return (uint16_t)(USARTx->DR & (uint16_t)0x01FF);
}

// USART获取标志状态
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG)
{
    if ((USARTx->SR & USART_FLAG) != (uint16_t)RESET)
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

// USART获取中断状态
ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT)
{
    uint16_t bitpos = 0x00, itmask = 0x00, usartreg = 0x00;
    
    // 简化实现
    if ((USARTx->SR & (USART_IT >> 0x08)) != (uint16_t)RESET)
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

// NVIC初始化
void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct)
{
    // 简化实现
}

// NVIC优先级分组配置
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
    // 简化实现
}

// SysTick时钟源配置
void SysTick_CLKSourceConfig(uint32_t SysTick_CLKSource)
{
    if (SysTick_CLKSource == SysTick_CLKSource_HCLK)
    {
        SysTick->CTRL |= SysTick_CLKSource_HCLK;
    }
    else
    {
        SysTick->CTRL &= SysTick_CLKSource_HCLK_Div8;
    }
}