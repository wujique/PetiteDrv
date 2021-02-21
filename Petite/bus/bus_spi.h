#ifndef __BUS_SPI_H_
#define __BUS_SPI_H_

#include "mem/p_list.h"
#include "petite_def.h"

/*
	SPI 分两层，
	1层是SPI控制器，不包含CS
	2层是SPI通道，由控制器+CS组成
*/
/*	SPI 设备定义 */
typedef struct
{
	PetiteNode pnode;
	
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
	/*句柄，空闲为-1，打开为0，spi控制器不能重复打开*/
	s32 gd;
	/*控制器硬件信息，初始化控制器时拷贝设备树的信息到此*/
	DevSpi dev;	
	
	/*模拟SPI的时钟分频设置*/
	u16 clk;
	/*链表*/
	struct list_head list;
}DevSpiNode;

/*	SPI 通道定义
	一个SPI通道，有一个SPI控制器+一根CS引脚组成*/
typedef struct
{
	PetiteNode pnode;
	
	/*SPI控制器名称*/
	char spi[DEV_NAME_SIZE];

	/*cs脚*/
	MCU_PORT csport;
	u16 cspin;
}DevSpiCh;

/*SPI通道节点*/
typedef struct
{
	s32 gd;
	
	DevSpiCh dev;
	
	DevSpiNode *spi;//控制器节点指针
	
	struct list_head list;
}DevSpiChNode;

extern s32 bus_spi_register(const DevSpi *dev);
extern s32 bus_spich_register(const DevSpiCh *dev);

extern DevSpiChNode *bus_spich_open(char *name, SPI_MODE mode, u16 KHz);
extern s32 bus_spich_close(DevSpiChNode * node);
extern s32 bus_spich_transfer(DevSpiChNode * node, u8 *snd, u8 *rsv, s32 len);
extern s32 bus_spich_cs(DevSpiChNode * node, u8 sta);


#endif

