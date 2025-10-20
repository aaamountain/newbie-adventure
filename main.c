/**
  ******************************************************************************
  * @file    main.c
  * @author  User
  * @version V1.0
  * @date    2025-10-20
  * @brief   按键控制跑马灯工作模式切换，串口双向通信
  ******************************************************************************
  * 功能说明：
  * 1. KEY0按键：LED0点亮，LED1熄灭
  * 2. KEY2按键：LED1点亮，LED0熄灭  
  * 3. KEY1按键：切换暂停/继续状态
  * 4. 串口实时上报灯光状态
  * 5. 串口接收"P"指令控制暂停/开启
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "gpio.h"
#include "usart.h"
#include "delay.h"
#include <string.h>

// 系统状态定义
typedef enum {
    LED_MODE_0 = 0,    // LED0亮，LED1灭
    LED_MODE_1 = 1     // LED1亮，LED0灭
} LED_Mode_t;

typedef enum {
    SYSTEM_RUNNING = 0,  // 运行状态
    SYSTEM_PAUSED = 1    // 暂停状态
} System_State_t;

// 全局变量
LED_Mode_t current_led_mode = LED_MODE_0;
System_State_t system_state = SYSTEM_RUNNING;
uint8_t uart_rx_buffer[10];
uint8_t uart_rx_flag = 0;

// 按键状态变量
uint8_t key0_pressed = 0;
uint8_t key1_pressed = 0;
uint8_t key2_pressed = 0;

// 函数声明
void System_Init(void);
void LED_Control(LED_Mode_t mode);
void Key_Scan(void);
void UART_SendStatus(void);
void UART_ProcessCommand(void);

/**
  * @brief  主函数
  * @param  None
  * @retval int
  */
int main(void)
{
    // 系统初始化
    System_Init();
    
    // 发送启动信息
    USART_SendString(USART1, "STM32 LED Control System Started\r\n");
    USART_SendString(USART1, "Commands: P - Pause/Resume\r\n");
    
    // 初始状态设置
    LED_Control(current_led_mode);
    UART_SendStatus();
    
    while(1)
    {
        // 按键扫描
        Key_Scan();
        
        // 处理串口命令
        if(uart_rx_flag)
        {
            UART_ProcessCommand();
            uart_rx_flag = 0;
        }
        
        // 延时
        delay_ms(10);
    }
}

/**
  * @brief  系统初始化
  * @param  None
  * @retval None
  */
void System_Init(void)
{
    // 系统时钟初始化
    SystemInit();
    
    // 延时初始化
    delay_init();
    
    // GPIO初始化
    GPIO_Config();
    
    // USART初始化
    USART_Config();
}

/**
  * @brief  LED控制函数
  * @param  mode: LED模式
  * @retval None
  */
void LED_Control(LED_Mode_t mode)
{
    if(system_state == SYSTEM_PAUSED)
    {
        return; // 暂停状态下不响应LED控制
    }
    
    switch(mode)
    {
        case LED_MODE_0:
            LED0_ON();   // LED0点亮
            LED1_OFF();  // LED1熄灭
            current_led_mode = LED_MODE_0;
            break;
            
        case LED_MODE_1:
            LED0_OFF();  // LED0熄灭
            LED1_ON();   // LED1点亮
            current_led_mode = LED_MODE_1;
            break;
            
        default:
            break;
    }
}

/**
  * @brief  按键扫描函数
  * @param  None
  * @retval None
  */
void Key_Scan(void)
{
    static uint8_t key0_state = 1, key1_state = 1, key2_state = 1;
    static uint32_t key0_count = 0, key1_count = 0, key2_count = 0;
    
    // KEY0扫描 (假设连接PA0，按下为低电平)
    if(KEY0_READ() == 0)
    {
        if(key0_state == 1)
        {
            key0_count++;
            if(key0_count >= 5) // 消抖，50ms
            {
                key0_state = 0;
                key0_pressed = 1;
                key0_count = 0;
            }
        }
    }
    else
    {
        key0_state = 1;
        key0_count = 0;
    }
    
    // KEY1扫描 (假设连接PA1)
    if(KEY1_READ() == 0)
    {
        if(key1_state == 1)
        {
            key1_count++;
            if(key1_count >= 5)
            {
                key1_state = 0;
                key1_pressed = 1;
                key1_count = 0;
            }
        }
    }
    else
    {
        key1_state = 1;
        key1_count = 0;
    }
    
    // KEY2扫描 (假设连接PA2)
    if(KEY2_READ() == 0)
    {
        if(key2_state == 1)
        {
            key2_count++;
            if(key2_count >= 5)
            {
                key2_state = 0;
                key2_pressed = 1;
                key2_count = 0;
            }
        }
    }
    else
    {
        key2_state = 1;
        key2_count = 0;
    }
    
    // 处理按键事件
    if(key0_pressed)
    {
        key0_pressed = 0;
        if(system_state == SYSTEM_RUNNING)
        {
            LED_Control(LED_MODE_0);
            UART_SendStatus();
        }
    }
    
    if(key1_pressed)
    {
        key1_pressed = 0;
        // 切换暂停/继续状态
        if(system_state == SYSTEM_RUNNING)
        {
            system_state = SYSTEM_PAUSED;
        }
        else
        {
            system_state = SYSTEM_RUNNING;
        }
        UART_SendStatus();
    }
    
    if(key2_pressed)
    {
        key2_pressed = 0;
        if(system_state == SYSTEM_RUNNING)
        {
            LED_Control(LED_MODE_1);
            UART_SendStatus();
        }
    }
}

/**
  * @brief  串口发送状态信息
  * @param  None
  * @retval None
  */
void UART_SendStatus(void)
{
    if(system_state == SYSTEM_PAUSED)
    {
        USART_SendString(USART1, "状态：暂停\r\n");
    }
    else
    {
        if(current_led_mode == LED_MODE_0)
        {
            USART_SendString(USART1, "状态：LED0亮，LED1灭\r\n");
        }
        else
        {
            USART_SendString(USART1, "状态：LED0灭，LED1亮\r\n");
        }
    }
}

/**
  * @brief  处理串口命令
  * @param  None
  * @retval None
  */
void UART_ProcessCommand(void)
{
    if(uart_rx_buffer[0] == 'P' || uart_rx_buffer[0] == 'p')
    {
        // 切换暂停/继续状态
        if(system_state == SYSTEM_RUNNING)
        {
            system_state = SYSTEM_PAUSED;
            USART_SendString(USART1, "收到P指令：系统暂停\r\n");
        }
        else
        {
            system_state = SYSTEM_RUNNING;
            USART_SendString(USART1, "收到P指令：系统继续\r\n");
        }
        UART_SendStatus();
    }
    
    // 清空接收缓冲区
    memset(uart_rx_buffer, 0, sizeof(uart_rx_buffer));
}

/**
  * @brief  USART1中断服务函数
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    static uint8_t rx_index = 0;
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t received_data = USART_ReceiveData(USART1);
        
        if(received_data == '\r' || received_data == '\n')
        {
            if(rx_index > 0)
            {
                uart_rx_flag = 1;
                rx_index = 0;
            }
        }
        else if(rx_index < sizeof(uart_rx_buffer) - 1)
        {
            uart_rx_buffer[rx_index++] = received_data;
        }
        
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}