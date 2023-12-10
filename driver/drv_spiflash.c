/**
 * @file            dev_spiflash.c
 * @brief           spi flash 驱动程序
 * @author          test
 * @date            2017年10月26日 星期四
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:      2017年10月26日 星期四
 *   作    者:      test
 *   修改内容:      创建文件
  版权说明：
		1 源码归屋脊雀工作室所有。
		2 可以用于的其他商业用途（配套开发板销售除外），不须授权。
		3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
		4 可随意修改源码并分发，但不可直接销售本代码获利，并且请保留WUJIQUE版权说明。
		5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
		6 使用本源码则相当于认同本版权说明。
		7 如侵犯你的权利，请联系：code@wujique.com
		8 一切解释权归屋脊雀工作室所有。
*/
#include "mcu.h"
#include "petite_config.h"
#include "petite.h"

#if (DRV_SPI_FLASH_MODULE == 1)
#include "board_sysconf.h"
#include "log.h"
#include "drv_spiflash.h"

//#define DEV_SPIFLASH_DEBUG

#ifdef DEV_SPIFLASH_DEBUG
#define SPIFLASH_DEBUG(l, args...) petite_log(l, "SPI FLASH", NULL,__FUNCTION__, __LINE__, ##args); 
#else
#define SPIFLASH_DEBUG(a, ...)
#endif

#define LogSpiFlash(l, args...) petite_log(l, "SPI FLASH", NULL,__FUNCTION__, __LINE__, ##args); 

/*
	SPI FLASH，命令的开始是从CS下降沿开始，所以每次操作都需要进行CS操作
*/

/*
	常用的SPI FLASH 参数信息
*/
const NorFlashPra SpiFlashPraList[]=
{
	{"default",    0Xef4017, 0Xef16, 2048, 4096, 8388608},
	{"MX25L3206E", 0XC22016, 0XC215, 1024, 4096, 4194304},
	{"W25Q64JVSI", 0Xef4017, 0Xef16, 2048, 4096, 8388608}
};
	
/* spi flash 命令*/
#define SPIFLASH_WRITE      0x02  /* Write to Memory instruction  Page Program */
#define SPIFLASH_WRSR       0x01  /* Write Status Register instruction */
#define SPIFLASH_WREN       0x06  /* Write enable instruction */
#define SPIFLASH_WRDIS      0x04  /* Write disable instruction */
#define SPIFLASH_READ       0x03  /* Read from Memory instruction */
#define SPIFLASH_FREAD      0x0B  /* Fast Read from Memory instruction */
#define SPIFLASH_RDSR       0x05  /* Read Status Register instruction  */
#define SPIFLASH_SE         0x20  /* Sector Erase instruction */
#define SPIFLASH_BE         0xD8  /* Bulk Erase instruction */
#define SPIFLASH_CE         0xC7  /* Chip Erase instruction */
#define SPIFLASH_PD         0xB9  /* Power down enable */
#define SPIFLASH_RPD        0xAB  /* Release from Power down mode */
#define SPIFLASH_RDMID      0x90  /* Read Device identification */
#define SPIFLASH_RDJID      0x9F  /* Read JEDEC identification */
#define SPIFLASH_DUMMY_BYTE 0xA5

/**
 *@brief:      dev_spiflash_writeen
 *@details:    FLASH 写使能:设置FLASH内的寄存器，
               允许写/擦除，每次写/擦除之前都需要发送
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
static s32 dev_spiflash_writeen(DevSpiFlashNode *node)  
{
    s32 len = 1;
    u8 command = SPIFLASH_WREN;
	bus_spich_cs(node->spichnode, 0);
	bus_spich_transfer(node->spichnode, &command, NULL, len); //数据传输
	bus_spich_cs(node->spichnode, 1);
    return 0;
}

/**
 *@brief:      dev_spiflash_waitwriteend
 *@details:    查询FLASH状态，等待写操作结束
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
static s32 dev_spiflash_waitwriteend(DevSpiFlashNode *node)
{
    u8 flash_status = 0;
    s32 len = 1;
    u8 command = SPIFLASH_RDSR;
	
	bus_spich_cs(node->spichnode, 0);
    bus_spich_transfer(node->spichnode, &command, NULL, len);
    do{
		
        bus_spich_transfer(node->spichnode, NULL, &flash_status, len);
    }
    while ((flash_status & 0x01) != 0); 
	bus_spich_cs(node->spichnode, 1);
		
	return 0;
}
/**
 *@brief:      dev_spiflash_erase
 *@details:    擦除一个sector
 *@param[in]   u32 addr  地址，包含这个地址的sector将被擦除
 *@param[out]  无
 *@retval:     
 */
