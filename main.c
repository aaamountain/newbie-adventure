#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include <string.h>   // 必须包含，用于strcmp/strstr等函数

// 系统状态定义
typedef enum {
    LED_MODE_0 = 0,  // LED0亮，LED1灭
    LED_MODE_1 = 1   // LED1亮，LED0灭
} LED_Mode_t;

// 全局变量
static LED_Mode_t current_mode = LED_MODE_0;  // 当前LED模式
static u8 system_paused = 0;                 // 系统暂停标志：0=运行，1=暂停
static u8 last_led0_state = 1;               // 记录上次LED0状态
static u8 last_led1_state = 1;               // 记录上次LED1状态
static u16 status_report_counter = 0;        // 状态上报计数器

// 函数声明
void LED_Control(LED_Mode_t mode);
void Send_LED_Status(void);
void Process_Serial_Command(void);

// LED控制函数
void LED_Control(LED_Mode_t mode)
{
    if(system_paused) return;  // 如果系统暂停，不执行LED控制
    
    switch(mode)
    {
        case LED_MODE_0:
            LED0 = 0;  // LED0点亮（低电平有效）
            LED1 = 1;  // LED1熄灭（高电平无效）
            break;
        case LED_MODE_1:
            LED0 = 1;  // LED0熄灭
            LED1 = 0;  // LED1点亮
            break;
        default:
            break;
    }
}

// 发送LED状态到串口
void Send_LED_Status(void)
{
    u8 led0_current = LED0;
    u8 led1_current = LED1;
    
    // 只有状态发生变化时才发送，避免频繁发送相同信息
    if(led0_current != last_led0_state || led1_current != last_led1_state)
    {
        if(system_paused)
        {
            printf("状态：暂停\r\n");
        }
        else
        {
            if(led0_current == 0 && led1_current == 1)
            {
                printf("状态：LED0亮，LED1灭\r\n");
            }
            else if(led0_current == 1 && led1_current == 0)
            {
                printf("状态：LED0灭，LED1亮\r\n");
            }
        }
        
        last_led0_state = led0_current;
        last_led1_state = led1_current;
    }
}

// 处理串口命令
void Process_Serial_Command(void)
{
    u16 len = 0;
    
    if(USART_RX_STA & 0x8000)  // 接收到完整命令
    {
        len = USART_RX_STA & 0x3fff;  // 得到接收数据长度
        USART_RX_BUF[len] = '\0';     // 字符串结束符
        
        // 处理暂停/继续命令
        if(strcmp((char*)USART_RX_BUF, "P") == 0)
        {
            system_paused = !system_paused;  // 切换暂停状态
            if(system_paused)
            {
                printf("收到指令P：系统暂停\r\n");
                printf("状态：暂停\r\n");
            }
            else
            {
                printf("收到指令P：系统继续\r\n");
                LED_Control(current_mode);  // 恢复当前模式
                Send_LED_Status();
            }
        }
        else
        {
            printf("无效指令，请输入: P (暂停/继续)\r\n");
        }
        
        // 清除接收状态，准备下一次接收
        USART_RX_STA = 0;
    }
}

int main(void)
{		
    u8 key_value = 0;
    u16 times = 0;
    
    // 系统初始化
    delay_init();	    	 // 延时函数初始化	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置NVIC中断分组
    uart_init(115200);	 // 串口初始化为115200
    LED_Init();			     // LED端口初始化
    KEY_Init();           // 按键初始化

    // 初始状态设置
    LED_Control(current_mode);  // 设置初始LED状态
    
    printf("\r\n=== 战舰STM32开发板 LED控制系统 ===\r\n");
    printf("功能说明：\r\n");
    printf("KEY0: 切换到模式0 (LED0亮，LED1灭)\r\n");
    printf("KEY2: 切换到模式1 (LED1亮，LED0灭)\r\n");
    printf("KEY1: 暂停/继续切换\r\n");
    printf("串口指令P: 暂停/继续控制\r\n");
    printf("系统启动完成，当前模式：LED0亮，LED1灭\r\n\r\n");
    
    while(1)
    {
        // === 处理串口命令 ===
        Process_Serial_Command();
        
        // === 按键扫描处理 ===
        key_value = KEY_Scan(0);  // 不支持连按
        
        if(key_value != 0)
        {
            switch(key_value)
            {
                case KEY0_PRES:  // KEY0按下 - 切换到模式0
                    if(!system_paused)  // 只有在非暂停状态下才响应
                    {
                        current_mode = LED_MODE_0;
                        LED_Control(current_mode);
                        printf("按键KEY0：切换到模式0\r\n");
                        Send_LED_Status();
                    }
                    break;
                    
                case KEY2_PRES:  // KEY2按下 - 切换到模式1
                    if(!system_paused)  // 只有在非暂停状态下才响应
                    {
                        current_mode = LED_MODE_1;
                        LED_Control(current_mode);
                        printf("按键KEY2：切换到模式1\r\n");
                        Send_LED_Status();
                    }
                    break;
                    
                case KEY1_PRES:  // KEY1按下 - 暂停/继续切换
                    system_paused = !system_paused;
                    if(system_paused)
                    {
                        printf("按键KEY1：系统暂停\r\n");
                        printf("状态：暂停\r\n");
                    }
                    else
                    {
                        printf("按键KEY1：系统继续\r\n");
                        LED_Control(current_mode);  // 恢复当前模式
                        Send_LED_Status();
                    }
                    break;
                    
                default:
                    break;
            }
        }
        
        // === 定时状态信息和心跳 ===
        times++;
        
        // 每5秒发送一次系统信息
        if(times % 5000 == 0)
        {
            printf("\r\n=== 系统运行中 ===\r\n");
            printf("当前状态：%s\r\n", system_paused ? "暂停" : "运行");
            if(!system_paused)
            {
                printf("当前模式：%s\r\n", (current_mode == LED_MODE_0) ? "LED0亮，LED1灭" : "LED0灭，LED1亮");
            }
            printf("可用指令：KEY0/KEY2(模式切换), KEY1(暂停/继续), 串口P(暂停/继续)\r\n\r\n");
        }
        
        // 定期发送状态（每1秒检查一次）
        status_report_counter++;
        if(status_report_counter >= 1000)
        {
            Send_LED_Status();
            status_report_counter = 0;
        }
        
        delay_ms(1);  // 1ms延时，提高响应速度
    }	 
}