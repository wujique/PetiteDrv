

#include "mcu.h"
#include "petite_config.h"
#include "petite.h"
#include "board_sysconf.h"
#include "drv_lcd.h"
#include "drv_spiflash.h"

/*
	所有设备挂在同一个链表，
	单片机设备不可能很多
	*/
struct list_head PetiteDevRoot = {&PetiteDevRoot, &PetiteDevRoot};


/*
	根据名字，获取设备节点，
	返回设备节点指针， NULL  说明没找到
	*/
PDevNode *petite_dev_get_node(char *name)
{
	struct list_head *listp;
	PDevNode *node;
	
	listp = PetiteDevRoot.next;
	while(1) {
		if (listp == &PetiteDevRoot) {
			LogPetite(LOG_INFO, "pnode cant find:%s!\r\n", name);	
			break;
		}
		node = list_entry(listp, PDevNode, list);
		
		if (strcmp(name, node->pdev->name) == 0) {
			//LogPetite(LOG_INFO, "pnode find:%s!\r\n", name);
			return node;
		}
		
		listp = listp->next;
	}

	return NULL;
}

int petite_dev_init_node(PDevNode *pnode, PetiteDev *pdev)
{

	if (pnode != NULL) {
		/* 把设备信息挂到设备节点 */
		pnode->pdev = pdev;
		/* 把节点加入链表 */
		list_add(&(pnode->list), &PetiteDevRoot);
	
		if (pdev->basebus != NULL){
			PDevNode *basenode = petite_dev_get_node(pdev->basebus);
			if (basenode == NULL) {
				LogPetite(LOG_WAR, "base bus not exist!\r\n");
				pnode->basenode = NULL;
			} else {
				/* 记录依赖节点，以便后续open时不再需要通过名字寻找 */
				pnode->basenode = basenode;
			}

		}
		
	}
	return 0;
}

/*
	系统设备注册
	通过缩进区分层级和依赖关系。
	后续可以考虑实现可见字符表示的设备树
*/
s32 petite_dev_register(void *DevTable[])
{
	uint8_t i=0;
	PetiteDev *pdev;
	
	while(1){
		if (DevTable[i] == NULL) break;
		pdev =	(PetiteDev *)DevTable[i];
		LogPetite(LOG_DEBUG, "register:%s\r\n", pdev->name);

		switch(pdev->type)
		{
			case BUS_I2C_V:
				bus_i2c_register((const DevI2c *)DevTable[i]);
				break;

			case BUS_SPI_V:
			case BUS_SPI_H:
				bus_spi_register((const DevSpi *)DevTable[i]);
				break;
			
			case BUS_SPI_CH:
				bus_spich_register((const DevSpiCh *)DevTable[i]);
				break;

			case DEV_LCD:
				lcd_dev_register((const DevLcd *)DevTable[i]);
				break;
			
			case DEV_SPIFLASH:
				dev_spiflash_register((const DevSpiFlash *)DevTable[i]);
				break;
			
			default:
				LogPetite(LOG_WAR, "register %s not register!\r\n\r\n", pdev->name);
				break;
		}

		
		i++;
	}
	
	LogPetite(LOG_DEBUG, "register: %d device!\r\n\r\n", i);	
	
	return 0;
}


