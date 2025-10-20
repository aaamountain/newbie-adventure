/**
  ******************************************************************************
  * @file    gpio.h
  * @author  User
  * @version V1.0
  * @date    2025-10-20
  * @brief   GPIO配置头文件
  ******************************************************************************
  */

#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f10x.h"

// LED引脚定义 (假设使用STM32F103C8T6开发板)
#define LED0_GPIO_PORT    GPIOC
#define LED0_GPIO_PIN     GPIO_Pin_13
#define LED0_GPIO_CLK     RCC_APB2Periph_GPIOC

#define LED1_GPIO_PORT    GPIOC
#define LED1_GPIO_PIN     GPIO_Pin_14
#define LED1_GPIO_CLK     RCC_APB2Periph_GPIOC

// 按键引脚定义
#define KEY0_GPIO_PORT    GPIOA
#define KEY0_GPIO_PIN     GPIO_Pin_0
#define KEY0_GPIO_CLK     RCC_APB2Periph_GPIOA

#define KEY1_GPIO_PORT    GPIOA
#define KEY1_GPIO_PIN     GPIO_Pin_1
#define KEY1_GPIO_CLK     RCC_APB2Periph_GPIOA

#define KEY2_GPIO_PORT    GPIOA
#define KEY2_GPIO_PIN     GPIO_Pin_2
#define KEY2_GPIO_CLK     RCC_APB2Periph_GPIOA

// LED控制宏定义
#define LED0_ON()         GPIO_ResetBits(LED0_GPIO_PORT, LED0_GPIO_PIN)
#define LED0_OFF()        GPIO_SetBits(LED0_GPIO_PORT, LED0_GPIO_PIN)
#define LED0_TOGGLE()     GPIO_WriteBit(LED0_GPIO_PORT, LED0_GPIO_PIN, \
                          (BitAction)(1 - GPIO_ReadOutputDataBit(LED0_GPIO_PORT, LED0_GPIO_PIN)))

#define LED1_ON()         GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN)
#define LED1_OFF()        GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN)
#define LED1_TOGGLE()     GPIO_WriteBit(LED1_GPIO_PORT, LED1_GPIO_PIN, \
                          (BitAction)(1 - GPIO_ReadOutputDataBit(LED1_GPIO_PORT, LED1_GPIO_PIN)))

// 按键读取宏定义
#define KEY0_READ()       GPIO_ReadInputDataBit(KEY0_GPIO_PORT, KEY0_GPIO_PIN)
#define KEY1_READ()       GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN)
#define KEY2_READ()       GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN)

// 函数声明
void GPIO_Config(void);
void LED_GPIO_Config(void);
void KEY_GPIO_Config(void);

#endif /* __GPIO_H */