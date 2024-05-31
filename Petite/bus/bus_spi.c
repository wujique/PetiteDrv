/**
 * @file            bus_spi.c
 * @brief           SPI总线管理
 * @author          test
 * @date            2019年03月26日 星期四
 * @version         初稿
 * @par             
 * @par History:
 * 1.日    期:      
 *   作    者:      test
 *   修改内容:      创建文件
		版权说明：
		1 源码归屋脊雀工作室所有。
		2 可以用于的其他商业用途（配套开发板销售除外），不须授权。
		3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
		4 可随意修改源码并分发，但不可直接销售本代码获利，并且保留版权说明。
		5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
		6 使用本源码则相当于认同本版权说明。
		7 如侵犯你的权利，请联系：code@wujique.com
		8 一切解释权归屋脊雀工作室所有。
*/
#include "mcu.h"
#include "cmsis_os.h"
#include "log.h"
#include "board_sysconf.h"
#include "petite.h"

#define BUS_SPI_DEBUG

#ifdef BUS_SPI_DEBUG
#define BUSSPI_DEBUG(l, args...) petite_log(l, "BUS SPI", NULL,__FUNCTION__, __LINE__, ##args); 
#else
#define BUSSPI_DEBUG(a, ...)
#endif


/**
 *@brief:      mcu_spi_register
 *@details:    注册SPI控制器设备
 *@param[in]   DevSpi *dev      
 *@param[out]  无
 *@retval:     
 */
PDevNode *bus_spi_register(const DevSpi *dev)
{

	struct list_head *listp;
	DevSpiNode *pnode;
	
	//wjq_log(LOG_DEBUG, "[register]:%s!\r\n", dev->pdev.name);
	/*  申请一个节点空间    	*/
	pnode = (DevSpiNode *)wjq_malloc(sizeof(DevSpiNode));
	petite_dev_init_node((PDevNode *)pnode, (PetiteDev *) dev);
	
	/*初始化*/
	if (dev->pdev.type == BUS_SPI_V)
		bus_vspi_init(dev);
	else if (dev->pdev.type == BUS_SPI_H)
		mcu_hspi_init(dev->pdev.name, &(dev->io));
	else {
		wjq_log(LOG_ERR, "spi register type err!\r\n");	
	}
	
	pnode->mutex = osMutexNew(NULL);
	if (pnode->mutex == NULL) {
		BUSSPI_DEBUG(LOG_ERR, "mutex new err!\r\n");
	} else {
		BUSSPI_DEBUG(LOG_WAR, "mutex new 0x%x!\r\n", pnode->mutex);	
	}
	
	
	return (PDevNode *)pnode;
}

/**
 *@brief:      mcu_spich_register
 *@details:    注册SPI通道
 *@param[in]   DevSpiCh *dev     
 *@param[out]  无
 *@retval:     
 */
PDevNode *bus_spich_register(const DevSpiCh *dev)
{
	struct list_head *listp;
	DevSpiChNode *pnode;

	BUSSPI_DEBUG(LOG_DEBUG, "register spi ch :%s!\r\n", dev->pdev.name);

	/* 申请一个节点空间 	*/
	pnode = (DevSpiChNode *)wjq_malloc(sizeof(DevSpiChNode));
	
	petite_dev_init_node((PDevNode *)pnode, (PetiteDev *) dev);

	pnode->gd = -1;

	/* 初始化管脚 */
	mcu_io_config_out(dev->csport,dev->cspin);
	mcu_io_output_setbit(dev->csport,dev->cspin);

	return (PDevNode *)pnode;
}

DevSpiChNode *bus_spich_opennode(DevSpiChNode *node, SPI_MODE mode, u16 KHz,  uint32_t wait);
/**
 *@brief:      bus_spi_open
 *@details:    打开SPI通道
 *@param[in]   DevSpiChNode * node
               u8 mode      模式
               u16 pre      预分频
 *@param[out]  无
 *@retval:     
 			   打开一次SPI，在F407上大概要2us
 */
