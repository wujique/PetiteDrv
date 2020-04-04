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
#include "log.h"
#include "p_list.h"
#include "board_sysconf.h"

#include "bus_spi.h"
#include "bus_vspi.h"

#define BUS_SPI_DEBUG

#ifdef BUS_SPI_DEBUG
#define BUSSPI_DEBUG	wjq_log 
#else
#define BUSSPI_DEBUG(a, ...)
#endif

/*


	所有SPI统一对外接口


*/
struct list_head DevSpiRoot = {&DevSpiRoot, &DevSpiRoot};
/**
 *@brief:      mcu_spi_register
 *@details:    注册SPI控制器设备
 *@param[in]   DevSpi *dev      
 *@param[out]  无
 *@retval:     
 */
s32 mcu_spi_register(const DevSpi *dev)
{

	struct list_head *listp;
	DevSpiNode *p;
	
	wjq_log(LOG_INFO, "[register] spi :%s!\r\n", dev->name);

	/*
		先要查询当前，防止重名
	*/
	listp = DevSpiRoot.next;
	while(1)
	{
		if(listp == &DevSpiRoot)
			break;

		p = list_entry(listp, DevSpiNode, list);

		if(strcmp(dev->name, p->dev.name) == 0)
		{
			wjq_log(LOG_INFO, "spi dev name err!\r\n");
			return -1;
		}
		
		listp = listp->next;
	}

	/* 
		申请一个节点空间 
		
	*/
	p = (DevSpiNode *)wjq_malloc(sizeof(DevSpiNode));
	list_add(&(p->list), &DevSpiRoot);

	memcpy((u8 *)&p->dev, (u8 *)dev, sizeof(DevSpi));
	

	/*初始化*/
	if(dev->type == DEV_SPI_V)
		mcu_vspi_init(dev);
	else if(dev->type == DEV_SPI_H)
		mcu_hspi_init(dev);
	
	p->gd = -1;
	
	return 0;
}

struct list_head DevSpiChRoot = {&DevSpiChRoot, &DevSpiChRoot};
/**
 *@brief:      mcu_spich_register
 *@details:    注册SPI通道
 *@param[in]   DevSpiCh *dev     
 *@param[out]  无
 *@retval:     
 */
s32 mcu_spich_register(const DevSpiCh *dev)
{
	struct list_head *listp;
	DevSpiChNode *p;
	DevSpiNode *p_spi;
	
	wjq_log(LOG_INFO, "[register] spi ch :%s!\r\n", dev->name);

	/*
		先要查询当前，防止重名
	*/
	listp = DevSpiChRoot.next;
	while(1)
	{
		if(listp == &DevSpiChRoot)
			break;

		p = list_entry(listp, DevSpiChNode, list);
		
		if(strcmp(dev->name, p->dev.name) == 0)
		{
			wjq_log(LOG_INFO, ">--------------------spi ch dev name err!\r\n");
			return -1;
		}
		
		listp = listp->next;
	}

	/* 寻找SPI控制器*/
	listp = DevSpiRoot.next;
	while(1)
	{
		if(listp == &DevSpiRoot)
		{
			wjq_log(LOG_INFO, ">---------------------spi ch reg err:no spi!\r\n");
			return -1;
		}


		p_spi = list_entry(listp, DevSpiNode, list);

		if(strcmp(dev->spi, p_spi->dev.name) == 0)
		{
			//wjq_log(LOG_INFO, "spi ch find spi!\r\n");
			break;
		}
		
		listp = listp->next;
	}
	/* 
		申请一个节点空间 
		
	*/
	p = (DevSpiChNode *)wjq_malloc(sizeof(DevSpiChNode));
	list_add(&(p->list), &DevSpiChRoot);
	memcpy((u8 *)&p->dev, (u8 *)dev, sizeof(DevSpiCh));
	p->gd = -1;
	p->spi = p_spi;

	/* 初始化管脚 */
	mcu_io_config_out(dev->csport,dev->cspin);
	mcu_io_output_setbit(dev->csport,dev->cspin);

	return 0;
}