s32 dev_spiflash_erase(DevSpiFlashNode *node, u32 addr)
{
    s32 len = 4;
    u8 command[4];

    command[0] = SPIFLASH_SE;
    command[1] = (u8)(addr>>16);
    command[2] = (u8)(addr>>8);
    command[3] = (u8)(addr);
    
    dev_spiflash_writeen(node);
	
	bus_spich_cs(node->spichnode, 0);
    bus_spich_transfer(node->spichnode, command, NULL, len);
	bus_spich_cs(node->spichnode, 1);

    dev_spiflash_waitwriteend(node);   

return 0;	
}

/**
 *@brief:      dev_spiflash_readmorebyte
 *@details:    读随意长度FLASH数据
 *@param[in]   u32 addr  
               u8 *dst  
               u32 len      
 *@param[out]  无
 *@retval:     
 */
s32 dev_spiflash_read(DevSpiFlashNode *node, u32 addr, u32 rlen, u8 *dst)
{
    
    s32 len = 4;
    u8 command[4];

    if(rlen == 0)return 0;

    command[0] = SPIFLASH_READ;
    command[1] = (u8)(addr>>16);
    command[2] = (u8)(addr>>8);
    command[3] = (u8)(addr);
	
	bus_spich_cs(node->spichnode, 0);
    bus_spich_transfer(node->spichnode, command, NULL, len); 
    bus_spich_transfer(node->spichnode, NULL, dst, rlen);
	bus_spich_cs(node->spichnode, 1);
    return(0);
}
/**
 *@brief:      dev_spiflash_write
 *@details:    写数据到FLASH
 *@param[in]   u8* pbuffer     
               u32 addr  
               u16 wlen        
 *@param[out]  无
 *@retval:     
 */
s32 dev_spiflash_write(DevSpiFlashNode *node,  u32 addr, u16 wlen, u8* src)
{
    s32 len;
    u8 command[4];

    while (wlen) {    
		dev_spiflash_writeen(node);
	
        command[0] = SPIFLASH_WRITE;
        command[1] = (u8)(addr>>16);
        command[2] = (u8)(addr>>8);
        command[3] = (u8)(addr);

        len = 4;
		bus_spich_cs(node->spichnode, 0);
        bus_spich_transfer(node->spichnode, command, NULL, len);
		
        len = 256 - (addr & 0xff);
        if (len < wlen) {
            bus_spich_transfer(node->spichnode, src, NULL, len);
            wlen -= len;
            src += len;
            addr += len;
        } else {
            bus_spich_transfer(node->spichnode, src, NULL, wlen);
            wlen = 0;
            addr += wlen;
            src += wlen;
        }
		
		bus_spich_cs(node->spichnode, 1);
		
        dev_spiflash_waitwriteend(node);       
    }
		
	return 0;
}

/**
 *@brief:      dev_spiflash_sector_erase
 *@details:    擦除一个sector
 *@param[in]   u32 sector  sector号
 *@param[out]  无
 *@retval:     
 */
s32 dev_spiflash_sector_erase(DevSpiFlashNode *node, u32 sector)
{
	u32 addr;

	if(sector >= node->pra->sectornum)
		return -1;
	
	addr = sector*(node->pra->sectorsize);

	dev_spiflash_erase(node, addr);
	
	return 0;	
}

/**
 *@brief:      dev_spiflash_sector_read
 *@details:    读取一个扇区
 *@param[in]   DevSpiFlash *dev  
               u32 sector  扇区号
               u8 *dst           
 *@param[out]  无
 *@retval:     
 */
