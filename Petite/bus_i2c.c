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
#include "p_list.h"
#include "bus_i2c.h"
#include "bus_vi2c.h"
#include "alloc.h"

/*	I2C模块维护一个链表，记录有当前初始化的I2C控制器*/
struct list_head DevI2cGdRoot = {&DevI2cGdRoot, &DevI2cGdRoot};

/**
 *@brief:      bus_i2c_register
 *@details:    初始化I2C接口， 相当于注册一个I2C设备
 *@param[in]   const DevI2c * dev I2C设备信息
 *@param[out]  无
 *@retval:     
 */
s32 bus_i2c_register(const DevI2c * dev)
{
    
	struct list_head *listp;
	DevI2cNode *p;
	
	wjq_log(LOG_INFO, "[register] i2c:%s!\r\n", dev->pnode.name);

	/*	先要查询当前I2C控制器，防止重名	*/
	listp = DevI2cGdRoot.next;
	while(1) {
		if (listp == &DevI2cGdRoot) break;

		p = list_entry(listp, DevI2cNode, list);
		//wjq_log(LOG_INFO, "i2c dev name:%s!\r\n", p->dev.name);
		
		if (strcmp(dev->pnode.name, p->dev.pnode.name) == 0) {
			wjq_log(LOG_INFO, "i2c dev name err!\r\n");
			return -1;
		}
		
		listp = listp->next;
	}

	/* 申请一个节点空间   并插入链表     	*/
	p = (DevI2cNode *) wjq_malloc(sizeof(DevI2cNode));
	list_add(&(p->list), &DevI2cGdRoot);
	
	/*	初始化设备节点 	*/
	memcpy((u8 *)&p->dev, (u8 *)dev, sizeof(DevI2c));
	p->gd = -1;

	if (dev->pnode.type == BUS_I2C_V)
		bus_vi2c_init(&p->dev);
	else if (dev->pnode.type == BUS_I2C_H)
		mcu_hi2c_init(&p->dev);
	
	return 0;
}
/**
 *@brief:      bus_i2c_open
 *@details:    根据名字打开一个i2c接口
 *@param[in]   void  
 *@param[out]  无
 *@retval:     返回设备节点
 */
DevI2cNode *bus_i2c_open(char *name)
{

	DevI2cNode *node;
	struct list_head *listp;
	
	//I2C_DEBUG(LOG_INFO, "i2c open:%s!\r\n", name);

	listp = DevI2cGdRoot.next;
	node = NULL;
	
	while(1) {
		if (listp == &DevI2cGdRoot) break;

		node = list_entry(listp, DevI2cNode, list);
		//I2C_DEBUG(LOG_INFO, "i2c dev name:%s!\r\n", node->dev.name);
		 
		if (strcmp(name, node->dev.pnode.name) == 0) {
			//I2C_DEBUG(LOG_INFO, "i2c dev open ok!\r\n");
			break;
		}
		
		listp = listp->next;
	}

	if (node != NULL) {
		if (node->gd == 0) {
			node = NULL;
		} else {
			node->gd = 0;
		}
	}
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

	if (node->gd != 0) return -1;

	node->gd = -1; 

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
	if (node == NULL) return -1;

	if(node->dev.pnode.type == BUS_I2C_H)
		return mcu_hi2c_transfer (node, addr, rw, data, datalen);
	else if (node->dev.pnode.type == BUS_I2C_V)	
		return bus_vi2c_transfer(node, addr, rw, data, datalen);
	else {
		wjq_log(LOG_DEBUG, "i2c dev type err\r\n");	
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

