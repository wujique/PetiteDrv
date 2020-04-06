#ifndef __MCU_H__
#define __MCU_H__

#include "stm32f10x.h"

#ifndef NULL
#define NULL 0
#endif

typedef   unsigned int size_t;

/* io端口组名称转义*/
typedef enum{
	MCU_PORT_NULL = 0x00,
	MCU_PORT_A,
	MCU_PORT_B,
	MCU_PORT_C,
	MCU_PORT_D,
	MCU_PORT_E,
	MCU_PORT_F,
	MCU_PORT_G,
	MCU_PORT_H,
	MCU_PORT_I,
	MCU_PORT_J,
	MCU_PORT_K,
	MCU_PORT_MAX
}MCU_PORT;

extern const GPIO_TypeDef *Stm32PortList[MCU_PORT_MAX];

/*IO 电平定义 */
typedef enum{
	MCU_IO_STA_0 = 0x00,
	MCU_IO_STA_1,
}MCU_IO_STA;
	
/*IO位定义，每个PORT支持16个BIT*/
typedef enum{
	MCU_IO_0 = 0x0001,
	MCU_IO_1 = 0x0002,
	MCU_IO_2 = 0x0004,
	MCU_IO_3 = 0x0008,
	MCU_IO_4 = 0x0010,
	MCU_IO_5 = 0x0020,
	MCU_IO_6 = 0x0040,
	MCU_IO_7 = 0x0080,
	MCU_IO_8 = 0x0100,
	MCU_IO_9 = 0x0200,
	MCU_IO_10 = 0x0400,
	MCU_IO_11 = 0x0800,
	MCU_IO_12 = 0x1000,
	MCU_IO_13 = 0x2000,
	MCU_IO_14 = 0x4000,
	MCU_IO_15 = 0x8000,
	MCU_IO_MAX = 16,
}MCU_IO;

typedef enum {
  MCU_UART_1 =0,
  MCU_UART_2,
  MCU_UART_3,
  MCU_UART_MAX,
}McuUartNum; 


/*
SPI模式
*/
typedef enum{
	SPI_MODE_0 =0,
	SPI_MODE_1,
	SPI_MODE_2,
	SPI_MODE_3,
	SPI_MODE_MAX
}SPI_MODE;


#endif

