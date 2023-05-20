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
#include "bus/bus_spi.h"
#include "bus/bus_vspi.h"

#define BUS_VSPI_DEBUG

#ifdef BUS_VSPI_DEBUG
#define VSPI_DEBUG	wjq_log 
#else
#define VSPI_DEBUG(a, ...)
#endif

extern uint32_t McuDelayUs;


/**
 *@brief:      mcu_vspi_init
 *@details:    初始化虚拟SPI
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 bus_vspi_init(const DevSpi *dev)
{

	//wjq_log(LOG_DEBUG, "vspi init:%s\r\n", dev->pnode.name);

	mcu_io_config_out(dev->clkport, dev->clkpin);
	mcu_io_output_setbit(dev->clkport,dev->clkpin);

	mcu_io_config_out(dev->mosiport, dev->mosipin);
	mcu_io_output_setbit(dev->mosiport, dev->mosipin);


	mcu_io_config_in(dev->misoport, dev->misopin);
	mcu_io_output_setbit(dev->misoport, dev->misopin);
	
	return 0;
}


/**
 *@brief:      vspi_delay
 *@details:    虚拟SPI时钟延时
 *@param[in]   u32 delay  
 *@param[out]  无
 *@retval:     
 */
void vspi_delay(u32 delay)
{
	volatile u32 i=delay;

	while(i>0) {
		i--;	
	}

}

u32 VspiDelay = 0;

/**
 *@brief:      mcu_vspi_open
 *@details:    打开虚拟SPI
 *@param[in]   SPI_DEV dev    
               SPI_MODE mode  
               u16 pre        
 *@param[out]  无
 *@retval:     
 */
s32 bus_vspi_open(DevSpiNode *node, SPI_MODE mode, u16 KHz)
{
		
    return 0;
}
/**
 *@brief:      mcu_vspi_close
 *@details:    关闭虚拟SPI
 *@param[in]   SPI_DEV dev  
 *@param[out]  无
 *@retval:     
 */
s32 bus_vspi_close(DevSpiNode *node)
{

    return 0;
}
/**
 *@brief:      mcu_vspi_transfer
 *@details:       虚拟SPI通信
 *@param[in]   SPI_DEV dev  
               u8 *snd      
               u8 *rsv      
               s32 len      
 *@param[out]  无
 *@retval:     

 		node->clk = 0, CLK时钟1.5M 2018.06.02
 */
s32 bus_vspi_transfer(DevSpiNode *node, u8 *snd, u8 *rsv, s32 len)
{
	u8 i;
	u8 data;
	s32 slen;
	u8 misosta;

	volatile u16 delay;
	
	DevSpi *dev;
	PDevNode *pnode;

	pnode = (PDevNode *)node;
	dev = (DevSpi *)pnode->pdev;
	
	if (node == NULL) {
		VSPI_DEBUG(LOG_DEBUG, "vspi dev err\r\n");
		return -1;
	}

    if ( ((snd == NULL) && (rsv == NULL)) || (len < 0) ) {
        return -1;
    }
	
	slen = 0;

	while(1) {
		if (slen >= len) break;

		if (snd == NULL)
			data = 0xff;
		else
			data = *(snd+slen);
		
		for(i=0; i<8; i++) {
			mcu_io_output_resetbit(dev->clkport, dev->clkpin);

			delay = node->clk;
			while(delay>0) {
				delay--;	
			}
			
			if(data&0x80)
				mcu_io_output_setbit(dev->mosiport, dev->mosipin);
			else
				mcu_io_output_resetbit(dev->mosiport, dev->mosipin);
			
			delay = node->clk;
			while(delay>0) {
				delay--;	
			}
			
			data<<=1;
			mcu_io_output_setbit(dev->clkport, dev->clkpin);
			
			delay = node->clk;
			while(delay>0) {
				delay--;	
			}
			
			misosta = mcu_io_input_readbit(dev->misoport, dev->misopin);
			if(misosta == MCU_IO_STA_1) {
				data |=0x01;
			} else {
				data &=0xfe;
			}
			
			delay = node->clk;
			while(delay>0) {
				delay--;	
			}
			
		}

		if(rsv != NULL) *(rsv+slen) = data;
		
		slen++;
	}

	return slen;
}



