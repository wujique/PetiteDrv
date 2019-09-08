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
#include "log.h"

#include "dev_spiflash.h"
#include "bus_lcd.h"
#include "bus_spi.h"
#include "bus_i2c.h"
#include "dev_lcd.h"


/*
	本文件用于配置系统有哪些设备
*/

/*
	矩阵按键硬件定义
	row输出，放前面
*/
KeyPadIO KeyPadIOList[KEY_PAD_ROW_NUM+KEY_PAD_COL_NUM]=
		{
			/*ROW*/
			{MCU_PORT_E, GPIO_Pin_0},
			{MCU_PORT_E, GPIO_Pin_1},
			{MCU_PORT_E, GPIO_Pin_2},
			{MCU_PORT_E, GPIO_Pin_3},
			/*COL*/
			{MCU_PORT_C, GPIO_Pin_3},
			{MCU_PORT_C, GPIO_Pin_2},
			{MCU_PORT_C, GPIO_Pin_1},
			{MCU_PORT_C, GPIO_Pin_0},
		};

/*---------------之下，是SPI I2C LCD 等抽象的比较好的设备和接口定义----------------*/

/*-------------------------------
	I2C控制器
-------------------------------*/
/*
	IO口模拟的I2C1
*/
const DevI2c DevVi2c1={
		.name = "VI2C1",
		.type = DEV_I2C_V,
		
		.sclport = MCU_PORT_B,
		.sclpin = GPIO_Pin_6,

		.sdaport = MCU_PORT_B,
		.sdapin = GPIO_Pin_7,
		};
		
const DevI2c DevVi2c2={
		.name = "VI2C2",
		.type = DEV_I2C_V,
		
		.sclport = MCU_PORT_B,
		.sclpin = GPIO_Pin_8,

		.sdaport = MCU_PORT_B,
		.sdapin = GPIO_Pin_9,
		};

/*
	硬件I2C控制器

	未测试，STM32的I2C还不会怎么用
*/
const DevI2c DevI2C1={
		.name = "I2C1",
		.type = DEV_I2C_H,
		
		.sclport = MCU_PORT_B,
		.sclpin = GPIO_Pin_6,

		.sdaport = MCU_PORT_B,
		.sdapin = GPIO_Pin_7,
		};	
/*----------------------
	IO口模拟SPI控制器
------------------------*/
/*
	VSPI1, 板上的LCD接口中的4根IO模拟SPI，
	用于XPT2046方案触摸处理，可读可写。
*/					
const DevSpi DevVSpi1IO={
		.name = "VSPI1",
		.type = DEV_SPI_V,
		
		/*clk*/
		.clkport = MCU_PORT_B,
		.clkpin = GPIO_Pin_0,
		/*mosi*/
		.mosiport = MCU_PORT_C,
		.mosipin = GPIO_Pin_5,
		/*miso*/
		.misoport = MCU_PORT_C,
		.misopin = GPIO_Pin_4,
	};

/* 
	外扩接口上的硬件SPI1，此处用IO模拟实现
*/
const DevSpi DevVSpi2IO={
		.name = "VSPI2",
		.type = DEV_SPI_V,
		
		/*clk*/
		.clkport = MCU_PORT_A,
		.clkpin = GPIO_Pin_5,
		/*mosi*/
		.mosiport = MCU_PORT_A,
		.mosipin = GPIO_Pin_7,
		/*miso*/
		.misoport = MCU_PORT_A,
		.misopin = GPIO_Pin_6,
	};


/*
	硬件SPI控制器：SPI3
	SPI驱动暂时支持SPI3，
	如果添加其他控制器，请修改mcu_spi.c中的硬件SPI控制器初始化代码
*/
const DevSpi DevSpi3IO={
		.name = "SPI3",
		.type = DEV_SPI_H,
		
		/*clk*/
		.clkport = MCU_PORT_B,
		.clkpin = GPIO_Pin_3,
		
		/*mosi*/
		.mosiport = MCU_PORT_B,
		.mosipin = GPIO_Pin_5,

		/*miso*/
		.misoport = MCU_PORT_B,
		.misopin = GPIO_Pin_4,
	};

