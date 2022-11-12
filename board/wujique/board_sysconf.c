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

/*
	本文件用于配置系统有哪些设备和资源
*/
/*-------------------------------
	I2C控制器
-------------------------------*/
const DevI2c DevVi2c0={
		.pnode={
				.name = "VI2C0",
				.type = BUS_I2C_V,
				},
		
		.sclport = MCU_PORT_F,
		.sclpin = MCU_IO_1,

		.sdaport = MCU_PORT_F,
		.sdapin = MCU_IO_0,
		};

/* 	IO口模拟的I2C1
	WM8978、TEA5767、外扩接口的I2C使用 */
const DevI2c DevVi2c1={
		.pnode={
				.name = "VI2C1",
				.type = BUS_I2C_V,
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
		.pnode={
				.name = "VSPI1",
				.type = BUS_SPI_V,
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
		.pnode={
				.name = "SPI3",
				.type = BUS_SPI_H,
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
		.pnode={
				.name = "SPI3_CH1",
				.type = BUS_SPI_CH,
		},					
		.spi = "SPI3",
		
		.csport = MCU_PORT_B,
		.cspin = MCU_IO_14,
		
	};		
/*	FLASH用 */
const DevSpiCh DevSpi3CH2={
		.pnode={
				.name = "SPI3_CH2",
				.type = BUS_SPI_CH,
			},		
	
		.spi = "SPI3",
		
		.csport = MCU_PORT_G,
		.cspin = MCU_IO_15,
		
	};
/* 	外扩接口的SPI，可接COG、OLED、SPI TFT、RF24L01 */			
const DevSpiCh DevSpi3CH3={
		.pnode={
				.name = "SPI3_CH3",
				.type = BUS_SPI_CH,
			},
			
		.spi = "SPI3",
		
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
		.pnode={
				.name = "VSPI1_CH1",
				.type = BUS_SPI_CH,
			},
		
		.spi = "VSPI1",
		
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
/*	spi cog/oled LCD接口，使用真正的SPI控制
	外扩接口中的SPI接口 */
const DevLcdBus BusLcdSpi3={
	.pnode={
				.name = "BusLcdSpi3",
				.type = BUS_LCD_SPI,
			},
	
	.basebus = "SPI3_CH3",

	.A0port = MCU_PORT_G,
	.A0pin = MCU_IO_4,

	.rstport = MCU_PORT_G,
	.rstpin = MCU_IO_7,

	.blport = MCU_PORT_G,
	.blpin = MCU_IO_9,

	.staport = MCU_PORT_F, 
	.stapin = MCU_IO_2,
};
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
/*	I2C接口的LCD总线*/
const DevLcdBus BusLcdI2C1={
	.pnode={
				.name = "BusLcdI2C1",
				.type = BUS_LCD_I2C,
			},
			
	.basebus = "VI2C1",

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
/*	8080接口的LCD总线 */	
const DevLcdBus BusLcd8080={
	.pnode={
				.name = "BusLcd8080",
				.type = BUS_LCD_8080,
			},
	
	.basebus = "8080",//无意义，8080操作直接嵌入在LCD BUS代码内

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
	.pnode={
				.name = "board_spiflash",
				.type = DEV_SPIFLASH,
			},
	
	.spich ="SPI3_CH2", 
	.pra = NULL,
};

const DevSpiFlash DevSpiFlashBoard={
	.pnode={
				.name = "core_spiflash",
				.type = DEV_SPIFLASH,
			},
	
	.spich ="SPI3_CH1", 
	.pra = NULL,
};

/*--------------------------------
	lcd设备树定义
	指明系统有多少个LCD设备，
--------------------------------*/

/*I2C接口的 OLED*/
const I2CPra OledLcdI2cPra ={
	.addr = 0x3D, /* 0x3c or 0x3d,  在cuckoo板子上 ,和摄像头共用I2C，OLED屏需要改为0x3C地址， */
	.fkhz = 200,/* 时钟频率，单位KHz */
};

const DevLcd DevLcdOled1={
	.pnode={
				.name = "i2coledlcd",
				.type = DEV_LCD,
		},
		  
	.id = 0X1315, 
	.width = 64, 
	.height = 128,

	.bus = &BusLcdI2C1,  
	.buspra = (void *)&OledLcdI2cPra,

	.i2c_cmd_reg = 0x00,
	.i2c_data_reg = 0x40,
};

/*SPI接口的 OLED*/
const PraSpiSet CogSpiSet = {
	.mode = SPI_MODE_3,
	.KHz = 10000,
};

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

/*SPI接口的 COG LCD*/
const DevLcd DevLcdCOG1	=	{

	.pnode={
				.name = "spicoglcd",
				.type = DEV_LCD,
		},
		
	.id = 0X7565, 
	.width = 64, 
	.height = 128,

	.bus = &BusLcdSpi3,  
	.buspra = (void *)&CogSpiSet,
};

/*fsmc接口的 tft lcd*/
#if 1
/* 屋脊雀2.8寸屏幕 */
const DevLcd DevLcdtTFT	=	{
	.pnode={
				.name = "tftlcd",
				.type = DEV_LCD,
			},
	/* 可以指定id为：
		0x9325，WJQ, 320 240
		0x9341, WJQ, 320 240
		0x1408, 好巨润, 480 800, 4.0寸的IPS屏幕，r61408驱动芯片
				*/	 
	.id = NULL, 
	/* 指定lcd 尺寸 */
	.width = 240, 
	.height = 320,

	.bus = &BusLcd8080,  
	.buspra = (void *)&CogSpiSet,
};
#endif	

/*SPI接口的 tft lcd*/
//const DevLcd DevLcdtTFT	=	{"spitftlcd", 		"BusLcdSpi3", 	0x9342, 240, 320};
//const DevLcd DevLcdtTFT	=	{"spitftlcd", 		"BusLcdVSpi1CH2", 	0x9342, 240, 320};
/*1.44寸 中景园*/
//const DevLcd DevSpiLcdtTFT	=	{"spitftlcd",   "BusLcdSpi3", 	0x7735, 128, 128};
/* 1.3寸，IPS，中景园，只有SCL&SDA的SPI接口LCD*/
//const DevLcd DevLcdVSPITFT =	{"vspitftlcd",		"BusLcdVSpi3",	0x7789, 240, 240};
/* spi 接口的三色墨水屏 */
//const DevLcd DevLcdSPIEPaper =	{"spiE-Paper",		"BusLcdSpi3",	0x9187, 176, 264};
/* spi 接口 黑白墨水屏 1.54寸 GDEH154D27*/
//const DevLcd DevLcdSPIEPaper =	{"spiE-Paper",		"BusLcdSpi3",	0x3820, 200, 200};


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
			//(void *)(&DevLcdSPIEPaper);
			(void *)(&DevLcdCOG1),
			//(void *)(&DevLcdSpiOled);
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


/*	字库定义		*/

/* 思源宋体 用字模工具生成的18030字库
	从矢量字体转来的，某些字效果非常不好
	仅供测试

	保存在文件系统
	*/
FontHead SYSongTi1212 ={

	.name = "SYsongti_12",//名字
	.size = 12,
	.path = "mtd0/1:font/songti1212.DZK",//路径
		//"mtd0/1:/shscn1212.DZK",
		
	.type = FONT_DOT_YMY,
	.st = FONT_ST_GB18030,
	.dt = FONT_V_H_U_D_L_R,
	.shift = 0,
	
	.bitmap={
		/* 以下五个数据在LCD描字时需要 */
		.rows = 12,
		.width = 12,
		.pitch = 2,
		.left = 0,
		.top = 10,
	},
	.datac = 24,

	
	};
	
FontHead SYSongTi1616 ={

	.name = "SYsongti_16",//名字
	.size = 16,
	.path = "mtd0/1:font/shscn1616.DZK",//路径
			//"mtd0/1:/songti1616.DZK",

	.type = FONT_DOT_YMY,
	.st = FONT_ST_GB18030,
	.dt = FONT_V_H_U_D_L_R,
	.shift = 0,
	
	.bitmap={
		/* 以下五个数据在LCD描字时需要 */
		.rows = 16,
		.width = 16,
		.pitch = 2,
		.left = 0,
		.top = 14,
	},
	.datac = 32,

	
	};
/*
	屋脊雀优化的点阵，基于思源宋体
	*/	
FontHead SYSongTiM1616 ={

	.name = "SYST_16_m",//名字
	.size = 16,
	.path = "mtd0/1:font/syst_m_16.bin",//路径
	
	.type = FONT_DOT_WJQ,
	.st = FONT_ST_GB18030,
	.dt = FONT_H_H_L_R_U_D,
	.shift = 0,
	
	.bitmap={
		/* 以下五个数据在LCD描字时需要 */
		.rows = 16,
		.width = 16,
		.pitch = 2,
		.left = 0,
		.top = 14,
	},
	.datac = 32,

};
FontHead SYSongTiM2424 ={

	.name = "SYST_24_m",//名字
	.size = 24,
	.path = "mtd0/1:font/syst_m_24.bin",//路径

	.type = FONT_DOT_WJQ,
	.st = FONT_ST_GB18030,
	.dt = FONT_H_H_L_R_U_D,
	.shift = 0,
	
	.bitmap={
		/* 以下五个数据在LCD描字时需要 */
		.rows = 24,
		.width = 24,
		.pitch = 3,
		.left = 0,
		.top = 20,
		},
	
	.datac = 72,

	
	};
/*------文泉驿点阵12pt------*/
FontHead WQYST16H18030 ={

	.name = "WQY_ST_16_H",//名字
	.size = 16,
	.path = "mtd0/1:font/wqy16h18030.bin",//路径
	
	.type = FONT_DOT_WJQ,
	.st = FONT_ST_GB18030,
	.dt = FONT_H_H_L_R_U_D,
	.shift = 0,
	
	.bitmap={
		/* 以下五个数据在LCD描字时需要 */
		.rows = 16,
		.width = 16,
		.pitch = 2,
		.left = 0,
		.top = 14,
	},
	
	.datac = 32,
};
	
FontHead WQYST12H18030 ={

	.name = "WQY_ST_12_H",//名字
	.size = 12,
	.path = "mtd0/1:font/wqy12h18030.bin",//路径
	
	.type = FONT_DOT_WJQ,
	.st = FONT_ST_GB18030,
	.dt = FONT_H_H_L_R_U_D,
	.shift = 0,
	
	.bitmap={
		/* 以下五个数据在LCD描字时需要 */
		.rows = 12,
		.width = 12,
		.pitch = 2,
		.left = 0,
		.top = 10,
		},
	
	.datac = 24,
};

FontHead *FontListN[FONT_LIST_MAX] = {
	&SYSongTi1212,
	&SYSongTi1616,	
	&SYSongTiM1616,
	&SYSongTiM2424,
	&WQYST16H18030,
	&WQYST12H18030,
	};


