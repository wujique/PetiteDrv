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
		
		.sclport = MCU_PORT_F,
		.sclpin = MCU_IO_1,

		.sdaport = MCU_PORT_F,
		.sdapin = MCU_IO_0,
		};

/* 	IO口模拟的I2C1
	WM8978、TEA5767、外扩接口的I2C使用 */
const DevI2c DevVi2c1={
		.pdev={
				.name = "VI2C1",
				.type = BUS_I2C_V,
				.basebus = NULL,
				.busconf = NULL,
				.basetype = DEV_NULL,
			},
		
		.sclport = MCU_PORT_D,
		.sclpin = MCU_IO_6,

		.sdaport = MCU_PORT_D,
		.sdapin = MCU_IO_7,
		};
	
		
/*----------------------
	IO口模拟SPI控制器
------------------------*/
/* 	VSPI1，核心板上的LCD接口中的4根IO模拟SPI，
	用于XPT2046方案触摸处理，可读可写。*/					
const DevSpi DevVSpi1IO={
		.pdev={
				.name = "VSPI1",
				.type = BUS_SPI_V,
				.basebus = NULL,
				.busconf = NULL,
				.basetype = DEV_NULL,
			},
		/*clk*/
		.clkport = MCU_PORT_B,
		.clkpin = MCU_IO_0,
		/*mosi*/
		.mosiport = MCU_PORT_D,
		.mosipin = MCU_IO_11,
		/*miso*/
		.misoport = MCU_PORT_D,
		.misopin = MCU_IO_12,
	};

#if 0
/* 	模拟SPI，无miso
	用于测试SPI程序框架 */
const DevSpi DevVspi3IO={
		.pnode={
				.name = "VSPI3",
				.type = BUS_SPI_V,
			},
		/*clk*/
		.clkport = MCU_PORT_G,
		.clkpin = MCU_IO_6,
		
		/*mosi*/
		.mosiport = MCU_PORT_F,
		.mosipin = MCU_IO_2,

		/*miso*/
		.misoport = MCU_PORT_NULL,
		.misopin = NULL,

	};
#endif

#if 0
/*  外扩接口模拟VSPI2， 与矩阵键盘，模拟I2C2冲突          */			
const DevSpi DevVspi2IO={
		.pnode={
				.name = "VSPI2",
				.type = BUS_SPI_V,
			},
		/*clk*/
		.clkport = MCU_PORT_F,
		.clkpin = MCU_IO_11,
		
		/*mosi*/
		.mosiport = MCU_PORT_F,
		.mosipin = MCU_IO_10,

		/*miso*/
		.misoport = MCU_PORT_F,
		.misopin = MCU_IO_9,

	};
#endif
/*
	硬件SPI控制器：SPI3
	SPI驱动暂时支持SPI3，
	如果添加其他控制器，请修改mcu_spi.c中的硬件SPI控制器初始化代码
*/
const DevSpi DevSpi3IO={
		.pdev={
				.name = "SPI3",
				.type = BUS_SPI_H,
				.basebus = NULL,
				.busconf = NULL,
				.basetype = DEV_NULL,
		},
		/*clk*/
		.clkport = MCU_PORT_B,
		.clkpin = MCU_IO_3,
		/*mosi*/
		.mosiport = MCU_PORT_B,
		.mosipin = MCU_IO_5,
		/*miso*/
		.misoport = MCU_PORT_B,
		.misopin = MCU_IO_4,
	};

/*------------------------ 
	SPI通道
-------------------------*/
/* 	FLASH用  */
const DevSpiCh DevSpi3CH1={
		.pdev={
			.name = "SPI3_CH1",
			.type = BUS_SPI_CH,

			.basebus = "SPI3",
			.busconf = NULL,
			.basetype = BUS_SPI_H,
		},					
		
		.csport = MCU_PORT_B,
		.cspin = MCU_IO_14,
		
	};		
/*	FLASH用 */
const DevSpiCh DevSpi3CH2={
		.pdev={
				.name = "SPI3_CH2",
				.type = BUS_SPI_CH,
				.basebus = "SPI3",
				.busconf = NULL,
				.basetype = BUS_SPI_H,
			},		
		
		.csport = MCU_PORT_G,
		.cspin = MCU_IO_15,
		
	};
/* 	外扩接口的SPI，可接COG、OLED、SPI TFT、RF24L01 */			
const DevSpiCh DevSpi3CH3={
		.pdev={
				.name = "SPI3_CH3",
				.type = BUS_SPI_CH,
				.basebus = "SPI3",
				.busconf = NULL,
				.basetype = BUS_SPI_H,
			},
			
		.csport = MCU_PORT_G,
		.cspin = MCU_IO_6,
		
	};
		
