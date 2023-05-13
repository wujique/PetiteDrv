/**
 * @file    partition.c
 * @brief   存储分区管理层
 * @details 将存储设备封装成统一的分区操作接口，以便上层组件（vfs，littlefs，flashdb）
 *          不需要关心设备信息。
 * @author  wujique(xxx@wujique.com)
 * @version 0.1
 * @date    2023-05-13
 * @copyright Copyright (c) 2023..
 */
#include "mcu.h"
#include "log.h"
#include "partition.h"
#include "mem/p_malloc.h"


extern int vfs_mount(void *par);
/*
	地址空间说明

	1. storage地址空间：
	     有三种
		内部空间：例如STM32内部Flash，固定分配好了，例如0x800 0000
	    XIP空间：例如STM32H7B的QSPI Flash，固定分配在0x90000 0000
	    用户分配：例如一片SPI FLASH，其实没有分配地址，
	   			可以选择一个芯片保留空间分配给这片Flash。

	2. partition空间：
		在定义的时候，起始地址用的是storage地址空间。
		用户使用时，是从0开始的地址空间。
		操作设备时，则需要+上partition地址后减去storage地址。

	*/

PartitionNode *PartitionRoot = NULL;

/**
 * @brief   根据分区名获取分区节点指针
 * 
 * @param   parname     分取名
 * @return  void*       分区指针，实际是PartitionNode类型
 * 
 */
void *petite_partition_get(const char *parname)
{
	PartitionNode *par;

	wjq_log(LOG_WAR, "part find: %s\r\n", parname);
	
	par = PartitionRoot;

	while(1) {
		if (par == NULL) break;

		if (strcmp(parname, par->def->name) == 0) {
			return par;
		}
		par = par->next;
	}

	wjq_log(LOG_WAR, "part no foud: %s\r\n", parname);
	return NULL;
}
/**
 * @brief   获取分区长度
 * 
 * @param   part        分区指针
 * @return  uint32_t    返回分区长度
 * 
 */
uint32_t petite_partition_getlen(void *part)
{
	PartitionNode *par;

	par = (PartitionNode *)part;

	wjq_log(LOG_INFO, "part size: 0x%x\r\n", par->def->size);
	return par->def->size;
}
/**
 * @brief   获取分区所在设备的块大小
 * 
 * @param   part        分区指针
 * @return  uint32_t    block大小，SPI Flash一般都是4K
 * 
 */
uint32_t petite_partition_getblksize(void *part)
{
	PartitionNode *par;
	PartitionNode *sto;
	
	par = (PartitionNode *)part;
	sto  = par->base.sto;

	wjq_log(LOG_INFO, "part getblksize: 0x%x\r\n", sto->base.dev->blksize);
	return sto->base.dev->blksize;
}

/**
 * @brief   读分区数据
 * 
 * @param   part        分区指针
 * @param   addr        读偏移（分区开始地址为0）
 * @param   dst         数据存储地址
 * @param   size        读长度
 * @return  int 
 * 
 */
int petite_partition_read(void *part, uint32_t addr, uint8_t *dst , size_t size)
{
	PartitionNode *par;
	PartitionNode *sto;
	void *dev;
	int res;
	uint32_t dev_addr;
	
	par = (PartitionNode *)part;
	sto  = par->base.sto;

	dev_addr = addr + par->def->addr - sto->def->addr;
	//wjq_log(LOG_INFO, "part read: %s 0x%x(0x%x), 0x%x\r\n", 
	//					sto->def->name, dev_addr, addr, size);
	
	dev = sto->base.dev->open(sto->def->name);
	if(dev == NULL) return -1;
	res = sto->base.dev->read(dev, dev_addr, dst, size);
	
	sto->base.dev->close(dev);

	return res;
}
/**
 * @brief   写数据到指定分区
 * @details 写数据，也就是prog，如有需要，请先erase
 * 
 * @param   part        分区指针
 * @param   addr        写地址偏移（分区开始地址为0）
 * @param   src         源数据地址
 * @param   size        数据长度
 * @return  int 
 * 
 */
int petite_partition_write(void *part, uint32_t addr, const uint8_t *src, size_t size)
{
	PartitionNode *par;
	PartitionNode *sto;
	void *dev;
	int res;
	uint32_t dev_addr;
	
	par = (PartitionNode *)part;
	sto  = par->base.sto;

	dev_addr = addr + par->def->addr - sto->def->addr;
	//wjq_log(LOG_INFO, "part write: %s 0x%x(0x%x), 0x%x\r\n", 
	//					sto->def->name, dev_addr, addr, size);
	dev = sto->base.dev->open(sto->def->name);
	if(dev == NULL) return -1;
	res = sto->base.dev->write(dev, dev_addr, src, size);
	
	sto->base.dev->close(dev);

	return res;
}

