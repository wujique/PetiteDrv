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
#ifndef __BOARD_SYSCONF__
#define __BOARD_SYSCONF__

#define PC_PORT  MCU_UART_4 

/* 
	本文件定义板设备情况。
	是对board_sysconf.c的补充0	
*/

/*-----------------------------------------------------------*/
/*	选择触摸屏检测方案: 使用触摸IC 或     内部ADC转换*/
#define SYS_USE_TS_IC_CASE		1
//#define SYS_USE_TS_ADC_CASE 	0

/*触摸屏检测方案：xpt2046 方案和ADC方案只能选一种*/
#if defined(SYS_USE_TS_IC_CASE) && defined(SYS_USE_TS_ADC_CASE)
 #error "please just select one touch device!"
#endif

/*	定义xpt 2046使用 模拟SPI1_ch1
	分频设置为0      */
#define SYS_USE_VSPI1	1	
#define XPT2046_SPI "VSPI1_CH1"
#define XPT2046_SPI_KHZ	  (2000)

#if 0
/*	如果使用硬件SPI3_CH4,
	分频不能设置太快*/
//#define XPT2046_SPI "SPI3_CH4"
//#define XPT2046_SPI_PRE	SPI_BaudRatePrescaler_8
#endif
/*-----------------------------------------------------------*/
#define ALBATROSS_TASK_STK_SIZE (1024)
#define ALBATROSS_TASK_PRIO	1

#define START_TASK_STK_SIZE (512)
#define START_TASK_PRIO	3//


/*-----------------------------------------------------------*/

#define SYS_FS_NUM 0

#define VFS_SD_DIR	"mtd0"//sd卡文件系统挂载vfs中的目录名
#define SYS_FS_FATFS_SD "1:/"//sd卡目录名， 这个名字跟diso.c中的序号有关系

#define VFS_USB_DIR	"mtd1"//USB
#define SYS_FS_FATFS_USB "2:/"
/*-----------------------------------------------------------*/

/*	字体类型定义 */
#define FONT_LIST_MAX 4	
extern struct fbcon_font_desc *FontList[FONT_LIST_MAX];


#endif


