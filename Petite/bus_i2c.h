#ifndef __BUS_I2C_H_
#define __BUS_I2C_H_

#include "p_list.h"
#include "petie_def.h"

typedef enum{
	DEV_I2C_H = 1,//硬件SPI控制器
	DEV_I2C_V = 2,//IO模拟SPI
}DEV_I2C_TYPE;

/*
	i2c设备定义
*/

typedef struct
{
	/*设备名称*/
	char name[DEV_NAME_SIZE];

	/*设备类型，IO模拟 or 硬件控制器*/
	DEV_I2C_TYPE type;

	/*设备需要的资源，模拟I2C只需要两根IO口*/
	MCU_PORT sclport;
	u16 sclpin;

	MCU_PORT sdaport;
	u16 sdapin;
}DevI2c;

/*
	

*/
typedef struct
{
	s32 gd;
	DevI2c dev;	
	struct list_head list;
}DevI2cNode;


#define MCU_I2C_MODE_W 0
#define MCU_I2C_MODE_R 1

extern s32 bus_i2c_register(const DevI2c * dev);
extern DevI2cNode *bus_i2c_open(char *name);
extern s32 bus_i2c_close(DevI2cNode *node);
extern s32 bus_i2c_transfer(DevI2cNode *node, u8 addr, u8 rw, u8* data, s32 datalen);

#endif

