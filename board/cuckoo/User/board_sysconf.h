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

/* 配置 mcu接口类型 */
#define MCU_STM32H7XX

/* */
#define PC_PORT  MCU_UART_4 

#define LOG_BOARD_TAG "BOARD"
#define LogBoard(l,args...) petite_log(l, LOG_BOARD_TAG, NULL,__FUNCTION__, __LINE__, ##args);
/* 
	本文件定义板设备情况。
	是对board_sysconf.c的补充0	
*/

/*--------------------- 任务定定义 ------------------------------*/
#define CUCKOO_TASK_STK_SIZE (1024*16)
#define CUCKOO_TASK_PRIO	1

#define CMD_TASK_STK_SIZE (1024*4)
#define CMD_TASK_PRIO	2


#define START_TASK_STK_SIZE (1024*2)
#define START_TASK_PRIO	3//


/*-----------------------------------------------------------*/
//sd卡 fatfs盘符， 数字可以变，格式不能变
#define SDPath "0:"

#define QSPI_FLASH_LITTLEFS_DIR "mtd1"
#define QSPI_FLASH_LITTLEFS_ADDR	0x90400000
#define QSPI_FLASH_LITTLEFS_SIZE 	0x300000

/*-----------------------------------------------------------*/

/*	字体类型定义 */
#define FONT_LIST_MAX 4	

#define OLED_I2C_ADDR 0x3d


#define DEV_WM8978_I2CBUS "VI2C1"
#define DEV_CAMERA_I2CBUS "VI2C1"

#define XPT2046_SPI "VSPI1_CH1"
#define XPT2046_SPI_KHZ	 2000


/*    rgb lcd 定义       */
//#define ATK4342 1
#define WKS43WQ 1

/* 矿工电子的4.3寸 LCD */
#ifdef ATK4342
/* 261120Byte显存*/
#define  LCD_WIDTH    ((uint16_t)480)          /* LCD PIXEL WIDTH            */
#define  LCD_HEIGHT   ((uint16_t)272)          /* LCD PIXEL HEIGHT           */

#define  LCD_HSYNC            ((uint16_t)1)   /* Horizontal synchronization */
#define  LCD_HBP              ((uint16_t)40)   /* Horizontal back porch      */
#define  LCD_HFP              ((uint16_t)5)   /* Horizontal front porch     */

#define  LCD_VSYNC            ((uint16_t)1)   /* Vertical synchronization   */
#define  LCD_VBP              ((uint16_t)8)    /* Vertical back porch        */
#define  LCD_VFP              ((uint16_t)8)    /* Vertical front porch       */
#endif

#ifdef WKS43WQ
/* 768K显存*/
#define  LCD_WIDTH    ((uint16_t)800)          /* LCD PIXEL WIDTH            */
#define  LCD_HEIGHT   ((uint16_t)480)          /* LCD PIXEL HEIGHT           */

#define  LCD_HSYNC            ((uint16_t)48)   /* Horizontal synchronization */
#define  LCD_HBP              ((uint16_t)88)   /* Horizontal back porch      */
#define  LCD_HFP              ((uint16_t)40)   /* Horizontal front porch     */

#define  LCD_VSYNC            ((uint16_t)3)   /* Vertical synchronization   */
#define  LCD_VBP              ((uint16_t)32)    /* Vertical back porch        */
#define  LCD_VFP              ((uint16_t)13)    /* Vertical front porch       */
#endif

#endif


