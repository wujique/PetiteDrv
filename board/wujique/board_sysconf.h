/**
 * @file                wujique_sysconf.h
 * @brief           系统配置
 * @author          wujique
 * @date            2018年3月30日 星期五
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年3月30日 星期五
 *   作    者:         wujique
 *   修改内容:   创建文件
*/
#ifndef __WUJIQUE_SYSCONF__
#define __WUJIQUE_SYSCONF__

/*定义调试信息输出串口号*/
#define PC_PORT  MCU_UART_3 

/* 	用宏控制哪些LCD驱动参加编译*/
#define LCD_DRIVER_ST7565		1
#define LCD_DRIVER_SSD1615		1
#define LCD_DRIVER_9341			1
//#define LCD_DRIVER_9341_8BIT	1
#define LCD_DRIVER_9325			1
//#define LCD_DRIVER_7735			1
//#define LCD_DRIVER_7789			1
//#define LCD_DRIVER_91874		1
//#define LCD_DRIVER_3820			1
#define LCD_DRIVER_NT35510		1


/*
	选择触摸屏检测方案
*/
#define SYS_USE_TS_IC_CASE		1
//#define SYS_USE_TS_ADC_CASE 	1

#ifdef SYS_USE_TS_IC_CASE
/*
	定义xpt 2046使用 模拟SPI1_ch1
	分频设置为0
*/
#define XPT2046_SPI "VSPI1_CH1"
#define XPT2046_SPI_PRE	0
/*
	如果使用硬件SPI3_CH4,
	分频不能设置太快
*/
//#define XPT2046_SPI "SPI3_CH4"
//#define XPT2046_SPI_PRE	SPI_BaudRatePrescaler_8

#endif

/*xpt2046方案基于VSPI1*/
#ifdef SYS_USE_TS_IC_CASE
	#define SYS_USE_VSPI1	1
#endif

/*
	RS485跟外扩串口设备共用。
*/
//#define SYS_USE_RS485	1
//#define SYS_USE_EXUART	1
#define SYS_USE_USB	1
#define SYS_USE_CAMERA  1

/* 
	对外扩接口进行功能定义  
*/

/*
	VSPI2 VI2C2 KEYPAD ,3者只能用一个
*/
//#define SYS_USE_VSPI2 1
//#define SYS_USE_VI2C2	1
#define SYS_USE_KEYPAD	1
#ifdef SYS_USE_KEYPAD
#define KEY_PAD_COL_NUM (4)//4列
#define KEY_PAD_ROW_NUM (4)//4行	
#endif

#define BOARD_DEV_HTU21U	1
#ifdef  BOARD_DEV_HTU21U
#define DEV_HTU21D_I2CBUS "VI2C2"
#endif

#define BOARD_DEV_HCOC	1
#ifdef	BOARD_DEV_HCOC
#define DEV_PTHCHO_UART MCU_UART_1
#endif

#if (defined(SYS_USE_EXUART) && defined(SYS_USE_RS485))
 #error "please not define SYS_USE_EXUART & SYS_USE_RS485 sametime!(in wujique_sysconf.h file)"
#endif

#if (defined(SYS_USE_EXUART) && defined(SYS_USE_USB))
 #error "please not define SYS_USE_EXUART & SYS_USE_USB sametime!(in wujique_sysconf.h file)"
#endif

#if (defined(SYS_USE_EXUART) && defined(SYS_USE_CAMERA))
 #error "please not define SYS_USE_EXUART & SYS_USE_CAMERA sametime!(in wujique_sysconf.h file)"
#endif

#if (defined(SYS_USE_RS485) && defined(SYS_USE_USB))
 #error "please not define SYS_USE_RS485 & SYS_USE_USB sametime!(in wujique_sysconf.h file)"
#endif
#if (defined(SYS_USE_RS485) && defined(SYS_USE_CAMERA))
 #error "please not define SYS_USE_RS485 & SYS_USE_CAMERA sametime!(in wujique_sysconf.h file)"
#endif

/*
	虚拟SPI2，使用外扩IO，跟矩阵按键冲突，跟I2C2也共用
*/
#if (defined(SYS_USE_VSPI2) && defined(SYS_USE_KEYPAD))
 #error "please not define SYS_USE_VSPI2 & SYS_USE_KEYPAD sametime!(in wujique_sysconf.h file)"
#endif
#if (defined(SYS_USE_VI2C2) && defined(SYS_USE_KEYPAD))
 #error "please not define SYS_USE_VI2C2 & SYS_USE_KEYPAD sametime!(in wujique_sysconf.h file)"
#endif
#if (defined(SYS_USE_VSPI2) && defined(SYS_USE_VI2C2))
 #error "please not define SYS_USE_VSPI2 & SYS_USE_VI2C2 sametime!(in wujique_sysconf.h file)"
#endif


