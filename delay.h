#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"

//如果需要使用OS,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif