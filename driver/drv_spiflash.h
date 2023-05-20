#ifndef __DEV_SPIFLASH_H_
#define __DEV_SPIFLASH_H_

#include "petite_def.h"
#include "petite_dev.h"
#include "mem/p_list.h"

#include "bus/bus_spi.h"

/*SPI FLASH 设备规格*/

typedef struct{
	char *name;
	u32 JID;
	u32 MID;
	/*容量，块数，块大小等信息*/
	u32 sectornum;//总块数
	u32 sectorsize;//块大小
	u32 structure;//总容量
}NorFlashPra;

/* 设备定义 */
typedef struct
{
	PetiteDev pdev;

	const NorFlashPra *pra;
}DevSpiFlash;

/* 设备节点 
   此处定义的是驱动需要的全局变量 
   是程序执行需要的参数 不同的设备需要不同的变量 
   本结构体在注册设备是创建，挂载到petite dev链表中 */
typedef struct
{
	PDevNode pnode;
	/**/
	s32 gd;	
	DevSpiChNode *spichnode;
	const NorFlashPra *pra;///如有需要，根据dev中的pra和实际情况，创建新的设备规格
	
}DevSpiFlashNode;

extern PDevNode *dev_spiflash_register(const DevSpiFlash *dev);
extern s32 dev_spiflash_sector_erase(DevSpiFlashNode *node, u32 sector);
extern s32 dev_spiflash_sector_read(DevSpiFlashNode *node, u32 sector, u8 *dst);	
extern s32 dev_spiflash_sector_write(DevSpiFlashNode *node, u32 sector, u8 *src);
extern void *dev_spiflash_open(char* name);
extern s32 dev_spiflash_close(DevSpiFlashNode *node);
extern s32 dev_spiflash_test(void);

#endif