/*------------------------ 
	SPI通道
-------------------------*/
/*
	板上FLASH用
*/
const DevSpiCh DevSpi3CH1={
		.name = "SPI3_CH1",
		.spi = "SPI3",
		
		.csport = MCU_PORT_C,
		.cspin = GPIO_Pin_7,
		
	};		
	

#if 0
/*
	模拟SPI通道，无CS
	用来调试没有CS的LCD屏幕，
	VSPI3，其实是外扩接口SPI附近的两个IO模拟的。
*/	
const DevSpiCh DevVSpi3CH1={
		.name = "VSPI3_CH1",
		.spi = "VSPI3",
		
		.csport = MCU_PORT_NULL,
		.cspin = NULL,
		
	};
#endif

/*
	模拟SPI通道
*/	
const DevSpiCh DevVSpi2CH1={
		.name = "VSPI2_CH1",
		.spi = "VSPI2",
		
		.csport = MCU_PORT_A,
		.cspin = GPIO_Pin_4,
	};


/* 
	LCD座子中的触摸屏接口, IO模拟SPI
*/
const DevSpiCh DevVSpi1CH1={
		.name = "VSPI1_CH1",
		.spi = "VSPI1",
		
		.csport = MCU_PORT_B,
		.cspin = GPIO_Pin_1,
		
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

/*-------------------------------------
	LCD硬件接口总线定义
	LCD硬件接口包含一个基本通信接口basebus、A0管脚，复位管脚，背光管脚。
---------------------------------------*/
#if 1
/*
	串行LCD接口，使用真正的SPI控制
	外扩接口中的SPI接口

	测试， 暂时用模拟SPI实现
*/
const DevLcdBus BusLcdSpi1={
	.name = "BusLcdSpi1",
	.type = LCD_BUS_SPI,
	.basebus = "VSPI2_CH1",

	.A0port = MCU_PORT_E,
	.A0pin = GPIO_Pin_4,

	.rstport = MCU_PORT_E,
	.rstpin = GPIO_Pin_5,

	.blport = MCU_PORT_A,
	.blpin = GPIO_Pin_9,

	.staport = MCU_PORT_NULL, 
	.stapin = NULL,
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
/*
	I2C接口的LCD总线
*/
const DevLcdBus BusLcdI2C1={
	.name = "BusLcdI2C1",
	.type = LCD_BUS_I2C,
	.basebus = "VI2C1",

	/*I2C接口的LCD总线，不需要其他IO*/
	.A0port = MCU_PORT_NULL,
	.A0pin = NULL,

	.rstport = MCU_PORT_NULL,
	.rstpin = NULL,
	
	.blport = MCU_PORT_NULL,
	.blpin = NULL,
};
/*
	8080接口的LCD总线
*/	
const DevLcdBus BusLcd8080={
	.name = "BusLcd8080",
	.type = LCD_BUS_8080,
	.basebus = "8080",//不使用用，8080操作直接嵌入在LCD BUS代码内

	/* 8080 不用A0脚，程序也不会操作AO */
	.A0port = MCU_PORT_NULL,
	.A0pin = NULL,

	.rstport = MCU_PORT_A,
	.rstpin = GPIO_Pin_15,
	
	.blport = MCU_PORT_B,
	.blpin = GPIO_Pin_15,

};


/*----------------------------
	FLASH设备定义
-----------------------------*/
const DevSpiFlash DevSpiFlashBoard={
	"core_spiflash",  
	"SPI3_CH1", 
	NULL
};

/*--------------------------------
	lcd设备树定义
	指明系统有多少个LCD设备，挂在哪个LCD总线上。
---------------------------------*/

/*I2C接口的 OLED*/

const DevLcd DevLcdOled1={
	.name = "i2coledlcd",  
	.buslcd = "BusLcdI2C1",  
	.id = 0X1315, 
	.width = 64, 
	.height = 128
	};

/*SPI接口的 OLED*/
/*
DevLcd DevLcdSpiOled	=	{
	.name = "spioledlcd", 	
	.buslcd = "BusLcdSpi3", 	
	.id = 0X1315, 
	.width = 64, 
	.height = 128};
*/
/*SPI接口的 COG LCD*/
const DevLcd DevLcdCOG1	=	{
	.name = "spicoglcd", 
	//.buslcd = "BusLcdVSpi2CH1", 
	.buslcd = "BusLcdSpi1",
	.id = 0X7565, 
	.width = 64, 
	.height = 128};

/*fsmc接口的 tft lcd*/
const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	NULL, 240, 320};
//const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	0x9325, 240, 320};
//const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	0x9341, 240, 320};
/*1408, 4.0寸的IPS屏幕*/
//const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	0x1408, 480, 800};
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

/*
	系统设备注册
	通过缩进区分层级和依赖关系。
	后续可以考虑实现可见字符表示的设备树
*/
s32 sys_dev_register(void)
{
	wjq_log(LOG_INFO, "[----------register---------------]\r\n");
	/*注册I2C总线*/
	mcu_i2c_register(&DevVi2c1);
			dev_lcdbus_register(&BusLcdI2C1);
					dev_lcd_register(&DevLcdOled1);
					
	//mcu_i2c_register(&DevVi2c2);
	
	mcu_spi_register(&DevSpi3IO);
			mcu_spich_register(&DevSpi3CH1);
					dev_spiflash_register(&DevSpiFlashBoard);

	mcu_spi_register(&DevVSpi2IO);
		mcu_spich_register(&DevVSpi2CH1);
			dev_lcdbus_register(&BusLcdSpi1);
					dev_lcd_register(&DevLcdCOG1);				
					
	mcu_spi_register(&DevVSpi1IO);
			mcu_spich_register(&DevVSpi1CH1);//8080接口的触摸屏
	
	dev_lcdbus_register(&BusLcd8080);
			dev_lcd_register(&DevLcdtTFT);
			
	wjq_log(LOG_INFO, "[---------- register finish ---------]\r\n");
	return 0;
}

/*

硬件参数配置表想法，类似LINUX的设备树
实际应用中，修改接口IO的可能性应该不大，
修改外设的可能性较大。
例如兼容多个LCD，降本的时候，会替代LCD，
很多LCD无法自动识别，所以，能用硬件配置文件最好。

格式：
[层级]类型:名称
	{
		参数
	}

例如：	
{
	[0]cpu:stm32f407
		{}
		[1]VI2C:VI2C1
			{	
				
			}
			[2]LCDBUS:BusLcdI2C1
				{
					
				}
				[3]LCD:i2coledlcd
					{
						
					}

}


*/

/*

	汉字字库只使用多国文字点阵字库生成器生成的点阵字库，
	使用模式二生成，也就是纵库。

*/
struct fbcon_font_desc font_songti_16x16 = {
	"songti16",
	"mtd1/songti1616.DZK",
	16,
	16,
	32
};
	
struct fbcon_font_desc font_songti_12x12 = {
	"songti12",
	"mtd1/songti1212.DZK",
	12,
	12,
	24
};

struct fbcon_font_desc font_siyuan_16x16 = {
	"siyuan16",
	"mtd1/shscn1616.DZK",
	16,
	16,
	32
};
	
struct fbcon_font_desc font_siyuan_12x12 = {
	"siyuan12",
	"mtd1/shscn1212.DZK",
	12,
	12,
	24
};

struct fbcon_font_desc *FontList[FONT_LIST_MAX]=
	{
		&font_songti_16x16,
		&font_songti_12x12,
		&font_siyuan_16x16,
		&font_siyuan_12x12
	};



