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


/* cuckoo 小板， SAI接口和外扩I2C接口的其他信号共用，
	需要注意，特别是外界OLED等外设时。*/
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
	I2C接口的LCD总线
*/
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
#if 0
const I2CPra OledLcdI2cPra ={
	.addr = 0x3c, /* i2c 设备地址，7bit模式 */
	.fkhz = 200,/* 时钟频率，单位KHz */
};
#endif
const DevLcd DevLcdOled1={
	.pnode={
				.name = "i2coledlcd",
				.type = DEV_LCD,
		},
		
	.id = 0X1315, 
	.width = 64, 
	.height = 128,

	.buslcd = "BusLcdI2C1",  
	//.buspra = &OledLcdI2cPra,
};

/*
	VSPI1，核心板上的LCD接口中的4根IO模拟SPI，
	用于XPT2046方案触摸处理，可读可写。
*/					
const DevSpi DevVSpi1IO={
		.pnode={
				.name = "VSPI1",
				.type = BUS_SPI_V,
			},
		/*clk*/
		.clkport = MCU_PORT_C,
		.clkpin = MCU_IO_4,
		/*mosi*/
		.mosiport = MCU_PORT_A,
		.mosipin = MCU_IO_5,
		/*miso*/
		.misoport = MCU_PORT_D,
		.misopin = MCU_IO_9,
	};
/* 
	LCD座子中的触摸屏接口, IO模拟SPI
*/
const DevSpiCh DevVSpi1CH1={
		.pnode={
				.name = "VSPI1_CH1",
				.type = BUS_SPI_CH,
			},
		
		.spi = "VSPI1",
		
		.csport = MCU_PORT_D,
		.cspin = MCU_IO_14,
		
	};


#if 0
const Devtp BusLcdI2C1={
	.pnode={
				.name = "CTP_GT1151",
				.type = BUS_LCD_I2C,
			},
			
	.basebus = "VI2C1",

	/* 触摸屏 */
	.A0port = MCU_PORT_NULL,
	.A0pin = NULL,

	.rstport = MCU_PORT_NULL,
	.rstpin = NULL,

	.blport = MCU_PORT_NULL,
	.blpin = NULL,

	.staport = MCU_PORT_NULL, 
	.stapin = NULL,
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
/*
	外扩接口的SPI，可接COG、OLED、SPI TFT、RF24L01
*/			
const DevSpiCh DevSpi3CH3={
		.pnode={
				.name = "SPI3_CH3",
				.type = BUS_SPI_CH,
			},
			
		.spi = "SPI3",
		
		.csport = MCU_PORT_A,
		.cspin = MCU_IO_15,
		
	};

#if 1
/*
	串行LCD接口，使用真正的SPI控制
	外扩接口中的SPI接口
*/
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

/*SPI接口的 COG LCD*/

const DevLcd DevLcdCOG1	=	{

	.pnode={
				.name = "spicoglcd",
				.type = DEV_LCD,
		},
		
	//.buslcd = "BusLcdVSpi2CH1", 
	.buslcd = "BusLcdSpi3",
	.id = 0X7565, 
	.width = 64, 
	.height = 128};
				
/* spi 接口的三色墨水屏 */
//const DevLcd DevLcdSPIEPaper =	{"spiE-Paper",		"BusLcdSpi3",	0x9187, 176, 264};
/* spi 接口 黑白墨水屏 1.54寸 GDEH154D27*/
const DevLcd DevLcdSPIEPaper =	{

	.pnode={
				.name = "spiE-Paper",
				.type = DEV_LCD,
		},
		 
	.buslcd = "BusLcdSpi3",
	.id = 0x3820, 
	.width = 200, 
	.height = 200};


/*
	系统设备注册
	通过缩进区分层级和依赖关系。
	后续可以考虑实现可见字符表示的设备树
*/
s32 petite_dev_register(void)
{
	wjq_log(LOG_DEBUG, "[register] petite_dev_register!\r\n");

	/*注册I2C总线*/
	bus_i2c_register(&DevVi2c1);
			//dev_lcdbus_register(&BusLcdI2C1);
					//dev_lcd_register(&DevLcdOled1);

				
	/*硬SPI3控制器，核心板和底板的FLASH、外扩接口的SPI口*/
	bus_spi_register(&DevSpi3IO);			
			bus_spich_register(&DevSpi3CH3);
					dev_lcdbus_register(&BusLcdSpi3);
						//dev_lcd_register(&DevLcdSPIEPaper);
						//dev_lcd_register(&DevLcdCOG1);
						//dev_lcd_register(&DevLcdSpiOled);

			//mcu_spich_register(&DevSpi3CH4);
			
	bus_spi_register(&DevVSpi1IO);			
			bus_spich_register(&DevVSpi1CH1);
	/*测试1.33寸IPS屏幕采用*/
	#if 0
	mcu_spi_register(&DevVspi3IO);
			mcu_spich_register(&DevVSpi3CH1);
					dev_lcdbus_register(&BusLcdVSpi3);
							dev_lcd_register(&DevLcdVSPITFT);
	#endif


	return 0;
}

/*	字库定义		*/
/*
	屋脊雀优化的点阵，基于思源宋体
	*/	
const FontHead SYSongTiM1616 ={

	.name = "SYST_16_m",//名字
	.size = 16,
	.path = "mtd0/0:font/syst_m_16.bin",//路径
	
	.type = FONT_DOT_WJQ,
	.st = FONT_ST_GB18030,
	.dt = FONT_H_H_L_R_U_D,
	.shift = 0,
	
	.datac = 32,
	.w = 16,
	.h = 16,
	
	};
const FontHead SYSongTiM2424 ={

	.name = "SYST_24_m",//名字
	.size = 24,
	.path = "mtd0/0:font/syst_m_24.bin",//路径

	.type = FONT_DOT_WJQ,
	.st = FONT_ST_GB18030,
	.dt = FONT_H_H_L_R_U_D,
	.shift = 0,
	
	.datac = 72,
	.w = 24,
	.h = 24,
	
	};
/*------文泉驿点阵12pt------*/
const FontHead WQYST16H18030 ={

	.name = "WQY_ST_16_H",//名字
	.size = 16,
	.path = "mtd0/0:font/wqy16h18030.bin",//路径
	
	.type = FONT_DOT_WJQ,
	.st = FONT_ST_GB18030,
	.dt = FONT_H_H_L_R_U_D,
	.shift = 0,
	
	.datac = 32,
	.w = 16,
	.h = 16,
	};
	
const FontHead WQYST12H18030 ={

	.name = "WQY_ST_12_H",//名字
	.size = 12,
	.path = "mtd0/0:font/wqy12h18030.bin",//路径
	
	.type = FONT_DOT_WJQ,
	.st = FONT_ST_GB18030,
	.dt = FONT_H_H_L_R_U_D,
	.shift = 0,
	
	.datac = 24,
	.w = 12,
	.h = 12,
	};

const FontHead *FontListN[FONT_LIST_MAX] = {
	&SYSongTiM1616,
	&SYSongTiM2424,
	&WQYST16H18030,
	&WQYST12H18030,
	};


