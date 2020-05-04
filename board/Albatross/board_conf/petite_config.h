#ifndef __PETITE_CONFIG_H__
#define __PETITE_CONFIG_H__

/* 配置PetiteDrv */

/* 设备名长度 */
#define DEV_NAME_SIZE	16

/* 这里的定义决定是否编译对应的驱动 */

/* 中间件配置 */
#define PANEL_SOUND_MODULE	0
#define PANEL_SOUND_I2S		0
#define PANEL_SOUND_DAC		0
#define PANEL_EMENU_MODULE	1
#define DRV_SDIO_MODULE		0
#define PANEL_FONT_MODULE	0//asc字符是标配，本宏控制是否用汉字点阵

/* dev */
#define DRV_RS485_MODULE 		0
#define DRV_KEYPAD_MODULE 		0
#define DRV_SPI_FLASH_MODULE 	0
#define DRV_WM8978_MODULE   	0
#define DRV_XPT2046_MODULE		1
#define DRV_HTU21U_MODULE		0
#define DRV_HCOC_MODULE			0

/* 摄像头功能 */
#define DRV_CAMERA_MODULE		0
#define DRV_CAMERA_OV2640 		0
#define DRV_CAMERA_OV9655	 	0

/* 	用宏控制哪些LCD驱动参加编译*/
#define LCD_DRIVER_ST7565		1
#define LCD_DRIVER_SSD1615		1
#define LCD_DRIVER_9341			1
#define LCD_DRIVER_9341_8BIT	0
#define LCD_DRIVER_9325			1
#define LCD_DRIVER_7735			0
#define LCD_DRIVER_7789			0
#define LCD_DRIVER_91874		0
#define LCD_DRIVER_3820			0
#define LCD_DRIVER_NT35510		1
#define LCD_DRIVER_R61408		1

/* 是否使用RTOS */
#define SYS_USE_RTOS 1

/* 文件系统 */
#define SYS_USE_VFS		0
#define SYS_FS_FATFS	0
#define SYS_FS_LITTLEFS	0
#define SYS_FS_SPIFS	0

/*定义alloc.c内存管理数组*/
#define AllocArraySize (80*1024)


#endif