#if 0		
/*
	外扩接口的SPI, 
	跟DevSpi3CH3用相同的IO，
	主要是接SPI接口的LCD,
	同时带触摸屏XPT2046，
	本通道就是控制XPT2046的。

	但是实际上，SPI接口的TFT 2.7寸LCD太慢了，
	只能用来测试程序框架。
*/
const DevSpiCh DevSpi3CH4={
		.name = "SPI3_CH4",
		.spi = "SPI3",
		
		.csport = MCU_PORT_F,
		.cspin = GPIO_Pin_2,
		
	};
#endif

#if 0
/*	模拟SPI通道，无CS
	用来调试没有CS的LCD屏幕，
	VSPI3，其实是外扩接口SPI附近的两个IO模拟的。*/	
const DevSpiCh DevVSpi3CH1={
		.pnode={
				.name = "VSPI3_CH1",
				.type = BUS_SPI_CH,
			},
		
		.spi = "VSPI3",
		
		.csport = MCU_PORT_NULL,
		.cspin = NULL,
		
	};
#endif

/* 	LCD座子中的触摸屏接口, IO模拟SPI*/
const DevSpiCh DevVSpi1CH1={
		.pdev={
				.name = "VSPI1_CH1",
				.type = BUS_SPI_CH,
				.basebus = "VSPI1",
				.busconf = NULL,
				.basetype = BUS_SPI_V,
			},
		
		.csport = MCU_PORT_B,
		.cspin = MCU_IO_1,
		
	};
#if 0		
/* 
	SPI彩屏，跟触摸屏用相同的控制器
*/		
const DevSpiCh DevVSpi1CH2={
		.name = "VSPI1_CH2",
		.spi = "VSPI1",
		
		.csport = MCU_PORT_D,
		.cspin = GPIO_Pin_14,
	};
#endif

#if 0
/*
	外扩IO VSPI通道
*/
const DevSpiCh DevVSpi2CH1={
		.name = "VSPI2_CH1",
		.spi = "VSPI2",
		
		.csport = MCU_PORT_F,
		.cspin = GPIO_Pin_12,
		
	};
#endif	

/*-------------------------------------
	LCD硬件接口总线定义
	LCD硬件接口包含一个基本通信接口basebus、A0管脚，复位管脚，背光管脚。
---------------------------------------*/
#if 1

#else
/*
	用来接没有CS和MISO的1.33寸LCD屏
*/
const DevLcdBus BusLcdVSpi3={
	.name = "BusLcdVSpi3",
	.type = LCD_BUS_SPI,
	.basebus = "VSPI3_CH1",

	.A0port = MCU_PORT_G,
	.A0pin = GPIO_Pin_4,

	.rstport = MCU_PORT_G,
	.rstpin = GPIO_Pin_7,

	.blport = MCU_PORT_G,
	.blpin = GPIO_Pin_9,
};

#endif 

/*	8080接口的LCD总线 */	
const DevLcdCtrlIO BusLcd8080={

	/* 8080 不用A0*/
	.A0port = MCU_PORT_NULL,
	.A0pin = NULL,

	.rstport = MCU_PORT_A,
	.rstpin = MCU_IO_15,
	
	.blport = MCU_PORT_B,
	.blpin = MCU_IO_15,

};


#if 0
/* 模拟SPI2（外扩IO）可接SPI接口的屏 */
const DevLcdBus BusLcdVSpi2CH1={
	.name = "BusLcdVSpi2CH1",
	.type = LCD_BUS_SPI,
	.basebus = "VSPI2_CH1",

	.A0port = MCU_PORT_F,
	.A0pin = GPIO_Pin_8,

	.rstport = MCU_PORT_F,
	.rstpin = GPIO_Pin_13,

	.blport = MCU_PORT_F,
	.blpin = GPIO_Pin_14,

	.staport = MCU_PORT_F, 
	.stapin = GPIO_Pin_15,
};
#endif

#if 0
/*
	
*/
const DevLcdBus BusLcdVSpi1CH2={
	.name = "BusLcdVSpi1CH2",
	.type = LCD_BUS_SPI,
	.basebus = "VSPI1_CH2",

	.A0port = MCU_PORT_D,
	.A0pin = GPIO_Pin_15,

	.rstport = MCU_PORT_A,
	.rstpin = GPIO_Pin_15,

	.blport = MCU_PORT_B,
	.blpin = GPIO_Pin_15,
};
#endif
/*----------------------------
	FLASH设备定义
-----------------------------*/
const DevSpiFlash DevSpiFlashCore={
	/*有一个叫做board_spiflash的SPI FLASH挂在DEV_SPI_3_2上，型号未知*/
	.pdev={
				.name = "board_spiflash",
				.type = DEV_SPIFLASH,
				.basebus = "SPI3_CH2",
				.busconf = NULL,
				.basetype = BUS_SPI_H,
			},

	.pra = NULL,
};

