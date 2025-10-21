#ifndef _STDIO_H
#define _STDIO_H

#include "stm32f10x.h"

// FILE结构体定义
typedef struct {
    int handle;
} FILE;

// 标准输出
extern FILE __stdout;

// 函数声明
int printf(const char *format, ...);
int fputc(int ch, FILE *f);

#endif /* _STDIO_H */