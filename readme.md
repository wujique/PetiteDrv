## PetiteDrv
一个基于freertos的软件框架，实现了一个设备静态挂载式的Petite设备管理。

例如，定义一条SPI总线：

```
const DevSpi DevSpi3IO={
		.pnode={
				.name = "SPI3",
				.type = BUS_SPI_H,
		},
		。。。
	};
```

基于这条SPI总线定义SPI通道：

```
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
```

然后将LCD或者SPI FLASH挂载在这些SPI通道上。

**实现一个文件配置系统各设备链接情况，请参考board_sysconf.c**

定义了标准的LCD驱动接口，适配了各种LCD驱动，按照此接口定义，非常容易添加新的LCD。

同时提供点阵字库解析功能。

**相当于完成了一个项目50%的开发**

## Directory Structure
|Dir|Decription|
|-|-|
|board|各种板的应用和测试程序，相当于应用层|
|driver|设备驱动：摄像头、LCD、语音codec、触摸屏、矩阵按键蜂鸣器等。|
|mcu| 芯片的HAL库                                                  |
|panel|中间件：wifi管理、显示、菜单界面、语音播放|
|Petite|Petite核心、总线管理（I2C&SPI等），内存管理、点阵字体模块|
|Utilities|Third party library: freertos,fatfs,lvgl,littlefs,zbar,zint|

## Petite

#### bus

i2c bus、 spi bus、vi2c bus、vspi bus

可以灵活将各种外设挂载在这些总线上，实现驱动和硬件分离。

#### font

点阵字体管理，支持多种格式点阵，能实现多语言，支持UTF8-T显示

#### mem

内存分配管理，kfifo管理、链表管理

## Main feature
Please find the detail from :board_sysconf.c/board_sysconf.h, and Petite.
## Contact us
Website:
<www.wujique.com>