/*触摸屏检测方案：xpt2046 方案和ADC方案只能选一种*/
#if defined(SYS_USE_TS_IC_CASE) && defined(SYS_USE_TS_ADC_CASE)
 #error "please just select one touch device!(in wujique_sysconf.h file)"
#endif

/* 是否使用RTOS */
#define SYS_USE_RTOS 1

#ifdef SYS_USE_RTOS
#define Wujique407_TASK_STK_SIZE (1024)
#define Wujique407_TASK_PRIO	1

#define START_TASK_STK_SIZE (512)
#define START_TASK_PRIO	3//

#define USB_TASK_STK_SIZE (1024)
#define USB_TASK_PRIO	2
#endif

/*定义alloc.c内存管理数组*/

#define AllocArraySize (80*1024)


/*  配置 虚拟文件系统 */
#define SYS_FS_NUM 5
#define SYS_FS_FATFS 1//使能FATFS
#define VFS_SD_DIR	"mtd0"//sd卡文件系统挂载vfs中的目录名
#define SYS_FS_FATFS_SD "1:/"//sd卡目录名， 这个名字跟diso.c中的序号有关系
#define VFS_USB_DIR	"mtd1"//USB
#define SYS_FS_FATFS_USB "2:/"

/*	字体类型定义 */
#define FONT_LIST_MAX 4	
extern struct fbcon_font_desc *FontList[FONT_LIST_MAX];


/* 中断优先级统一管理          */
#define NVIC_PRE(x) NVIC_PRE_PRI_##x
#define NVIC_SUB(x) NVIC_SUB_PRI_##x
/*
#define NVIC_PRE_PRI_
#define NVIC_SUB_PRI_
*/
/*
	USB 中断
	
*/
#define NVIC_PRE_PRI_OTG_FS_IRQn	1
#define NVIC_SUB_PRI_OTG_FS_IRQn	1

#if 0
#define NVIC_PRE_PRI_OTG_HS_EP1_OUT_IRQn	1
#define NVIC_SUB_PRI_OTG_HS_EP1_OUT_IRQn	2
#define NVIC_PRE_PRI_OTG_HS_EP1_IN_IRQn 1
#define NVIC_SUB_PRI_OTG_HS_EP1_IN_IRQn 1
#endif
/*
	SDIO_IRQn 0 0
*/
#define NVIC_PRE_PRI_SDIO_IRQn	0
#define NVIC_SUB_PRI_SDIO_IRQn	0
/*
SD_SDIO_DMA_IRQn 0 1
*/
#define NVIC_PRE_PRI_SD_SDIO_DMA_IRQn	1
#define NVIC_SUB_PRI_SD_SDIO_DMA_IRQn	0


#define NVIC_PRE_PRI_USART1_IRQn	3
#define NVIC_SUB_PRI_USART1_IRQn	3

#define NVIC_PRE_PRI_USART2_IRQn	3
#define NVIC_SUB_PRI_USART2_IRQn	3

#define NVIC_PRE_PRI_USART3_IRQn	3
#define NVIC_SUB_PRI_USART3_IRQn	3

#define NVIC_PRE_PRI_TIM2_IRQn		1
#define NVIC_SUB_PRI_TIM2_IRQn  	1

#define NVIC_PRE_PRI_ADC_IRQn	1
#define NVIC_SUB_PRI_ADC_IRQn	1

#define NVIC_PRE_PRI_CAN1_RX0_IRQn	2
#define NVIC_SUB_PRI_CAN1_RX0_IRQn	0
/*
i2s 播放
DMA1_Stream4_IRQn 0 0
*/
#define NVIC_PRE_PRI_DMA1_Stream4_IRQn	0
#define NVIC_SUB_PRI_DMA1_Stream4_IRQn 	0

/*
i2s 录音
DMA1_Stream3_IRQn	0 0
*/
#define NVIC_PRE_PRI_DMA1_Stream3_IRQn	0
#define NVIC_SUB_PRI_DMA1_Stream3_IRQn	0

/*
简易滴答时钟
TIM5_IRQn 2 2
*/
#define NVIC_PRE_PRI_TIM5_IRQn	2
#define NVIC_SUB_PRI_TIM5_IRQn	2

/*
	dac sound
	TIM3_IRQn 0 0
*/
#define NVIC_PRE_PRI_TIM3_IRQn	0
#define NVIC_SUB_PRI_TIM3_IRQn	0

/* 
adc 方式触摸屏流程控制
TIM7_IRQn 1 1
*/
#define NVIC_PRE_PRI_TIM7_IRQn	1
#define NVIC_SUB_PRI_TIM7_IRQn	1

/*
网口检测
EXTI15_10_IRQn 1 1
*/
#define NVIC_PRE_PRI_EXTI15_10_IRQn	1
#define NVIC_SUB_PRI_EXTI15_10_IRQn	1

#endif


