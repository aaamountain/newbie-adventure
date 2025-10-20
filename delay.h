/**
  ******************************************************************************
  * @file    delay.h
  * @author  User
  * @version V1.0
  * @date    2025-10-20
  * @brief   延时函数头文件
  ******************************************************************************
  */

#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

// 函数声明
void delay_init(void);
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);

#endif /* __DELAY_H */