#ifndef __PETITE_DEF__
#define __PETITE_DEF__


/* 配置PetiteDrv */

/* 设备名长度 */
#define DEV_NAME_SIZE	16

/* 	用宏控制哪些LCD驱动参加编译*/
#define TFT_LCD_DRIVER_COG12864
#define TFT_LCD_DRIVER_SSD1615
#define TFT_LCD_DRIVER_9341
#define TFT_LCD_DRIVER_9341_8BIT
#define TFT_LCD_DRIVER_9325
#define TFT_LCD_DRIVER_7735
#define TFT_LCD_DRIVER_7789
#define TFT_LCD_DRIVER_91874
#define TFT_LCD_DRIVER_3820

/*设备类型定义*/
typedef enum{
	DEV_NULL = 0,
	BUS_LCD_SPI,
	BUS_LCD_I2C,
	BUS_LCD_8080,
	BUS_LCD_MAX,

	BUS_I2C_H = 0x11,//硬件SPI控制器
	BUS_I2C_V = 0x12,//IO模拟SPI

	BUS_SPI_H = 0x21,//硬件SPI控制器
	BUS_SPI_V = 0x22,//IO模拟SPI
	BUS_SPI_CH = 0x23,//spi 通道

	DEV_SPIFLASH = 0x31,
	DEV_LCD = 0x32,
}PetiteDevType;

typedef struct
{
	const char name[DEV_NAME_SIZE];
	PetiteDevType type;
}PetiteNode;


#endif
