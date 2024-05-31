#ifndef __MCU_H__
#define __MCU_H__

#include "board_sysconf.h"

#ifdef MCU_STM32F1XX
#include "stm32f10x.h"
#endif

#ifdef MCU_STM32H7XX
#include "stm32h7xx.h"

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

#ifndef NULL
#define NULL 0
#endif

typedef   unsigned int size_t;
#endif

#ifdef MCU_STM32F4XX
#include "stm32f4xx.h"
#endif
typedef   unsigned int size_t;
/*------------------------------------*/

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
	MCU_IO_NULL = 0,
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

extern void mcu_io_init(void);
extern void mcu_io_config_in(MCU_PORT port, MCU_IO pin);
extern void mcu_io_config_out(MCU_PORT port, MCU_IO pin);
extern void mcu_io_output_setbit(MCU_PORT port, MCU_IO pin);
extern void mcu_io_output_resetbit(MCU_PORT port, MCU_IO pin);
extern u8 mcu_io_input_readbit(MCU_PORT port, MCU_IO pin);
extern u16 mcu_io_input_readport(MCU_PORT port);
/*------------------------------------*/
#include "Petite/mem/p_kfifo.h"

typedef enum {
  MCU_UART_1 =0,
  MCU_UART_2,
  MCU_UART_3,
  MCU_UART_4,
  MCU_UART_5,
  MCU_UART_6,
  MCU_UART_7,
  MCU_UART_8,
  MCU_UART_MAX,
}McuUartNum;

typedef struct
{
  uint32_t BaudRate; //波特率           
  uint16_t WordLength;  //        
  uint16_t StopBits;            
  uint16_t Parity;              
  uint16_t Mode;               
  uint16_t HardwareFlowControl; 
  
  uint32_t bufsize;//接收缓冲长度
} BusUartPra;

typedef struct {
	McuUartNum comport;
	
	/* mcu 层会直接操作下面变量 */
	char *buf;
	struct _pkfifo Kfifo;//接收缓冲
	u8  OverFg;//溢出标志	
}BusUartNode;

extern s32 mcu_uart_init(McuUartNum comport, BusUartNode *BusNode, const BusUartPra *Pra);
extern s32 mcu_uart_deinit (McuUartNum comport);
extern s32 mcu_uart_send (McuUartNum comport, u8 *buf, s32 len);
extern s32 mcu_uart_set_baud (McuUartNum comport, s32 baud);
s32 mcu_uart_write(McuUartNum comport, u8 *buf, s32 len);
#define mcu_uart_send mcu_uart_write
/*------------------------------------*/
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

/**
 * @brief   spi通道IO口定义
 * 
 * 
 */
typedef struct
{
	MCU_PORT clkport;
	MCU_IO clkpin;
	MCU_PORT mosiport;
	MCU_IO mosipin;
	MCU_PORT misoport;
	MCU_IO misopin;

}SpiIoDef;

extern s32 mcu_hspi_init(const char *name, const SpiIoDef *io);
extern s32 mcu_hspi_open(const char *name, SPI_MODE mode, u16 KHz);
extern s32 mcu_hspi_close(const char *name);
extern s32 mcu_hspi_transfer(const char *name, u8 *snd, u8 *rsv, s32 len);
/*------------------------------------*/
#define MCU_I2C_MODE_W 0
#define MCU_I2C_MODE_R 1

extern void SCCB_GPIO_Config(void);
extern uint8_t bus_sccb_writereg(uint8_t DeviceAddr, uint16_t Addr, uint8_t Data);
extern uint8_t bus_sccb_readreg(uint8_t DeviceAddr, uint16_t Addr);
/*------------------------------------*/
/*定义则使用中断方式*/
#define MCU_ADC_IRQ 1

extern void mcu_adc_init(s32 (*AdcCallback)(u16));
extern u16 mcu_adc_get_conv(u8 ch);
extern s32 mcu_adc_start_conv(u8 ch);

extern s32 mcu_dac_init(void);
/**
 *@brief:      mcu_dac_open
 *@details:    打开DAC控制器
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
extern s32 mcu_dac_open(void);
/**
 *@brief:      mcu_dac_output_vol
 *@details:    设置DAC输出值
 *@param[in]   u16 vol， 电压，单位MV，0-Vref  
 *@param[out]  无
 *@retval:     
 */
extern void mcu_dac_output_vol(u16 vol);
/**
 *@brief:      mcu_dac_output
 *@details:    将一个数值作为DAC值输出
 *@param[in]   u16 data  
 *@param[out]  无
 *@retval:     
 */
extern void mcu_dac_output(u16 data);
/**
 *@brief:      mcu_dac_test
 *@details:    DAC测试程序
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
extern s32 mcu_dac_test(void);
/*------------------------------------------*/
#define DCMI_FLAG_BUF0 	0X01
#define DCMI_FLAG_BUF1	0X02
#define DCMI_FLAG_FRAME	0X04

int mcu_dcmi_captruce(uint32_t Mode, uint32_t addr, uint32_t Length);

extern void BUS_DCMI_Config(u8 mode, u16 pck, u16 vs, u16 hs);
extern void BUS_DCMI_DMA_Init(u8 mode, u32 Memory0BaseAddr, u32 Memory1BaseAddr, u32 BufferSize);
extern void MCO1_Init(void);
/*------------------------------------------*/
extern volatile u16 *LcdReg;
extern volatile u16 *LcdData;