DevSpiChNode *bus_spich_open(const char *name, SPI_MODE mode, u16 KHz,  uint32_t wait)
{
	PDevNode *pnode;
	DevSpiChNode *chnode;

	pnode = petite_dev_get_node((char *)name);
	chnode = (DevSpiChNode *)pnode;

	//BUSSPI_DEBUG(LOG_DEBUG, "dev spi ch name:%s!\r\n", pnode->pdev->name);
	//BUSSPI_DEBUG(LOG_DEBUG, "dev spi name:%s!\r\n", pbasenode->pdev->name);
	
	chnode = bus_spich_opennode(chnode, mode, KHz,  wait);
	
	return chnode;
}


/**
 *@brief:      bus_spi_opennode
 *@details:    打开SPI通道
 *@param[in]   DevSpiChNode * node
               u8 mode      模式
               u16 pre      预分频
 *@param[out]  无
 *@retval:     
 			   打开一次SPI，在F407上大概要2us
 */
DevSpiChNode *bus_spich_opennode(DevSpiChNode *node, SPI_MODE mode, u16 KHz,  uint32_t wait)
{

	s32 res;
	PDevNode *pnode;

	PDevNode *pbasenode;
	DevSpiChNode *chnode;

	DevSpiNode *spinode;
	DevSpiCh *devspich;
	
	osStatus_t osres;
	
	pnode = (PDevNode *)node;
	
	chnode = node;
	devspich = (DevSpiCh *)pnode->pdev;
		
	pbasenode = pnode->basenode;
	spinode = (DevSpiNode *)pbasenode;

	//BUSSPI_DEBUG(LOG_DEBUG, "dev spi ch name:%s!\r\n", pnode->pdev->name);
	//BUSSPI_DEBUG(LOG_DEBUG, "dev spi name:%s!\r\n", pbasenode->pdev->name);
	
	if (chnode != NULL) {
		
		if(chnode->gd == 0) {
			BUSSPI_DEBUG(LOG_ERR, "spi ch open err:using!\r\n");
			chnode = NULL;
		} else {
			osres = osMutexAcquire(spinode->mutex, wait);

			if ( osOK != osres) {
				BUSSPI_DEBUG(LOG_ERR, "spi ch open err mutex:%s!\r\n", pbasenode->pdev->name);
				chnode = NULL;
			} else {
				spinode->clk = KHz;
				spinode->mode = mode;

				/*打开SPI控制器*/
				res = -1;
				if (pbasenode->pdev->type == BUS_SPI_H) {
					res = mcu_hspi_open(spinode->pnode.pdev->name, mode, KHz);	
				} else if(pbasenode->pdev->type == BUS_SPI_V) {
					res = bus_vspi_open(spinode, mode, KHz);	
				} else {
					BUSSPI_DEBUG(LOG_ERR, "spi type err!\r\n");		
				}

				if (res == 0) {
					chnode->gd = 0;
					//BUSSPI_DEBUG(LOG_DEBUG, "spi dev open ok: %s!\r\n", pbasenode->pdev->name);
					mcu_io_output_resetbit(devspich->csport, devspich->cspin);
				} else {
					BUSSPI_DEBUG(LOG_ERR, "spi dev open err!\r\n");
					chnode = NULL;
					osMutexRelease(spinode->mutex);
				}
			}	
		}
	}else {
		BUSSPI_DEBUG(LOG_ERR, "spi ch no exist!\r\n");	
	}
	
	return chnode;
}

/**
 *@brief:      mcu_spi_close
 *@details:    关闭SPI 通道
 *@param[in]   DevSpiChNode * node  
 *@param[out]  无
 *@retval:     
 */