/**
 *@brief:      mcu_spi_open
 *@details:    打开SPI通道
 *@param[in]   DevSpiChNode * node
               u8 mode      模式
               u16 pre      预分频
 *@param[out]  无
 *@retval:     
 			   打开一次SPI，在F407上大概要2us
 */
DevSpiChNode *mcu_spi_open(char *name, SPI_MODE mode, u16 pre)
{

	s32 res;
	DevSpiChNode *node;
	struct list_head *listp;
	
	//BUSSPI_DEBUG(LOG_INFO, "spi ch open:%s!\r\n", name);

	listp = DevSpiChRoot.next;
	node = NULL;
	
	while(1)
	{
		if(listp == &DevSpiChRoot)
			break;

		node = list_entry(listp, DevSpiChNode, list);
		//BUSSPI_DEBUG(LOG_INFO, "spi ch name%s!\r\n", node->dev.name);
		
		if(strcmp(name, node->dev.name) == 0)
		{
			//BUSSPI_DEBUG(LOG_INFO, "spi ch dev get ok!\r\n");
			break;
		}
		else
		{
			node = NULL;
		}
		
		listp = listp->next;
	}

	if(node != NULL)
	{
		
		if(node->gd == 0)
		{
			//BUSSPI_DEBUG(LOG_INFO, "spi ch open err:using!\r\n");
			node = NULL;
		}
		else
		{
			/*打开SPI控制器*/
			if(node->spi->dev.type == DEV_SPI_H)
			{
				res = mcu_hspi_open(node->spi, mode, pre);	
			}
			else if(node->spi->dev.type == DEV_SPI_V)
			{
				res = mcu_vspi_open(node->spi, mode, pre);	
			}

			if(res == 0)
			{
				node->gd = 0;
				//BUSSPI_DEBUG(LOG_INFO, "spi dev open ok: %s!\r\n", name);
				mcu_io_output_resetbit(node->dev.csport, node->dev.cspin);
			}
			else
			{
				//BUSSPI_DEBUG(LOG_INFO, "spi dev open err!\r\n");
				node = NULL;
			}
		}
	}
	else
	{
		BUSSPI_DEBUG(LOG_INFO, ">-------spi ch no exist!\r\n");	
	}
	
	return node;
}


/**
 *@brief:      mcu_spi_close
 *@details:    关闭SPI 通道
 *@param[in]   DevSpiChNode * node  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_spi_close(DevSpiChNode * node)
{
	if(node->spi->dev.type == DEV_SPI_H)
	{
		mcu_hspi_close(node->spi);
	}
	else
		mcu_vspi_close(node->spi);
	
	/*拉高CS*/
	mcu_io_output_setbit(node->dev.csport, node->dev.cspin);
	node->gd = -1;
 
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
s32 mcu_spi_transfer(DevSpiChNode * node, u8 *snd, u8 *rsv, s32 len)
{
	if(node == NULL)
		return -1;

	if(node->spi->dev.type == DEV_SPI_H)
		return mcu_hspi_transfer(node->spi, snd, rsv, len);
	else if(node->spi->dev.type == DEV_SPI_V)	
		return mcu_vspi_transfer(node->spi, snd, rsv, len);
	else
	{
		BUSSPI_DEBUG(LOG_DEBUG, "spi dev type err\r\n");	
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
s32 mcu_spi_cs(DevSpiChNode * node, u8 sta)
{
	switch(sta)
	{
		case 1:
			mcu_io_output_setbit(node->dev.csport, node->dev.cspin);
			break;
			
		case 0:
			mcu_io_output_resetbit(node->dev.csport, node->dev.cspin);
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
	u8 src[16];
	u8 rsv[16];
	
	/*打开SPI通道*/
	spichnode = mcu_spi_open("VSPI1_CH1", SPI_MODE_1, 4);
	if(spichnode == NULL)
	{
		while(1);
	}
	/*读10个数据*/
	mcu_spi_transfer(spichnode, NULL, rsv, 10);
	/*写10个数据*/
	mcu_spi_transfer(spichnode, src, NULL, 10);
	/*读写10个数据*/
	mcu_spi_transfer(spichnode, src, rsv, 10);

	mcu_spi_close(spichnode);
}

#endif

