#ifndef _PETITE_DEV_H_
#define _PETITE_DEV_H_

#include "mem/p_list.h"
#include "petite_config.h"

typedef enum{
	DEV_NULL = 0,

	BUS_I2C_H = 0x11,//硬件SPI控制器
	BUS_I2C_V = 0x12,//IO模拟SPI

	BUS_SPI_H = 0x21,//硬件SPI控制器
	BUS_SPI_V = 0x22,//IO模拟SPI
	BUS_SPI_CH = 0x23,//spi 通道

	BUS_8080	= 0X24,
	
	DEV_SPIFLASH = 0x31,
	DEV_LCD = 0x32,
}PetiteDevType;


/* petite dev define 
	本结构体用来定义设备树上的设备，
	里面的所有参数都是静态变量,
	也就是用const定义的 ，不使用ram空间 */
typedef struct {	
	const char name[DEV_NAME_SIZE];
	PetiteDevType type;
	
	char *basebus;//总线名字
	void *busconf;//bus配置信息，如果是spi ch，就是模式 频率等信息, I2c
	PetiteDevType basetype;//依赖的总线类型，有此参数，可避免驱动多次查询。
	
}PetiteDev;

/* 设备节点 
	用于管理 */
typedef struct _strPDevNode{	
	/* 所有节点连成一个链表 */
	struct list_head list;
	PetiteDev *pdev;

	struct _strPDevNode *basenode;//依赖节点
}PDevNode;


extern PDevNode *petite_dev_get_node(char *name);
extern s32 petite_dev_register(void *DevTable[]);


#endif