s32 bus_spich_close(DevSpiChNode * node)
{
	PDevNode *pnode;
	DevSpiChNode *chnode;

	PDevNode *pbasenode;
	DevSpiNode *spinode;
	
	DevSpiCh *devspich;
	
	
	chnode = node;
	pnode = (PDevNode *)chnode;
	devspich = (DevSpiCh *)pnode->pdev;
	pbasenode = pnode->basenode;
	spinode = (DevSpiNode *)pbasenode;

	//BUSSPI_DEBUG(LOG_INFO, "close spi ch:%s\r\n", pnode->pdev->name);
	//BUSSPI_DEBUG(LOG_INFO, "close spi:%s\r\n", pbasenode->pdev->name);
	
	if (chnode == NULL) return -1;
	if(chnode->gd != 0) return -1;

	if(pbasenode->pdev->type == BUS_SPI_H){
		mcu_hspi_close(spinode->pnode.pdev->name);
	} else if(pbasenode->pdev->type == BUS_SPI_V) { 
		bus_vspi_close(spinode);
	} else {
		BUSSPI_DEBUG(LOG_WAR, "close spi type err!\r\n");
	}
	/*拉高CS*/

	mcu_io_output_setbit(devspich->csport, devspich->cspin);

	chnode->gd = -1;
	
 	osMutexRelease(spinode->mutex);

	return 0;
}
/**
 *@brief:      mcu_spi_transfer
 *@details:    SPI 传输
 *@param[in]   DevSpiChNode * node
 			   u8 *snd  
               u8 *rsv  
               s32 len  
 *@param[out]  无
 *@retval:     
 */
s32 bus_spich_transfer(DevSpiChNode * node, u8 *snd, u8 *rsv, s32 len)
{
	PDevNode *pnode;

	PDevNode *pbasenode;
	DevSpiChNode *chnode;

	DevSpiNode *spinode;
	DevSpiCh *devspich;
	
	chnode = node;
	pnode = (PDevNode *)chnode;

	pbasenode = pnode->basenode;
	spinode = (DevSpiNode *)pbasenode;
	
	if (node == NULL) return -1;

	if (pbasenode->pdev->type == BUS_SPI_H)
		return mcu_hspi_transfer(spinode->pnode.pdev->name, snd, rsv, len);
	else if (pbasenode->pdev->type == BUS_SPI_V)	
		return bus_vspi_transfer(spinode, snd, rsv, len);
	else {
		BUSSPI_DEBUG(LOG_ERR, "spi dev type err\r\n");	
	}
	return -1;
}
/**
 *@brief:      mcu_spi_cs
 *@details:    操控对应SPI的CS
 *@param[in]   DevSpiChNode * node  
               u8 sta   1 高电平，0 低电平     
 *@param[out]  无
 *@retval:     
 */
s32 bus_spich_cs(DevSpiChNode * node, u8 sta)
{
	DevSpiCh *devspich;

	devspich = (DevSpiCh *)node->pnode.pdev;
	
	switch(sta)
	{
		case 1:
			mcu_io_output_setbit(devspich->csport, devspich->cspin);
			break;
			
		case 0:
			mcu_io_output_resetbit(devspich->csport, devspich->cspin);
			break;
			
		default:
			return -1;

	}

	return 0;
}

#if 0
void spi_example(void)
{
	DevSpiChNode *spichnode;
	u8 src[128];
	u8 rsv[128];
	uint16_t i;
	
	Uprintf("\r\n----------test spi -----------\r\n");
	/*打开SPI通道*/
	spichnode = bus_spich_open("SPI3_CH3", SPI_MODE_1, 40000, 0xffffffff);
	if (spichnode == NULL) {
		while(1);
	}
	/*读10个数据*/
	//bus_spich_transfer(spichnode, NULL, rsv, 10);
	/*写10个数据*/
	//bus_spich_transfer(spichnode, src, NULL, 10);
	/*读写10个数据*/
	for(i=0;i<128;i++)
		src[i] = i;
	
	//memset(src, 0x58, 20);
	while(1) {
		bus_spich_cs(spichnode, 0);
		bus_spich_transfer(spichnode, src, rsv, 128);
		bus_spich_cs(spichnode, 1);
		DUMP_HEX_16BYTE(rsv, 128);
		Delay(1000);
	}
	bus_spich_close(spichnode);

	Uprintf("test spi finish\r\n");
	while(1);
}

#endif