const DevSpiFlash DevSpiFlashBoard={
	.pdev={
				.name = "core_spiflash",
				.type = DEV_SPIFLASH,
				.basebus = "SPI3_CH1",
				.busconf = NULL,
				.basetype = BUS_SPI_H,
			},
	
	.pra = NULL,
};

/*--------------------------------
	lcd设备树定义
	指明系统有多少个LCD设备，
--------------------------------*/
/*	I2C接口的LCD总线*/
const DevLcdCtrlIO BusLcdI2C1={

	/*I2C接口的LCD总线，不需要其他IO*/
	.A0port = MCU_PORT_NULL,
	.A0pin = NULL,

	.rstport = MCU_PORT_NULL,
	.rstpin = NULL,

	.blport = MCU_PORT_NULL,
	.blpin = NULL,

	.staport = MCU_PORT_NULL, 
	.stapin = NULL,
};

/*I2C接口的 OLED*/
const I2CPra OledLcdI2cPra ={
	.addr = 0x3D, /* 0x3c or 0x3d,  在cuckoo板子上 ,和摄像头共用I2C，OLED屏需要改为0x3C地址， */
	.fkhz = 200,/* 时钟频率，单位KHz */
};

const DevLcd DevLcdOled1={
	.pdev={
				.name = "i2coledlcd",
				.type = DEV_LCD,

				.basebus = "VI2C1",
				.busconf = (void *)&OledLcdI2cPra,
				.basetype = BUS_I2C_V,
		},

	.ctrlio = &BusLcdI2C1,
	
	.id = 0X1315, 
	.width = 64, 
	.height = 128,

	.i2c_cmd_reg = 0x00,
	.i2c_data_reg = 0x40,
};


/*	spi cog/oled LCD接口，使用真正的SPI控制
	外扩接口中的SPI接口 */
const DevLcdCtrlIO BusLcdSpi3={

	.A0port = MCU_PORT_G,
	.A0pin = MCU_IO_4,

	.rstport = MCU_PORT_G,
	.rstpin = MCU_IO_7,

	.blport = MCU_PORT_G,
	.blpin = MCU_IO_9,

	.staport = MCU_PORT_F, 
	.stapin = MCU_IO_2,
};

/*SPI接口的 OLED*/
const PraSpiSet CogSpiSet = {
	.mode = SPI_MODE_3,
	.KHz = 10000,
};
	
#if 0
DevLcd DevLcdSpiOled	=	{	
	.pnode={
				.name = "spioledlcd",
				.type = DEV_LCD,
		},
	
	.id = 0X1315, 
	.width = 64, 
	.height = 128,
	
	.bus = &BusLcdSpi3,  
	.buspra = (void *)&CogSpiSet,
};
#endif
/*SPI接口的 COG LCD*/
const DevLcd DevLcdCOG1	=	{
	.pdev={
				.name = "spicoglcd",
				.type = DEV_LCD,

				.basebus = "SPI3_CH3",
				.busconf = (void *)&CogSpiSet,
				.basetype = BUS_SPI_CH,
		},

	.ctrlio = &BusLcdSpi3,
	
	.id = 0X7565, 
	.width = 64, 
	.height = 128,

};
#if 0
const PraSpiSet TftSpiSet = {
	.mode = SPI_MODE_3,
	.KHz = 10000,
};

const DevLcd DevSpiLcdTFT	=	{

	.pnode={
				.name = "spitftlcd",
				.type = DEV_LCD,
		},
		
	/*1.44寸 中景园 128*128 tft lcd */
	#if 0	
	.id = 0x7735, 
	.width = 128, 
	.height = 128,
	#endif
	
	#if 1	
	/* 1.3寸，IPS，中景园，只有SCL&SDA的SPI接口LCD*/
	.id = 0x7789, 
	.width = 128, 
	.height = 128,
	#endif
	
	.bus = &BusLcdSpi3,  
	.buspra = (void *)&TftSpiSet,
};

const PraSpiSet EpaperSpiSet = {
	.mode = SPI_MODE_3,
	.KHz = 10000,
};