/**
 * @brief   擦除分区部分数据
 * @details 参数有size，但是并不能擦除任意长度。
 *          本函数按照block擦除，因此，实际擦除长度可能超出size
 *          建议上层按照block的倍数进行擦除。
 *          同时，addr在最好也按照block对齐
 * 
 * @param   part        分区指针
 * @param   addr        擦除起始地址
 * @param   size        参数描述
 * @return  int 
 * 
 */
int petite_partition_erase(void *part, uint32_t addr, size_t size)
{
	PartitionNode *par;
	PartitionNode *sto;
	void *dev;
	uint32_t dev_addr;
	//wjq_log(LOG_INFO, "part erase: 0x%x, 0x%x\r\n", addr, size);

	par = (PartitionNode *)part;
	sto  = par->base.sto;

	dev_addr = addr + par->def->addr - sto->def->addr;
	
	//wjq_log(LOG_INFO, "part erase: %s 0x%x(0x%x), 0x%x\r\n", 
	//					sto->def->name, dev_addr, addr, size);
	
	dev = sto->base.dev->open(sto->def->name);
	if(dev == NULL) return -1;
	sto->base.dev->erase(dev, dev_addr, size);
	
	sto->base.dev->close(dev);

	return 0;
}

/**
 * @brief spi flash驱动  
 * 
 */
extern StorageDev StorageExSpiFlash;

/**
 * @brief   根据分区表进行分区初始化
 * @details 包含storage和partition两部分初始化
 *          storage进行绑定设备驱动。
 *          partition则根据分区类型进行初始化，比如初始化文件系统。
 * @param   Partable    分区表，包含storage和partition信息
 * @return  int 
 * @note    分区表一定要先定义storage，紧跟着定义这个storage的partition
 */
int petite_partition_init(PartitionDef *Partable)
{
	PartitionDef *pardef;
	PartitionNode *node;
	static PartitionNode *sto;
	
	pardef = Partable;
	
	while(1) {
		if (pardef->maptype == NULL) break;

		/* 申请一个节点 */
		node = (PartitionNode *)wjq_malloc(sizeof(PartitionNode));
		/* 登记分区信息 */
		node->def = pardef;
		/*  插入node链表	*/
		node->next = PartitionRoot;
		PartitionRoot = node;
		
		if (strcmp(pardef->maptype, "sto") == 0){
			/* sto 分区用来指定设备存储空间，在初始化的时候，找到对应驱动，
				并将设备信息与设备驱动绑定在一起 */
			sto = node;
			if (strcmp(node->def->type, "spiflash") == 0 ) {
				node->base.dev = &StorageExSpiFlash;
			}
			
		} else if (strcmp(pardef->maptype, "par") == 0) {
			/* par 分区，指明空间用途，在初始化时，将par与sto绑定，
				并根据par类型进行初始化，如挂载对应文件系统。*/
			if (pardef->addr >= sto->def->addr
				&& pardef->addr +  pardef->size <= sto->def->addr + sto->def->size) {
				/* 把分区和storage关联 */
				node->base.sto = sto;

				if (strcmp(pardef->type, "littlefs") == 0) {
					vfs_mount(node);
				} else if(strcmp(pardef->type, "fatfs") == 0){
					vfs_mount(node);
				}
			} else  {
				wjq_log(LOG_ERR, "partiton addr over storage!\r\n");
			}
				
		}
		pardef++;
	}	

	#if 0
	node = PartitionRoot;
	while(1) {
		
		if(node == NULL) break;

		wjq_log(LOG_INFO, "%s %s %s 0x%x 0x%x\r\n", 
				node->def->maptype, node->def->type,node->def->name,node->def->addr,node->def->size);
	
		node = node->next;
	
	}
	#endif

	return 0;
}

#if 0
uint8_t rbuf[64];
uint8_t wbuf[64];

void petite_partition_test(void)
{
	void *part;

	part = petite_partition_get("kvdb");

	petite_partition_read(part, 0, rbuf, 64);

	PrintFormat(rbuf, 64);

	petite_partition_erase(part, 0,  64);
	
	wbuf[0] = 0x66;
	wbuf[1] = 0x77;
	
	petite_partition_write(part, 0, wbuf, 64);
	
	petite_partition_read(part, 0, rbuf, 64);
	PrintFormat(rbuf, 64);
	
}
#endif