s32 dev_spiflash_sector_read(DevSpiFlashNode *node, u32 sector, u8 *dst)	
{
	if(sector >= node->pra->sectornum)
		return -1;
	
	return dev_spiflash_read(node, sector*(node->pra->sectorsize), node->pra->sectorsize, dst);
}
/**
 *@brief:      dev_spiflash_sector_write
 *@details:    写一个扇区
 *@param[in]   DevSpiFlash *dev  
               u32 sector        
               u8 *src           
 *@param[out]  无
 *@retval:     
 */
s32 dev_spiflash_sector_write(DevSpiFlashNode *node, u32 sector, u8 *src)
{
	u16 sector_size;

	if(sector >= node->pra->sectornum)
		return -1;
	
	sector_size = node->pra->sectorsize;
	dev_spiflash_write(node, sector*sector_size, sector_size, src);
	return 0;
}
/**
 *@brief:      dev_spiflash_readMTD
 *@details:    读FLASH MID号
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
static u32 dev_spiflash_readMTD(DevSpiFlashNode *node)
{
    u32 MID;
    s32 len = 4;
    u8 command[4];
    u8 data[2];

    command[0] = SPIFLASH_RDMID;
    command[1] = 0;
    command[2] = 0;
    command[3] = 0;
	
	bus_spich_cs(node->spichnode, 0);
    bus_spich_transfer(node->spichnode, command, NULL, len);
    len = 2;
    bus_spich_transfer(node->spichnode, NULL, data, len);
	bus_spich_cs(node->spichnode, 1);
    MID = data[0];
    MID = (MID<<8) + data[1];
         
    return MID;
}
/**
 *@brief:      dev_spiflash_readJTD
 *@details:    读FLASH JTD号
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
static u32 dev_spiflash_readJTD(DevSpiFlashNode *node)
{
    u32 JID;
    s32 len = 1;
    u8 command = SPIFLASH_RDJID;
    u8 data[3];
	
	bus_spich_cs(node->spichnode, 0);
    len = 1;
    bus_spich_transfer(node->spichnode, &command, NULL, len);
    len = 3;
    bus_spich_transfer(node->spichnode, NULL, data, len);
	bus_spich_cs(node->spichnode, 1);
	
    JID = data[0];
    JID = (JID<<8) + data[1];
    JID = (JID<<8) + data[2];
    
    return JID;
}

/**
 *@brief:      SpiFlashOpen
 *@details:    打开SPI FLASH
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void *dev_spiflash_open(char* name)
{

	DevSpiFlashNode *node;
	PDevNode *pnode;	
	PetiteDev *pdev;
	
	SPIFLASH_DEBUG(LOG_INFO, "spi flash open:%s!\r\n", name);

	node = NULL;

	pnode = petite_dev_get_node(name);
	node = (DevSpiFlashNode *)pnode;
	pdev = pnode->pdev;
	
	/* 打开设备 */
	if (node != NULL) {
		
		if (node->gd == 0) {
			SPIFLASH_DEBUG(LOG_INFO, "spi flash open err:using!\r\n");
			node = NULL;
		} else {
			node->spichnode = bus_spich_opennode((DevSpiChNode *)pnode->basenode, SPI_MODE_3, 20000, 0xffffffff);
			//node->spichnode = bus_spich_open(pdev->basebus, SPI_MODE_3, 20000, 0xffffffff);
			if(node->spichnode == NULL)	node = NULL;
			else node->gd = 0;
		}
	}

	SPIFLASH_DEBUG(LOG_INFO, "spi flash open :%d!\r\n", node);
	
	return (void *)node;
}

/*
	获取设备节点，后续通过节点操作，
	避免查找节点浪费时间
	*/
void *dev_spiflash_getnode(char* name)
{
	DevSpiFlashNode *node;
	PDevNode *pnode;	
	
	SPIFLASH_DEBUG(LOG_INFO, "spi flash open:%s!\r\n", name);

	pnode = petite_dev_get_node(name);
	node = (DevSpiFlashNode *)pnode;

	return (void *)node;
}


