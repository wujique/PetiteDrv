#ifndef _DRV_I2C_EEPROM_H_
#define _DRV_I2C_EEPROM_H_


/*设备规格 */
typedef struct
{
	char *name;
	u8 addr;///7bit地址
	u8 pagesize;///
	u16 structure;///总容量 Byte
}_strI2CEeprom;


/*设备定义*/
typedef struct
{
	PetiteDev pdev;
	const _strI2CEeprom *pra;//设备规格
}DevI2cEeprom;


/* 设备节点 */
typedef struct
{
	s32 gd;
	
	/* 设备规格， 设备定义也定义了一个设备规格。
		注册设备时会进行拷贝，如有需要，会根据实际情况进行修改 */
	_strI2CEeprom *pra;
	/*spi 通道节点*/
	DevSpiChNode *spichnode;

	/*设备定义 */
	const DevI2cEeprom *dev;
	
	struct list_head list;
}DevI2cEepromNode;


#endif



