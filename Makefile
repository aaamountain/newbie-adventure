# STM32F10x Makefile
# 适用于正点原子战舰STM32开发板

# 项目名称
PROJECT = led_control_system

# 源文件
SOURCES = main.c led.c key.c delay.c sys.c usart.c

# 头文件目录
INCLUDES = -I.

# 编译器设置（需要安装arm-none-eabi-gcc工具链）
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# 编译选项
CFLAGS = -mcpu=cortex-m3 -mthumb -Wall -fdata-sections -ffunction-sections
CFLAGS += -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER
CFLAGS += $(INCLUDES)

# 链接选项
LDFLAGS = -mcpu=cortex-m3 -mthumb -specs=nosys.specs
LDFLAGS += -Wl,--gc-sections

# 目标文件
OBJECTS = $(SOURCES:.c=.o)

# 默认目标
all: $(PROJECT).elf $(PROJECT).hex $(PROJECT).bin
	$(SIZE) $(PROJECT).elf

# 生成ELF文件
$(PROJECT).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# 生成HEX文件
$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -O ihex $< $@

# 生成BIN文件
$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

# 编译C文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	rm -f $(OBJECTS) $(PROJECT).elf $(PROJECT).hex $(PROJECT).bin

# 伪目标
.PHONY: all clean

# 依赖关系
main.o: main.c led.h key.h delay.h sys.h usart.h
led.o: led.c led.h sys.h
key.o: key.c key.h sys.h delay.h
delay.o: delay.c delay.h sys.h
sys.o: sys.c sys.h
usart.o: usart.c usart.h sys.h