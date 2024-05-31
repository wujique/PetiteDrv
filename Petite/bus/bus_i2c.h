#ifndef __BUS_I2C_H_
#define __BUS_I2C_H_

#include "mcu.h"
#include "petite_dev.h"
#include "cmsis_os.h"


/*	i2c设备定义 */
typedef struct
{
	PetiteDev pdev;
/*-----------------------------*/
	/*设备需要的资源，模拟I2C只需要两根IO口*/
	MCU_PORT sclport;
	MCU_IO sclpin;

	MCU_PORT sdaport;
	MCU_IO sdapin;
}DevI2c;

/*  设备节点 */
typedef struct
{
	PDevNode pnode;
	/*-----------------------------*/
	s32 gd;
	osMutexId_t mutex;	

	uint16_t clkkhz;//时钟频率
	
}DevI2cNode;

typedef struct{
	uint8_t addr;
	uint16_t fkhz;
}I2CPra;

extern PDevNode * bus_i2c_register(const DevI2c * dev);
extern DevI2cNode *bus_i2c_open(char *name, uint32_t wait, uint16_t clk);
extern s32 bus_i2c_close(DevI2cNode *node);
extern s32 bus_i2c_transfer(DevI2cNode *node, u8 addr, u8 rw, u8* data, s32 datalen);

#endif

