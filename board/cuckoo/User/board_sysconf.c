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
	需要注意，特别是外接OLED等外设 */
const DevI2c DevVi2c1={
	.pdev={
			.name = "VI2C1",
			.type = BUS_I2C_V,

			.basebus = NULL,
			.busconf = NULL,
			.basetype = DEV_NULL,
		},

	.sclport = MCU_PORT_C,
	.sclpin = MCU_IO_4,

	.sdaport = MCU_PORT_A,
	.sdapin = MCU_IO_5,
};

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
	
};
/*	外扩接口的SPI，可接COG、OLED、SPI TFT、RF24L01*/		
const DevSpiCh DevSpi3CH3={	
	.pdev={
		.name = "SPI3_CH3",
		.type = BUS_SPI_CH,

		.basebus = "SPI3",
		.busconf = NULL,
		.basetype = BUS_SPI_H,
	},
	
	.csport = MCU_PORT_A,
	.cspin = MCU_IO_15,
};
		
const DevSpiCh DevSpi3CH4={	
	.pdev={
		.name = "SPI3_CH4",
		.type = BUS_SPI_CH,

		.basebus = "SPI3",
		.busconf = NULL,
		.basetype = BUS_SPI_H,
	},	
	
	.csport = MCU_PORT_E,
	.cspin = MCU_IO_14,		
};

/*-----------------------------------------------------------*/
/* 	I2C接口的LCD总线 */
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
		
/* I2C接口的 OLED */
const I2CPra OledLcdI2cPra ={
	.addr = 0x3D, /* 0X3C OR 0X3D */
	.fkhz = 200,/* 时钟频率，单位KHz */
};

const DevLcd DevLcdOled1 =	{
	.pdev = {
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


/*SPI接口的 COG LCD*/
const PraSpiSet CogSpiSet = {
	.mode = SPI_MODE_3,
	.KHz = 10000,
};

const DevLcdCtrlIO BusLcdSpi3={
	.A0port = MCU_PORT_B,
	.A0pin = MCU_IO_13,

	.rstport = MCU_PORT_B,
	.rstpin = MCU_IO_12,

	.blport = MCU_PORT_B,
	.blpin = MCU_IO_11,

	.staport = MCU_PORT_E, 
	.stapin = MCU_IO_14,
};

const DevLcd DevLcdCOG1 =	{
	.pdev = {
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

/* spi 接口 黑白墨水屏 1.54寸 GDEH154D27 */
const DevLcd DevLcdEpaper	=	{
	.pdev = {
		.name = "spiE-Paper",
		.type = DEV_LCD,
		
		.basebus = "SPI3_CH3",
		.busconf = (void *)&CogSpiSet,
		.basetype = BUS_SPI_CH,
	},
	
	.ctrlio = &BusLcdSpi3,
	.id = 0x3820, 
	.width = 200, 
	.height = 200,
};
		
const DevSpiFlash DevSpiFlashEx={
	.pdev = {
		.name = "ex_spiflash",
		.type = DEV_SPIFLASH,
		
		.basebus = "SPI3_CH4",
		.busconf = (void *)&CogSpiSet,
		.basetype = BUS_SPI_H,
	},

	.pra = NULL,
};


void *PetiteDevTable[]={
	/*注册I2C总线，将oled1挂载此 I2C总线上 */
	(void *)&DevVi2c1,
		(void *)&DevLcdOled1,
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
	 先定义sto设备，在定义par分区
	*/ 
const PartitionDef PetitePartitonTable[] =
{
	#if 0
	/* 7b0 芯片内部Flash，128K */
	{"sto","onchip","7b0_flash",      0x8000000,  0x20000},
		{"par","app","boot",    	0x8000000,  0x10000},
		{"par","flashdb","boot",    0x8010000,  0x10000},

	/*主板上的qspi flash */
	{"sto","xip_flash","xip_qflash", 0x90000000, 0x20000},
		{"par","app","core",    	0x90000000, 0x20000},
		//{"par","par","def",    		0x90000000, 0x20000},
		{"par","app","app",     	0x90020000, 0x20000},
	#endif
	
	/* 外扩SPI 接口的 flash, sto 空间大小通过设备获取 */
	{"sto","spiflash","ex_spiflash", 0x50000000, 0},
		{"par","FlashDB","flashdb",     0x50000000, 0x20000},//128K的kv
		{"par", VFS_STR_LITTLEFS, "mtd0",     0x50000000+0x100000, 0x400000},//4M 文件系统，sqlite基于文件系统
		
	/* 把sd卡也放到partiton，以便将其初始化为fatfs后挂载到vfs中 
		目前sdmmc并不归 partition管理。
		addr 和 size， 和实际不符，32位寻址最大才4G，sd卡早超出了 */
	{"sto", "sdmmc",  "socket1", 0x60000000, 0},
		{"par",VFS_STR_FATFS, SDPath,     0x60000000, 0},
	/*--------------------------------------------------*/
	{NULL,NULL,NULL,0,0},
};
#endif


