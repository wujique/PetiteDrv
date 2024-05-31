/**
 * @file            mcu_i2c.c
 * @brief           IO模拟I2C
 * @author          test
 * @date            2019年03月30日 星期一
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:      
 *   作    者:      屋脊雀工作室
 *   修改内容:      创建文件
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
#include "log.h"
#include "mem/p_list.h"
#include "bus/bus_i2c.h"
#include "bus/bus_vi2c.h"
#include "mem/p_malloc.h"
#include "petite_dev.h"


#define BUS_I2C_DEBUG

#ifdef BUS_I2C_DEBUG
#define BUSI2C_DEBUG(l, args...) petite_log(l, "BUS I2C", NULL,__FUNCTION__, __LINE__, ##args); 
#else
#define BUSI2C_DEBUG(a, ...)
#endif

/**
 *@brief:      bus_i2c_register
 *@details:    初始化I2C接口， 相当于注册一个I2C设备
 *@param[in]   const DevI2c * dev I2C设备信息
 *@param[out]  无
 *@retval:     
 */
PDevNode *bus_i2c_register(const DevI2c * dev)
{
    
	struct list_head *listp;
	DevI2cNode *pnode;
	
	BUSI2C_DEBUG(LOG_DEBUG, "i2c:%s!\r\n", dev->pdev.name);


	/* 申请一个节点空间   并插入链表     	*/
	pnode = (DevI2cNode *) wjq_malloc(sizeof(DevI2cNode));
	
	petite_dev_init_node((PDevNode *)pnode, (PetiteDev *) dev);

	/*初始化互斥 */
	pnode->gd = -1;
	pnode->mutex = osMutexNew(NULL);
	if (pnode->mutex == NULL) {
		BUSI2C_DEBUG(LOG_ERR, "mutex new err!\r\n");
	}
	
	if (dev->pdev.type == BUS_I2C_V)
		bus_vi2c_init(dev);
	else if (dev->pdev.type == BUS_I2C_H)
		return 0;//mcu_hi2c_init(&p->dev);
	
	return (PDevNode *)pnode;
}
/**
 *@brief:      bus_i2c_open
 *@details:    根据名字打开一个i2c接口
 *@param[in]   name 设备名称
 			   wait 等待时间，单位ms, 0xffffffff 永远
 			   clk 时钟频率， 单位kHZ
 *@param[out]  无
 *@retval:     返回设备节点
 */
DevI2cNode *bus_i2c_open (char *name, uint32_t wait, uint16_t clk)
{

	DevI2cNode *node;

	osStatus_t res;
	
	//BUSI2C_DEBUG(LOG_INFO, "i2c open:%s!\r\n", name);
	node = (DevI2cNode *)petite_dev_get_node(name);
		
	if (node != NULL) {
		/* 判断互斥是否可用          osWaitForever*/

		//BUSI2C_DEBUG(LOG_INFO, "i2c mutex %d ", wait);
		res = osMutexAcquire(node->mutex, wait);

		if ( osOK != res) {
			BUSI2C_DEBUG(LOG_ERR, "fail ");
			node = NULL;
		} else {
			node->clkkhz = 	clk;
		}	
	}
	//BUSI2C_DEBUG(LOG_INFO, "open suc\r\n ");
	return node;
}
/**
 *@brief:      mcu_i2c_close
 *@details:    关闭I2C节点
 *@param[in]   DevI2cNode *node 
 *@param[out]  无
 *@retval:     -1 关闭失败；0 关闭成功
 */
s32 bus_i2c_close(DevI2cNode *node)
{
	if  (node == NULL) return -1;

	//BUSI2C_DEBUG(LOG_INFO, "i2c close:%s!\r\n", node->dev.pnode.name);
	osMutexRelease(node->mutex);

	return 0;
}

/**
 *@brief:      mcu_i2c_transfer
 *@details:    中间无重新开始位的传输流程
 *@param[in]   DevI2cNode * node  I2C节点
 			   u8 addr   7位地址
               u8 rw    0 写，1 读    
               u8* data  
               s32 datalen 发送数据长度
 *@param[out]  无
 *@retval:     
 */
s32 bus_i2c_transfer(DevI2cNode *node, u8 addr, u8 rw, u8* data, s32 datalen)
{
	PetiteDevType type;
	
	if (node == NULL) return -1;

	type = node->pnode.pdev->type;
	
	if( type== BUS_I2C_H)
		return 0;//mcu_hi2c_transfer (node, addr, rw, data, datalen);
	else if (type == BUS_I2C_V)	
		return bus_vi2c_transfer(node, addr, rw, data, datalen);
	else {
		wjq_log(LOG_ERR, "i2c dev type err\r\n");	
	}	
	return -1;
}

/**
 * @brief   带寄存器地址的I2C读写
 * 
 * @param   node        I2C设备节点
 * @param   addr        设备地址
 * @param   reg         寄存器地址数据
 * @param   reglen      寄存器地址长度，字节
 * @param   rw          读or写
 * @param   data        数据地址
 * @param   datalen     数据长度
 * @return  s32 
 * 
 */
s32 bus_vi2c_transfer_reg(DevI2cNode *node, u8 addr, u8* reg, u8 reglen, u8 rw, u8* data, s32 datalen)
{
	PetiteDevType type;
	int res;
	DevI2c *dev;
	PDevNode *pnode;

	if (node == NULL) return -1;
	pnode = (PDevNode *)node;

	dev = (DevI2c *)pnode->pdev;

	type = node->pnode.pdev->type;
	
	if( type== BUS_I2C_H)
		return 0;//mcu_hi2c_transfer (node, addr, rw, data, datalen);
	else if (type == BUS_I2C_V)	{
		res = mcu_vi2c_transfer_reg(dev, addr, reg, reglen, rw, data, datalen);
		return res;
	} else {
		wjq_log(LOG_ERR, "i2c dev type err\r\n");	
	}	
	return -1;
}


#if 0

void mcu_i2c_example(void)
{
	DevI2cNode *node;
	
	node = mcu_i2c_open("VI2C1");
	if(node == NULL)
	{
		wjq_log(LOG_DEBUG, "open VI2C1 err!\r\n");
		while(1);
	}
	
	u8 data[16];
	mcu_i2c_transfer(node, 0x70, MCU_I2C_MODE_W, data, 8);
	mcu_i2c_transfer(node, 0x70, MCU_I2C_MODE_R, data, 8);

	mcu_i2c_close(node);
}

#endif

