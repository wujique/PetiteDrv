/**
 * @file            dev_lcdbus.c
 * @brief           对各种LCD接口封装
 * @author          wujique
 * @date            2018年4月18日 星期三
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年4月18日 星期三
 *   作    者:         wujique
 *   修改内容:   创建文件
	1 源码归屋脊雀工作室所有。
	2 可以用于的其他商业用途（配套开发板销售除外），不须授权。
	3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
	4 可随意修改源码并分发，但不可直接销售本代码获利，并且请保留WUJIQUE版权说明。
	5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
	6 使用本源码则相当于认同本版权说明。
	7 如侵犯你的权利，请联系：code@wujique.com
	8 一切解释权归屋脊雀工作室所有。
*/
#include <stdarg.h>
#include <stdio.h>

#include "mcu.h"
#include "petite.h"
#include "board_sysconf.h"

#include "drv_lcd.h"

#if (PETITE_BUS_LCD_8080 == 1)
extern volatile u16 *LcdReg;
extern volatile u16 *LcdData;
#endif
	
/**
 *@brief:      bus_lcd_IO_init
 *@details:    初始化lcd接口IO
 *@param[in]   const DevLcdBus *dev  
 *@param[out]  无
 *@retval:     
 */
void bus_lcd_IO_init(const DevLcdBus *bus) 
{

	if (bus->pnode.type == BUS_LCD_I2C) return;

	/* 初始化管脚 */
	if (bus->A0port != NULL) {
		mcu_io_config_out(bus->A0port,bus->A0pin);
		mcu_io_output_setbit(bus->A0port,bus->A0pin);
	}
	
	if (bus->rstport != NULL) {
		mcu_io_config_out(bus->rstport,bus->rstpin);
		mcu_io_output_setbit(bus->rstport,bus->rstpin);
	}
	
	if (bus->blport != NULL) {
		mcu_io_config_out(bus->blport,bus->blpin);
		mcu_io_output_setbit(bus->blport,bus->blpin);
	}
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 bus_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	DevLcdBus const*bus;
	bus = lcd->dev.bus;
	
	if (sta ==MCU_IO_STA_1) {
		mcu_io_output_setbit(bus->blport, bus->blpin);
	} else {
		mcu_io_output_resetbit(bus->blport, bus->blpin);	
	}
	return 0;
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 bus_lcd_rst(DevLcdNode *lcd, u8 sta)
{
	DevLcdBus const*bus;
	bus = lcd->dev.bus;
	
	if (sta ==MCU_IO_STA_1) {
		mcu_io_output_setbit(bus->rstport, bus->rstpin);
	} else {
		mcu_io_output_resetbit(bus->rstport, bus->rstpin);	
	}
	return 0;
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
DevLcdNode *bus_lcd_open(DevLcdNode *lcd)
{
	PetiteDevType type;
	char *busname;
	uint16_t bus_clk;
	I2CPra *i2c_pra;
	PraSpiSet *spi_pra;
	SPI_MODE spi_mode;
	
	type = lcd->dev.bus->pnode.type;
	busname = (char *)lcd->dev.bus->basebus;
	
	if (type == BUS_LCD_SPI) {
		spi_pra = (PraSpiSet *)lcd->dev.buspra;
		bus_clk = spi_pra->KHz;
		spi_mode = spi_pra->mode;
		
		lcd->basenode = (void *)bus_spich_open(busname, spi_mode, bus_clk, 0xffffffff);

	} else if (type == BUS_LCD_I2C) {
		/*todo I2C频率配置 */
		i2c_pra = (I2CPra *)lcd->dev.buspra;
		bus_clk = i2c_pra->fkhz;
		
		lcd->basenode = bus_i2c_open(busname, 0xffffffff, bus_clk);
	} else if (type == BUS_LCD_8080) {
		#if (PETITE_BUS_LCD_8080 == 1)
		/*8080特殊处理*/
		lcd->basenode = (void *)1;
		#else
		wjq_log(LOG_INFO, "lcd bus unsuport 8080 !\r\n");
		#endif
	}
	
	if (lcd->basenode == NULL) {
		wjq_log(LOG_INFO, "lcd bus open base bus err!\r\n");	

	} else {

	}

	return lcd;
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */

s32 bus_lcd_close(DevLcdNode *lcd)
{
	PetiteDevType type;
	type = lcd->dev.bus->pnode.type;

	if(type == BUS_LCD_SPI){
		bus_spich_close((DevSpiChNode *)lcd->basenode);
	}else if(type == BUS_LCD_I2C){
		bus_i2c_close((DevI2cNode *)lcd->basenode);	
	}else if(type == BUS_LCD_8080){	
		#if (PETITE_BUS_LCD_8080 == 1)
		/*8080特殊处理*/
		lcd->basenode = NULL;
		#else
		wjq_log(LOG_INFO, "lcd bus unsuport 8080 !\r\n");
		#endif
	}
	
	return 0;
}


/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 bus_lcd_write_data(DevLcdNode *lcd, u8 *data, u32 len)
{
	/*直接定义256字节，可能有BUG，要根据len动态申请*/
	u8 tmp[256];
	u32 i;
	
	u8 i2c_addr;
	I2CPra *i2c_pra;
	
	PetiteDevType type;

	type = lcd->dev.bus->pnode.type;

	switch(type)
	{
		case BUS_LCD_SPI:
		{
			bus_spich_transfer((DevSpiChNode *)lcd->basenode,  data, NULL, len);
		}
		break;
		case BUS_LCD_I2C:
		{
			tmp[0] = 0x40;
			memcpy(&tmp[1], data, len);
			
			i2c_pra = (I2CPra *)lcd->dev.buspra;
			i2c_addr = i2c_pra->addr;
			
			bus_i2c_transfer((DevI2cNode *)lcd->basenode, i2c_addr, MCU_I2C_MODE_W, tmp, len+1);
		}
		break;
		
		case BUS_LCD_8080:			
		{
			#if (PETITE_BUS_LCD_8080 == 1)
			/*8080特殊处理*/
			u16 *p;
			p = (u16 *)data;
			for(i=0; i<len; i++)
			{
				*LcdData = *(p+i);	
			}
			#else
			wjq_log(LOG_INFO, "lcd bus unsuport 8080 !\r\n");
			#endif
			}
		break;
		default:
			break;
	}
	
	return 0;
}


s32 bus_lcd_w_data(DevLcdNode *lcd, u16 color, u32 len)
{

	u32 i;
	PetiteDevType type;
	type = lcd->dev.bus->pnode.type;

	switch(type)
	{

		case BUS_LCD_8080:			
		{
			#if (PETITE_BUS_LCD_8080 == 1)
			/*8080特殊处理*/
			for(i=len; i>0; i--) {
				*LcdData = color;	
			}
			#else
			wjq_log(LOG_INFO, "lcd bus unsuport 8080 !\r\n");
			#endif
			
		}
		break;
		default:
			break;
	}
	
	return 0;
}

/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 bus_lcd_read_data(DevLcdNode *lcd, u8 *data, u32 len)
{
	u32 i;
	PetiteDevType type;
	type = lcd->dev.bus->pnode.type;
	
	switch(type)
	{
		case BUS_LCD_SPI:
		{	
			bus_spich_transfer((DevSpiChNode *)lcd->basenode,  NULL, data, len);
		}
		break;
		
		case BUS_LCD_I2C:
		{	

		}
		break;
		
		case BUS_LCD_8080:
		{
			
			#if (PETITE_BUS_LCD_8080 == 1)
			u16 *p;
			p = (u16 *)data;
			
			for(i=0; i<len; i++){
				*(p+i) = *LcdData;	
			}
			#else
			wjq_log(LOG_INFO, "lcd bus unsuport 8080 !\r\n");
			#endif
		}
		break;
		default:
			break;
	}
	return 0;	
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
s32 bus_lcd_write_cmd(DevLcdNode *lcd, u16 cmd)
{
	u8 tmp[2];
	
	u8 i2c_addr;
	I2CPra *i2c_pra;
	
	PetiteDevType type;
	DevLcdBus const*bus;
	
	type = lcd->dev.bus->pnode.type;
	bus = lcd->dev.bus;
	switch(type)
	{
		case BUS_LCD_SPI:
		{	
			mcu_io_output_resetbit(bus->A0port, bus->A0pin);
			tmp[0] = cmd;
			bus_spich_transfer((DevSpiChNode *)lcd->basenode,  &tmp[0], NULL, 1);
			mcu_io_output_setbit(bus->A0port, bus->A0pin);
		}
		break;
		
		case BUS_LCD_I2C:
		{	
			tmp[0] = 0x00;
			tmp[1] = cmd;
			
			i2c_pra = (I2CPra *)lcd->dev.buspra;
			i2c_addr = i2c_pra->addr;
			
			bus_i2c_transfer((DevI2cNode *)lcd->basenode, i2c_addr, MCU_I2C_MODE_W, tmp, 2);
		}
		break;
		
		case BUS_LCD_8080:
		{
			#if (PETITE_BUS_LCD_8080 == 1)
			*LcdReg = cmd;
			#else
			wjq_log(LOG_INFO, "lcd bus unsuport 8080 !\r\n");
			#endif
		}
		break;
		default:
			break;
	}
	return 0;
}