/**
 *@brief:      SpiFlashOpennode
 *@details:    	通过节点打开设备
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void *dev_spiflash_opennode(void *devnode)
{
	DevSpiFlashNode *node;
	PDevNode *pnode;	
	PetiteDev *pdev;
	
	node = (DevSpiFlashNode *)devnode;
	pnode =(PDevNode *)node;
	pdev = pnode->pdev;
	
	/* 打开设备 */
	if (node != NULL) {
		
		if (node->gd == 0) {
			SPIFLASH_DEBUG(LOG_INFO, "spi flash open err:using!\r\n");
			node = NULL;
		} else {
			node->spichnode = bus_spich_opennode((DevSpiChNode *)pnode->basenode, SPI_MODE_3, 20000, 0xffffffff);
			//node->spichnode = bus_spich_open(pdev->basebus, SPI_MODE_3, 20000, 0xffffffff);
			if(node->spichnode == NULL)	node = NULL;
			else node->gd = 0;
		}
	}

	SPIFLASH_DEBUG(LOG_INFO, "spi flash open :%d!\r\n", node);
	
	return (void *)node;
}


/**
 *@brief:      dev_spiflash_close
 *@details:       关闭SPI FLASH设备
 *@param[in]  DevSpiFlash *dev  
 *@param[out]  无
 *@retval:     
 */
s32 dev_spiflash_close(DevSpiFlashNode *node)
{
	if(node->gd != 0)
		return -1;
	
	bus_spich_close(node->spichnode); 

	node->gd = -1;
	return 0;
}


PDevNode *dev_spiflash_register(const DevSpiFlash *dev)
{
	DevSpiFlashNode *flashnode;

	PetiteDev *pdev;

	pdev = (PetiteDev *)dev;

	u32 JID = 0;
	u32 MID = 0;
	u8 index = 0;
	
	LogSpiFlash(LOG_INFO, "name:%s!\r\n", pdev->name);

	/* 申请一个节点空间 */
	flashnode = (DevSpiFlashNode *)wjq_malloc(sizeof(DevSpiFlashNode));
	petite_dev_init_node((PDevNode *)flashnode, (PetiteDev *) dev);
	

	flashnode->gd = -1;

	/*读 ID，超找FLASH信息*/
	flashnode->spichnode = bus_spich_open(pdev->basebus, SPI_MODE_3, 10000, 0xffffffff); //打开spi

	if (flashnode->spichnode != NULL) {
		JID = dev_spiflash_readJTD(flashnode);
		LogSpiFlash(LOG_DEBUG, "%s jid:0x%x\r\n", pdev->name, JID);
		
		//MID  = dev_spiflash_readMTD(node);
		//LogSpiFlash(LOG_DEBUG, "%s mid:0x%x\r\n", dev->pnode.name, MID);
		
		/*根据JID查找设备信息*/
		flashnode->pra = &(SpiFlashPraList[0]);
		for (index = 1; index<(sizeof(SpiFlashPraList)/sizeof(NorFlashPra));index++) {
			if (SpiFlashPraList[index].JID == JID) {
				flashnode->pra = &(SpiFlashPraList[index]);
				LogSpiFlash(LOG_DEBUG, "flash name %s\r\n", flashnode->pra->name);
				break;
			}
		}
		
		bus_spich_close(flashnode->spichnode);
	}
	return (PDevNode *)flashnode;
}

#include "petite.h"

