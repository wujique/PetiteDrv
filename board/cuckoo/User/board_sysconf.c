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
#include "drv_config.h"

#include "drv_spiflash.h"


/* cuckoo 小板， SAI接口和外扩I2C接口的其他信号共用，
	需要注意，特别是外界OLED等外设*/
const DevI2c DevVi2c1={
		.pnode={
				.name = "VI2C1",
				.type = BUS_I2C_V,
				},
		
		.sclport = MCU_PORT_C,
		.sclpin = MCU_IO_4,

		.sdaport = MCU_PORT_A,
		.sdapin = MCU_IO_5,
		};
/*
	I2C接口的LCD总线 */
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
				
/*I2C接口的 OLED*/
const I2CPra OledLcdI2cPra ={
	.addr = 0x3D, /* 0X3C OR 0X3D */
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

		/* 硬件SPI 直接放到 mcu中初始化 根据name来判断初始化 写死*/		
		#if 0
		/*clk*/
		.clkport = MCU_PORT_B,
		.clkpin = MCU_IO_3,
		
		/*mosi*/
		.mosiport = MCU_PORT_B,
		.mosipin = MCU_IO_5,

		/*miso*/
		.misoport = MCU_PORT_B,
		.misopin = MCU_IO_4,
		#endif
	};
/*	外扩接口的SPI，可接COG、OLED、SPI TFT、RF24L01*/			
const DevSpiCh DevSpi3CH3={
		.pnode={
				.name = "SPI3_CH3",
				.type = BUS_SPI_CH,
			},
			
		.spi = "SPI3",
		
		.csport = MCU_PORT_A,
		.cspin = MCU_IO_15,
		
	};
const DevSpiCh DevSpi3CH4={
		.pnode={
				.name = "SPI3_CH4",
				.type = BUS_SPI_CH,
			},
			
		.spi = "SPI3",
		
		.csport = MCU_PORT_E,
		.cspin = MCU_IO_14,
		
	};

/*	串行LCD接口，使用真正的SPI控制
	外扩接口中的SPI接口*/
const DevLcdBus BusLcdSpi3={
	.pnode={
				.name = "BusLcdSpi3",
				.type = BUS_LCD_SPI,
			},
	
	.basebus = "SPI3_CH3",

	.A0port = MCU_PORT_B,
	.A0pin = MCU_IO_13,

	.rstport = MCU_PORT_B,
	.rstpin = MCU_IO_12,

	.blport = MCU_PORT_B,
	.blpin = MCU_IO_11,

	.staport = MCU_PORT_E, 
	.stapin = MCU_IO_14,
};

/*SPI接口的 COG LCD*/
const PraSpiSet CogSpiSet = {
	.mode = SPI_MODE_3,
	.KHz = 10000,
};

const DevLcd DevLcdCOG1	=	{

	.pnode={
				.name = "spicoglcd",
				.type = DEV_LCD,
		},
	
	.id = 0X7565, 
	.width = 64, 
	.height = 128,

	.bus = (DevLcdBus *)&BusLcdSpi3,
	.buspra = (void *)&CogSpiSet,
};
				
/* spi 接口 黑白墨水屏 1.54寸 GDEH154D27*/
const DevLcd DevLcdSPIEPaper =	{
	.pnode={
				.name = "spiE-Paper",
				.type = DEV_LCD,
		},
		 
	.id = 0x3820, 
	.width = 200, 
	.height = 200,
	
	.bus = &BusLcdSpi3,
	.buspra = (void *)&CogSpiSet,
};
				
const DevSpiFlash DevSpiFlashEx={
	.pnode={
				.name = "ex_spiflash",
				.type = DEV_SPIFLASH,
			},
	
	.spich ="SPI3_CH4", 
	.pra = NULL,
};


void *PetiteDevTable[]={
	/*注册I2C总线，将oled1挂载此 I2C总线上 */
	(void *)&DevVi2c1,
	//	(void *)&DevLcdOled1,
	/*硬SPI3控制器，外扩接口的SPI口，并定义一个BusLcd在此总线上 
		可将一些LCD设备挂在本总线上 */
	(void *)&DevSpi3IO,
		(void *)&DevSpi3CH3,
			(void *)&DevLcdCOG1,
		(void *)&DevSpi3CH4,
			(void *)&DevSpiFlashEx,
	/* dont move*/
	NULL,
	};


/*	字库定义		*/
/*
	屋脊雀优化的点阵，基于思源宋体
	*/	
const FontHead SYSongTiM1616 ={

	.name = "SYST_16_m",//名字
	.size = 16,
	.baseline = 14,
	.path = "mtd0/0:font/syst_m_1616_18030.bin",//路径
	
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
const FontHead SYSongTiM2424 ={

	.name = "SYST_24_m",//名字
	.size = 24,
	.baseline = 20,
	.path = "mtd0/0:font/syst_m_2424_gb18030.bin",//路径

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
const FontHead WQYST16H18030 ={

	.name = "WQY_ST_16_H",//名字
	.size = 16,
	.baseline = 14,
	.path = "mtd0/0:font/wqy16h18030.bin",//路径
	
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
	
const FontHead WQYST12H18030 ={

	.name = "WQY_ST_12_H",//名字
	.size = 12,
	.baseline = 10,
	.path = "mtd0/0:font/wqy12h18030.bin",//路径
	
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

const FontHead *FontListN[FONT_LIST_MAX] = {
	&SYSongTiM1616,
	&SYSongTiM2424,
	&WQYST16H18030,
	&WQYST12H18030,
	};


