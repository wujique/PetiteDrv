#ifndef __BUS_SPI_H_
#define __BUS_SPI_H_

#include "mem/p_list.h"
#include "petite_def.h"
#include "petite_dev.h"

#include "cmsis_os.h"

/*
	SPI 分两层，
	1层是SPI控制器，不包含CS
	2层是SPI通道，由控制器+CS组成
*/
/*	SPI 设备定义 */
typedef struct
{
	PetiteDev pdev;
	
	MCU_PORT clkport;
	u16 clkpin;

	MCU_PORT mosiport;
	u16 mosipin;

	MCU_PORT misoport;
	u16 misopin;

}DevSpi;

/*	SPI控制器设备节点 */
typedef struct
{
	PDevNode pnode;
	
	osMutexId_t mutex;
	
	/*模拟SPI的时钟分频设置*/
	u16 clk;
	SPI_MODE mode;

}DevSpiNode;

/*	SPI 通道定义
	一个SPI通道，有一个SPI控制器+一根CS引脚组成*/
typedef struct
{
	PetiteDev pdev;
	
	/*cs脚*/
	MCU_PORT csport;
	u16 cspin;
}DevSpiCh;

/*SPI通道节点*/
typedef struct
{
	PDevNode pnode;
	
	s32 gd;

}DevSpiChNode;

/*	SPI 参数，在打开spi时配置到spi
	*/
typedef struct{
	SPI_MODE mode;
	u16 KHz;
}PraSpiSet;

extern PDevNode * bus_spi_register(const DevSpi *dev);
extern PDevNode * bus_spich_register(const DevSpiCh *dev);

extern DevSpiChNode *bus_spich_open(char *name, SPI_MODE mode, u16 KHz,  uint32_t wait);
extern DevSpiChNode *bus_spich_opennode(DevSpiChNode *node, SPI_MODE mode, u16 KHz,  uint32_t wait);

extern s32 bus_spich_close(DevSpiChNode * node);
extern s32 bus_spich_transfer(DevSpiChNode * node, u8 *snd, u8 *rsv, s32 len);
extern s32 bus_spich_cs(DevSpiChNode * node, u8 sta);


#endif

