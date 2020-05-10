/**
 * @file            mcu_i2c.c
 * @brief           
 * @author          test
 * @date            2017年10月30日 星期一
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:      2017年10月30日 星期一
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
#include "bus_i2c.h"

#define MCU_I2C_DEBUG

#ifdef MCU_I2C_DEBUG
#define I2C_DEBUG	wjq_log 
#else
#define I2C_DEBUG(a, ...)
#endif


#define SCCB_TIMEOUT_MAX    10000
	
void SCCB_GPIO_Config(I2C_TypeDef* I2CDef)
{
	


}
	
uint8_t bus_sccb_writereg(I2C_TypeDef* I2CDef, uint8_t DeviceAddr, uint16_t Addr, uint8_t Data)
{
	
	
	
	/* If operation is OK, return 0 */
	return 0;

}
	
uint8_t bus_sccb_readreg(I2C_TypeDef* I2CDef, uint8_t DeviceAddr, uint16_t Addr)
{
 
  return 0;
}

s32 mcu_hi2c_init(DevI2c * dev)
{

	
	return 0;
}


s32 mcu_hi2c_transfer(DevI2cNode *node, u8 addr, u8 rw, u8* data, s32 datalen)
{
	if(rw == MCU_I2C_MODE_W)//写
    {

    }
    else if(rw == MCU_I2C_MODE_R)//读
    {
		
    }
	return -1;
}