#if 0
/**
 *@brief:      dev_spiflash_test_fun
 *@details:    测试FLASH,擦除写读，调试时使用，量产存数据后不要测试
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void dev_spiflash_test_fun(char *name)
{
    u32 addr;
    u16 tmp;
    u8 i = 1;
    u8 rbuf[4096];
    u8 wbuf[4096];
    u8 err_flag = 0;

	DevSpiFlashNode *node;
	
	
    wjq_log(LOG_DEBUG, ">:-------dev_spiflash_test-------\r\n");
    node = dev_spiflash_open(name);
	//wjq_log(LOG_DEBUG, ">:-------%s-------\r\n", node->dev.pnode.name);
	if (node == NULL) {
		wjq_log(LOG_DEBUG, "open spi flash ERR\r\n");
		while(1);
	}
    i = 0;
    for (tmp = 0; tmp < 4096; tmp++) {
        wbuf[tmp] = i;
        i++;
    }
    //sector 1 进行擦除，然后写，校验。
    wjq_log(LOG_DEBUG, ">:-------test sector erase-------\r\n", addr);
    
    addr = 0;
    dev_spiflash_sector_erase(node, addr);
    wjq_log(LOG_DEBUG, "erase...");

    dev_spiflash_sector_read(node, addr, rbuf);;//读一页回来
    wjq_log(LOG_DEBUG, "read...");
    
    for (tmp = 0; tmp < node->pra->sectorsize; tmp++) {
        if (rbuf[tmp] != 0xff) {
            wjq_log(LOG_DEBUG, "%x=%02X ", tmp, rbuf[tmp]);//擦除后不等于0XFF,坏块    
            err_flag = 1;
        }
    }

    dev_spiflash_sector_write(node, addr, wbuf);
    wjq_log(LOG_DEBUG, "write...");
    
    dev_spiflash_sector_read(node, addr, rbuf);
    wjq_log(LOG_DEBUG, "read...");
    
    wjq_log(LOG_DEBUG, "\r\n>:test wr..\r\n");
    
    for (tmp = 0; tmp < node->pra->sectorsize; tmp++) {
        if (rbuf[tmp] != wbuf[tmp]) {
            wjq_log(LOG_DEBUG, "%x ", tmp);//读出来的跟写进去的不相等 
            err_flag = 1;
        }
    }

    if (err_flag == 1)
        wjq_log(LOG_DEBUG, "bad sector\r\n");
    else
        wjq_log(LOG_DEBUG, "OK sector\r\n");

	dev_spiflash_close(node);
}
/*
	检测整片FLASH
*/
void dev_spiflash_test_chipcheck(char *name)
{
    u32 addr;
	u16 sector;
    u16 tmp;
    u8 i = 1;
    u8 *rbuf;
    u8 *wbuf;
    u8 err_flag = 0;

	DevSpiFlashNode *node;
	
    wjq_log(LOG_DEBUG, ">:-------dev_spiflash_test-------\r\n");
    node = dev_spiflash_open(name);

	if (node == NULL) {
		wjq_log(LOG_DEBUG, "open spi flash ERR\r\n");
		while(1);
	}

	rbuf = (u8*)wjq_malloc(node->pra->sectorsize);
	wbuf = (u8*)wjq_malloc(node->pra->sectorsize);

	for (sector = 0; sector < node->pra->sectornum; sector++) {
	    i = sector%0xff;
		
	    for (tmp = 0; tmp < node->pra->sectorsize; tmp++) {
	        wbuf[tmp] = i;
	        i++;
	    }
	    
	    addr = sector * (node->pra->sectorsize);
		
		wjq_log(LOG_DEBUG, ">:sector:%d, addr:0x%08x,", sector, addr);
	    dev_spiflash_sector_erase(node, sector);
	    wjq_log(LOG_DEBUG, "erase...");

	    dev_spiflash_sector_read(node, sector, rbuf);;//读一页回来
	    wjq_log(LOG_DEBUG, "read...");
	    
	    for (tmp = 0; tmp < node->pra->sectorsize; tmp++) {
	        if (rbuf[tmp] != 0xff) {
	            //wjq_log(LOG_FUN, "%x=%02X ", tmp, rbuf[tmp]);//擦除后不等于0XFF,坏块    
	            err_flag = 1;
	        }
	    }

	    dev_spiflash_sector_write(node, sector, wbuf);
	    wjq_log(LOG_DEBUG, "write...");
	    
	    dev_spiflash_sector_read(node, sector, rbuf);
	    wjq_log(LOG_DEBUG, "read...");
	    
	    for (tmp = 0; tmp < node->pra->sectorsize; tmp++) {
	        if (rbuf[tmp] != wbuf[tmp]) {
	            //wjq_log(LOG_FUN, "%x ", tmp);//读出来的跟写进去的不相等 
	            err_flag = 1;
	        }
	    }

	    if (err_flag == 1)
	        wjq_log(LOG_DEBUG, "bad sector\r\n");
	    else
	        wjq_log(LOG_DEBUG, "OK sector\r\n");
	}
	dev_spiflash_close(node);

	wjq_free(rbuf);
	wjq_free(wbuf);
}

