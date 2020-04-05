#ifndef __MCU_H__
#define __MCU_H__


#include "stm32f10x.h"


typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  /*!< Read Only */
typedef __I int16_t vsc16;  /*!< Read Only */
typedef __I int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  /*!< Read Only */
typedef __I uint16_t vuc16;  /*!< Read Only */
typedef __I uint8_t vuc8;   /*!< Read Only */

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

#include "mcu_timer.h"
#include "mcu_io.h"

#include "mcu_uart.h"
#include "mcu_i2c.h"
#include "mcu_spi.h"
#include "mcu_rtc.h"
#include "mcu_fsmc.h"

#endif