const DevLcd DevEpaper	=	{

	.pnode={
				.name = "spiE-Paper",
				.type = DEV_LCD,
		},
		
	/* 大连佳显 spi 接口的三色墨水屏 GDEW027C44*/
	#if 1	
	.id = 0x9187, 
	.width = 176, 
	.height = 264,
	#endif
	
	#if 0	
	/* 大连佳显 spi 接口 黑白墨水屏 1.54寸 GDEH154D27*/
	.id = 0x3820, 
	.width = 200, 
	.height = 200,
	#endif
	
	.bus = &BusLcdSpi3,  
	.buspra = (void *)&EpaperSpiSet,
};
#endif
/*fsmc接口的 tft lcd*/
#if 1
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
	/* 可以指定id为：
		0x9325，WJQ, 240, 320
		0x9341, WJQ, 240, 320 
		0x1408, 好巨润, 480 800, 4.0寸的IPS屏幕，r61408驱动芯片 */	 
	.id = NULL, 
	/* 指定lcd 尺寸 */
	.width = 240, 
	.height = 320,
};
#endif	

/*SPI接口的 tft lcd*/
//const DevLcd DevLcdtTFT	=	{"spitftlcd", 		"BusLcdSpi3", 	0x9342, 240, 320};
//const DevLcd DevLcdtTFT	=	{"spitftlcd", 		"BusLcdVSpi1CH2", 	0x9342, 240, 320};

/* 	简单设备树   	*/
void *PetiteDevTable[]={
	/*注册I2C总线*/
	(void *)(&DevVi2c0),
	
	(void *)(&DevVi2c1),
		(void *)(&DevLcdOled1),
					
	/*硬SPI3控制器，核心板和底板的FLASH、外扩接口的SPI口*/
	(void *)(&DevSpi3IO),
		(void *)(&DevSpi3CH1),
			(void *)(&DevSpiFlashBoard),		
		(void *)(&DevSpi3CH2),
			(void *)(&DevSpiFlashCore),			
		(void *)(&DevSpi3CH3),
			//(void *)(&DevEpaper),
			(void *)(&DevLcdCOG1),
			//(void *)(&DevLcdSpiOled);
			//(void *)(&DevSpiLcdTFT),
		//(void *)(&DevSpi3CH4);
	
#if (SYS_USE_VSPI1 == 1)
	(void *)(&DevVSpi1IO),
		(void *)(&DevVSpi1CH1),//8080接口的触摸屏
		//(void *)(&DevVSpi1CH2);
#endif
	
#if (SYS_USE_VSPI2 == 1)
	(void *)(&DevVspi2IO),
		(void *)(&DevVSpi2CH1),
			(void *)(&DevLcdSpiOled),
#endif
	
	/*测试1.33寸IPS屏幕采用*/
#if 0
	(void *)(&DevVspi3IO),
		(void *)(&DevVSpi3CH1),
			(void *)(&DevLcdVSPITFT),
#endif
	(void *)(&DevLcdtTFT),

	/* dont move*/
	NULL,
	};


const DevTouch BoardDevTp = {
	.name = "board_rtp_xtp2046",

	/** 触摸屏像素 */
	.w = 240,
	.h = 320,

	/* 电阻屏或电容屏*/
	.type = TP_TYPE_RTP,
	/* 用于匹配 驱动*/
	.icid = 0x2046,

	.rstport = MCU_PORT_NULL,
	.rstpin = NULL,

	.intport = MCU_PORT_NULL,
	.intpin = NULL,

	.basebus = "VSPI1_CH1",


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
			{"par", VFS_STR_LITTLEFS, "mtd1",	  0x50000000, 0x100000},//1M 文件系统，sqlite基于文件系统
			
		/* 核心板 SPI flash */
		{"sto","spiflash","core_spiflash", 0x60000000, 0x800000},
			{"par","FlashDB","flashdb", 	0x60000000, 0x20000},//128K的kv
			{"par", VFS_STR_LITTLEFS, "mtd0",	  0x60000000+0x20000, 0x100000},//1M 文件系统，sqlite基于文件系统
			
		/* 把sd卡也放到partiton，以便将其初始化为fatfs后挂载到vfs中 
			目前sdmmc并不归 partition管理。
			addr 和 size， 和实际不符，32位寻址最大才4G，sd卡早超出了 */
		{"sto", "sdmmc",  "socket1", 0x70000000, 0},
			{"par",VFS_STR_FATFS, SYS_FS_FATFS_SD,	  0x70000000, 0},
		/*   如何实现热插拔？*/	
		{"sto", "usbmsc",  "socket0", 0x80000000, 0},
			{"par",VFS_STR_FATFS, SYS_FS_FATFS_USB,	  0x80000000, 0},
		/*--------------------------------------------------*/
		{NULL,NULL,NULL,0,0},
	};
#endif


