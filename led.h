#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

//LED端口定义
#define LED0 PBout(5)	// LED0 -> PB.5
#define LED1 PEout(5)	// LED1 -> PE.5

void LED_Init(void);//初始化

#endif