void dev_spiflash_test_chiperase(char *name)
{
    u32 addr;
	u16 sector;
    u16 tmp;
    u8 i = 1;
    u8 *rbuf;
    u8 *wbuf;
    u8 err_flag = 0;

	DevSpiFlashNode *node;
	
    wjq_log(LOG_DEBUG, ">:-------dev_spiflash_test-------\r\n");
    node = dev_spiflash_open(name);

	if (node == NULL) {
		wjq_log(LOG_DEBUG, "open spi flash ERR\r\n");
		while(1);
	}
	
	rbuf = (u8*)wjq_malloc(node->pra->sectorsize);

	for (sector = 0; sector < node->pra->sectornum; sector++) {

	    addr = sector * (node->pra->sectorsize);
		
		wjq_log(LOG_DEBUG, ">:sector:%d, addr:0x%08x,", sector, addr);
	    dev_spiflash_sector_erase(node, sector);
	    wjq_log(LOG_DEBUG, "erase...");

	    dev_spiflash_sector_read(node, sector, rbuf);;//读一页回来
	    wjq_log(LOG_DEBUG, "read...");
	    
	    for (tmp = 0; tmp < node->pra->sectorsize; tmp++) {
	        if (rbuf[tmp] != 0xff) {
	            //wjq_log(LOG_FUN, "%x=%02X ", tmp, rbuf[tmp]);//擦除后不等于0XFF,坏块    
	            err_flag = 1;
	        }
	    }

	    if(err_flag == 1)
	        wjq_log(LOG_DEBUG, "bad sector\r\n");
	    else
	        wjq_log(LOG_DEBUG, "OK sector\r\n");
	}
	dev_spiflash_close(node);

	wjq_free(rbuf);

}


s32 dev_spiflash_test(void)
{
	dev_spiflash_test_fun("board_spiflash");
	dev_spiflash_test_fun("core_spiflash");
	return 0;
}
#endif

/**------------------------------封装接口给partition使用-----------------------------------------*/

int storage_spiflash_read(void *node, uint32_t offset, uint8_t *buf, size_t size)
{
	return dev_spiflash_read((DevSpiFlashNode *)node, offset, size, buf);	

}
int storage_spiflash_write(void *node, uint32_t offset, const uint8_t *buf, size_t size)
{
	return dev_spiflash_write((DevSpiFlashNode *)node, offset, size, (uint8_t *)buf);
}
int storage_spiflash_erase(void *node, uint32_t offset, size_t size)
{
	size_t esize=0;
	DevSpiFlashNode *flashnode = node;
	
	while(1) {
		if(esize >= size) break;
		
		dev_spiflash_erase(flashnode, offset+esize);
		esize +=flashnode->pra->sectorsize; 
	}
	return 0;
}


int storage_spiflash_getblksize(void *node)
{
	int blksize;

	DevSpiFlashNode *flashnode = node;

	blksize = flashnode->pra->sectorsize;
	SPIFLASH_DEBUG(LOG_DEBUG, "%s, sectorsize:0x%xByte\r\n",flashnode->pra->name, blksize);
	
	return blksize;	

}


int storage_spiflash_getsize(void *node)
{
	int structure_size;

	DevSpiFlashNode *flashnode;
	

	flashnode = node;

	PetiteDev *pdev;
	pdev = flashnode->pnode.pdev;
	SPIFLASH_DEBUG(LOG_DEBUG, "get size %s\r\n",pdev->name);
	
	SPIFLASH_DEBUG(LOG_DEBUG, "get size %s\r\n",flashnode->pra->name);
	
	structure_size = flashnode->pra->structure;
	SPIFLASH_DEBUG(LOG_DEBUG, "structure size:0x%xByte\r\n", structure_size);
	return structure_size;	

}


#include "partition.h"

const StorageDev StorageExSpiFlash ={
	
	.getblksize = storage_spiflash_getblksize,
	.getsize = storage_spiflash_getsize,

	.getnode = dev_spiflash_getnode,
	.open = dev_spiflash_opennode,	
	.read = storage_spiflash_read,
	.write = storage_spiflash_write,
	.erase = storage_spiflash_erase,
	.close = dev_spiflash_close,
};



#endif

