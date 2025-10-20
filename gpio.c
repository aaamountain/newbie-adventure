/**
  ******************************************************************************
  * @file    gpio.c
  * @author  User
  * @version V1.0
  * @date    2025-10-20
  * @brief   GPIO配置源文件
  ******************************************************************************
  */

#include "gpio.h"

/**
  * @brief  GPIO总配置函数
  * @param  None
  * @retval None
  */
void GPIO_Config(void)
{
    LED_GPIO_Config();
    KEY_GPIO_Config();
}

/**
  * @brief  LED GPIO配置
  * @param  None
  * @retval None
  */
void LED_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(LED0_GPIO_CLK | LED1_GPIO_CLK, ENABLE);
    
    // 配置LED0引脚
    GPIO_InitStructure.GPIO_Pin = LED0_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 输出速度50MHz
    GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStructure);
    
    // 配置LED1引脚
    GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);
    
    // 初始状态：LED0和LED1都熄灭
    LED0_OFF();
    LED1_OFF();
}

/**
  * @brief  按键GPIO配置
  * @param  None
  * @retval None
  */
void KEY_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(KEY0_GPIO_CLK | KEY1_GPIO_CLK | KEY2_GPIO_CLK, ENABLE);
    
    // 配置KEY0引脚
    GPIO_InitStructure.GPIO_Pin = KEY0_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStructure);
    
    // 配置KEY1引脚
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
    
    // 配置KEY2引脚
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
}