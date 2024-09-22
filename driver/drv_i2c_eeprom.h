#ifndef _DRV_I2C_EEPROM_H_
#define _DRV_I2C_EEPROM_H_


/*设备规格 */
typedef struct
{
	char *name;
	u8 addr;///7bit地址
	u8 pagesize;///
	u16 structure;///总容量 Byte
}I2CEepromPra;


/*设备定义*/
typedef struct
{
	PetiteDev pdev;
	I2CEepromPra pra;//设备规格
}DevI2cEeprom;


/* 设备节点 */
typedef struct
{
	PDevNode pnode;
	
	s32 gd;

	DevI2cNode *basenode;
	const DevI2cEeprom *dev;	
}I2cEepromNode;


extern void mc24c_test(char *I2cName, DevI2c *dev);


#endif



