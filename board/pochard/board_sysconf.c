/**
 * @file            wujique_sysconf.c
 * @brief           系统管理
 * @author          wujique
 * @date            2018年4月9日 星期一
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年4月9日 星期一
 *   作    者:         wujique
 *   修改内容:   创建文件
*/
#include "mcu.h"

#include "board_sysconf.h"
#include "petite.h"

#include "log.h"	
#include "drv_spiflash.h"
#include "drv_lcd.h"
#include "touch.h"

/*
	本文件用于配置系统有哪些设备和资源
*/
/*-------------------------------
	I2C控制器
-------------------------------*/
const DevI2c DevVi2c0={
		.pdev={
				.name = "VI2C0",
				.type = BUS_I2C_V,

				.basebus = NULL,
				.busconf = NULL,
				.basetype = DEV_NULL,
		},
		
		.sclport = MCU_PORT_B,
		.sclpin = MCU_IO_8,

		.sdaport = MCU_PORT_B,
		.sdapin = MCU_IO_9,
		};
	
/*
	硬件SPI控制器：SPI1
	如果添加其他控制器，请修改mcu_spi.c中的硬件SPI控制器初始化代码
*/
const DevSpi DevSpi1IO={
		.pdev={
				.name = "SPI1",
				.type = BUS_SPI_H,
				.basebus = NULL,
				.busconf = NULL,
				.basetype = DEV_NULL,
		},
		/*clk*/
		.clkport = MCU_PORT_B,
		.clkpin = MCU_IO_3,
		/*mosi*/
		.mosiport = MCU_PORT_A,
		.mosipin = MCU_IO_7,
		/*miso*/
		.misoport = MCU_PORT_B,
		.misopin = MCU_IO_4,
	};

/*------------------------ 
	SPI通道
-------------------------*/
/* 	FLASH用  */
const DevSpiCh DevSpi1CH1={
		.pdev={
			.name = "SPI1_CH1",
			.type = BUS_SPI_CH,

			.basebus = "SPI1",
			.busconf = NULL,
			.basetype = BUS_SPI_H,
		},					
		
		.csport = MCU_PORT_E,
		.cspin = MCU_IO_1,
		
	};		


/*-------------------------------------
	LCD硬件接口总线定义
	LCD硬件接口包含一个基本通信接口basebus、A0管脚，复位管脚，背光管脚。
---------------------------------------*/

/*	8080接口的LCD总线 */	
const DevLcdCtrlIO BusLcd8080={

	/* 8080 不用A0*/
	.A0port = MCU_PORT_NULL,
	.A0pin = NULL,

	.rstport = MCU_PORT_D,
	.rstpin = MCU_IO_3,
	
	.blport = MCU_PORT_B,
	.blpin = MCU_IO_0,

};


/*----------------------------
	FLASH设备定义
-----------------------------*/
const DevSpiFlash DevSpiFlashCore={
	/*有一个叫做board_spiflash的SPI FLASH挂在DEV_SPI_1_1上，型号未知*/
	.pdev={
				.name = "board_spiflash",
				.type = DEV_SPIFLASH,
				.basebus = "SPI1_CH1",
				.busconf = NULL,
				.basetype = BUS_SPI_H,
			},

	.pra = NULL,
};


/* 屋脊雀2.8寸屏幕 */
const DevLcd DevLcdtTFT	=	{
	.pdev={
				.name = "tftlcd",
				.type = DEV_LCD,

				.basebus = "bus_8080",
				.busconf = NULL,
				.basetype = BUS_8080,
			},

	.ctrlio = &BusLcd8080,
 
	.id = NULL, 
	/* 指定lcd 尺寸 */
	.width = 320, 
	.height = 480,
};


const DevTouch BoardDevTp = {
	.name = "board_ctp_g1158",

	/** 触摸屏像素 */
	.w = 320,
	.h = 480,

	/* 电阻屏或电容屏*/
	.type = TP_TYPE_CTP,
	/* 用于匹配 驱动*/
	.icid = 0x1158,

	.rstport = MCU_PORT_NULL,
	.rstpin = NULL,

	.intport = MCU_PORT_NULL,
	.intpin = NULL,

	.basebus = "VI2C0",


};



/* 	简单设备树   	*/
void *PetiteDevTable[]={
	/*注册I2C总线*/
	(void *)(&DevVi2c0),
					
	/*硬SPI3控制器，核心板和底板的FLASH、外扩接口的SPI口*/
	(void *)(&DevSpi1IO),
		(void *)(&DevSpi1CH1),
			(void *)(&DevSpiFlashCore),			

	(void *)(&DevLcdtTFT),

	/* dont move*/
	NULL,
	};

/*
	定义板存储与分区。
	1.在进行 partition初始化之前，设备要先完成初始化。
	2.非系统映射Flash，最好映射到芯片保留地址上。
	3.partition会判断分区格式，有需要则进行初始化
		app:程序
		data:纯数据，无指定格式
		par: partiton表
		flashdb：KV数据库
		constfs: 资源文件系统
		littlefs: flash文件系统
	*/
#if 1
#include "partition.h"
	/*
		 先定义sto设备，再定义par分区
		*/ 
	PartitionDef PetitePartitonTable[] =
	{
	#if 0
		/* 407 芯片内部Flash */
		{"sto","onchip","7b0_flash",	  0x8000000,  0x20000},
			{"par","app","boot",		0x8000000,	0x10000},
			{"par","flashdb","boot",	0x8010000,	0x10000},
	
	#endif
		/* 底板SPI  flash */
		{"sto","spiflash","board_spiflash", 0x50000000, 0x800000},
			{"par", VFS_STR_LITTLEFS, "mtd0",	  0x50000000, 0x100000},//1M 文件系统，sqlite基于文件系统
			{"par","FlashDB","flashdb", 	0x50000000+0x100000, 0x20000},//128K的kv
		/* 把sd卡也放到partiton，以便将其初始化为fatfs后挂载到vfs中 
			目前sdmmc并不归 partition管理。
			addr 和 size， 和实际不符，32位寻址最大才4G，sd卡早超出了 */
		//{"sto", "sdmmc",  "socket1", 0x70000000, 0},
		//	{"par",VFS_STR_FATFS, SYS_FS_FATFS_SD,	  0x70000000, 0},
		/*--------------------------------------------------*/
		{NULL,NULL,NULL,0,0},
	};
#endif