void mcu_fsmc_lcd_Init(uint8_t conf_num);
/*------------------------------------------*/
#define 	AUDIO_I2S_LSB         0x00	
#define 	AUDIO_I2S_MSB         0x01
#define 	AUDIO_I2S_Phillips    0x02
#define 	AUDIO_I2S_PCM         0x03

#if 0	
#define 	AUDIO_I2S_Data_16b    0x00
#define 	AUDIO_I2S_Data_20b    0x01
#define 	AUDIO_I2S_Data_24b    0x02
#define 	AUDIO_I2S_Data_32b    0x03
#endif

extern void mcu_i2s_init (u8 type);
extern void mcu_i2s_config(u32 AudioFreq, u16 Standard,u16 DataFormat);
extern void mcu_i2s_dma_init(u16 *buffer,u32 len);
extern void mcu_i2s_dma_start(void);
extern void mcu_i2s_dma_stop(void);
extern void mcu_i2s_dma_process(void);

extern void mcu_i2sext_config(u32 AudioFreq, u16 Standard,u16 DataFormat);
extern void mcu_i2sext_dma_init(u16* buf0, u16 *buf1, u32 len);
extern void mcu_i2sext_dma_start(void);
extern void mcu_i2sext_dma_stop(void);
/*------------------------------------------*/

/*
	硬定时器定义
	407 有多少个定时器?
*/
typedef enum
{
	MCU_TIMER_NULL = 0,
	MCU_TIMER_1,
	MCU_TIMER_2,
	MCU_TIMER_3,
	MCU_TIMER_4,
	MCU_TIMER_5,
	MCU_TIMER_6,
	MCU_TIMER_7,
	MCU_TIMER_8,
	MCU_TIMER_9,
	MCU_TIMER_10,
	MCU_TIMER_11,
	MCU_TIMER_12,
	MCU_TIMER_13,
	MCU_TIMER_14,
	MCU_TIMER_MAX,
}McuTimerNum;

typedef enum
{
	MCU_TIMER_1US = 1,
	MCU_TIMER_10US,
	MCU_TIMER_100US,
	MCU_TIMER_500US,
	MCU_TIMER_100NS,
	MCU_TIMER_DEF_MAX
}McuTimerTickDef;
typedef enum
{
	MCU_TIMER_RE = 0,/*重复*/
	MCU_TIMER_ONE,/*单次*/
}McuTimerReType;
	
typedef enum
{
	MCU_TIMER_CH1 = 0,
	MCU_TIMER_CH2,
	MCU_TIMER_CH3,
	MCU_TIMER_CH4
}McuTimerCh;

typedef enum
{
	MCU_TIMER_CAP_RISING = 0,
	MCU_TIMER_CAP_FALLING,
	MCU_TIMER_CAP_BOTHEDGE,
}McuTimerCapEdge;


extern s32 mcu_timer_init(McuTimerNum timer);
extern s32 mcu_timer_config(McuTimerNum timer, McuTimerTickDef tickdef, u32 tick, void (*callback)(void), u8 type);
extern s32 mcu_timer_start(McuTimerNum timer);
extern s32 mcu_timer_stop(McuTimerNum timer);

extern s32 mcu_timer_pwm_init(McuTimerNum timer, McuTimerTickDef tickdef, u32 tick, u8 duty, McuTimerCh ch);

extern s32 mcu_timer_cap_init(McuTimerNum timer, McuTimerTickDef tickdef, McuTimerCh ch, McuTimerCapEdge edge);
extern u32 mcu_timer_get_cap(McuTimerNum timer, McuTimerCh ch);

extern s32 mcu_io_config_timer(MCU_PORT port, MCU_IO pin, McuTimerNum timer);
/*------------------------------------------*/
#if 0
typedef long time_t;

/*
	从1970年1月1日0:0:0开始过了多少秒
*/
struct timeval {
	time_t tv_sec; /*秒*/    
	time_t tv_usec; /*微秒*/ 
};

struct tm {
	int tm_sec; /*秒，正常范围0-59， 但允许至61*/    
	int tm_min; /*分钟，0-59*/    
	int tm_hour; /*小时， 0-23*/     
	int tm_mday; /*日，即一个月中的第几天，1-31*/    
	int tm_mon; /*月， 从一月算起，0-11*/  
	int tm_year; /*年， 从1900至今已经多少年*/ 
	int tm_wday; /*星期，一周中的第几天， 从星期日算起，0-6*/    
	int tm_yday; /*从今年1月1日到目前的天数，范围0-365*/    
	int tm_isdst; /*夏令时标识符 1: 是 DST，0: 不是 DST，负数：不了解*/ 
	long int tm_gmtoff;/*指定了日期变更线东面时区中UTC东部时区正秒数或UTC西部时区的负秒数*/
	const char *tm_zone;     /*当前时区的名字(与环境变量TZ有关)*/
};
#endif 

extern s32 mcu_rtc_init(void);
extern s32 mcu_rtc_set_time(u8 hours, u8 minutes, u8 seconds);
extern s32 mcu_rtc_set_date(u8 year, u8 weekday, u8 month, u8 date);
/*------------------------------------------*/

/*------------------------------------------*/

/*------------------------------------------*/

/*------------------------------------------*/

/*------------------------------------------*/
#endif

