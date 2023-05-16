#ifndef _PARTITION_H_
#define _PARTITION_H_

#include "mcu.h"


/*-----------------------------------------*/
/**
 * @brief   分区定义
 * 
 * 
 */
typedef struct _PartitionDef {
	char *maptype;/// Storage层级，"sto"，存储空间；"par"，分区空间。
	char *type;///类型，当maptype=par，type指出分区类型；当maptype=sto时，type名称用来关联驱动
	char *name;///名称，maptype=sto时，本名称为设备名称，用于和设备驱动匹配。
			  ///当maptype=par时，则为分区名称，上层（APP、fs）可通过本名称对分区进行操作
	uint32_t addr;//分区起始地址
	uint32_t size;///为0，则说明需要通过侦测获得，或者，是其他情况，例如sd卡槽，SD卡容量是不固定的，有时还需要热插拔
}PartitionDef;

/**
 * @brief   存储设备定义
 * @details 底层设备提供本结构体内容给partition使用
 * 
 */
typedef struct _StorageDev{
	int (*getblksize)(void *dev);
	int (*getsize)(void *dev);
	
	void *(*open)(const char *name);	
	int (*read)(void *dev, uint32_t offset, uint8_t *buf, size_t size);
	int (*write)(void *dev, uint32_t offset, const uint8_t *buf, size_t size);
	int (*erase)(void *dev, uint32_t offset, size_t size);
	int (*close)(void *dev);
}StorageDev;

/**
 * @brief   分区节点定义
 * @details storage是一种特殊partition
 * 
 */
typedef struct _PartitionNode {
	PartitionDef *def;///指向对应分区定义

	union{
		struct _PartitionNode *sto;///分区指向storage
		const StorageDev *dev;///storage指向设备 操作结构体
	}base;
	/* 不同的partiton，不同的格式，初始化后需要的定义 
		比如初始化为littlefs后，需要一些参数变量，可以挂在这个指针上 */
	void *context;

	struct _PartitionNode *next;///所有partition node组成单向链表
}PartitionNode;


extern void *petite_partition_get(const char *parname);
extern uint32_t petite_partition_getlen(void *part);
extern uint32_t petite_partition_getblksize(void *part);
extern int petite_partition_read(void *part, uint32_t addr, uint8_t *buf, size_t size);
extern int petite_partition_write(void *part, uint32_t addr, const uint8_t *buf, size_t size);
extern int petite_partition_erase(void *part, uint32_t addr, size_t size);
extern int petite_partition_init();


#endif

