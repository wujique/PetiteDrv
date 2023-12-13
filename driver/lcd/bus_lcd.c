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
#include "board_sysconf.h"

#include "drv_lcd.h"



/**
 *@brief:      bus_lcd_IO_init
 *@details:    初始化lcd接口IO
 *@param[in]   const DevLcdBus *dev  
 *@param[out]  无
 *@retval:     
 */
void bus_lcd_IO_init(const DevLcdCtrlIO *CtrlIO) 
{
	/* 初始化管脚 */
	if (CtrlIO->A0port != NULL) {
		mcu_io_config_out(CtrlIO->A0port, CtrlIO->A0pin);
		mcu_io_output_setbit(CtrlIO->A0port, CtrlIO->A0pin);
	}
	
	if (CtrlIO->rstport != NULL) {
		mcu_io_config_out(CtrlIO->rstport, CtrlIO->rstpin);
		mcu_io_output_setbit(CtrlIO->rstport, CtrlIO->rstpin);
	}
	
	if (CtrlIO->blport != NULL) {
		mcu_io_config_out(CtrlIO->blport, CtrlIO->blpin);
		mcu_io_output_setbit(CtrlIO->blport, CtrlIO->blpin);
	}
}
/**
 *@brief:     
 *@details:   
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
static s32 bus_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	const DevLcdCtrlIO *CtrlIO;
	DevLcd *dev;

	dev = (DevLcd *)lcd->pnode.pdev;
	CtrlIO = dev->ctrlio;
	
	if (sta ==MCU_IO_STA_1) {
		mcu_io_output_setbit(CtrlIO->blport, CtrlIO->blpin);
	} else {
		mcu_io_output_resetbit(CtrlIO->blport, CtrlIO->blpin);	
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
static s32 bus_lcd_rst(DevLcdNode *lcd, u8 sta)
{
	const DevLcdCtrlIO *CtrlIO;
	DevLcd *dev;

	dev = (DevLcd *)lcd->pnode.pdev;
	
	//LogLcdDrv(LOG_DEBUG, "dev name:%s\r\n", dev->pdev.name);
	
	CtrlIO = dev->ctrlio;

	if (sta ==MCU_IO_STA_1) {
		mcu_io_output_setbit(CtrlIO->rstport, CtrlIO->rstpin);
	} else {
		mcu_io_output_resetbit(CtrlIO->rstport, CtrlIO->rstpin);	
	}
	return 0;
}



#if (PETITE_BUS_LCD_8080 == 1)
extern volatile u16 *LcdReg;
extern volatile u16 *LcdData;
#endif	

static DevLcdNode *bus_lcd_8080_open(DevLcdNode *lcd)
{
	/*8080特殊处理*/
	lcd->basenode = (void *)1;

	return lcd;
}

static s32 bus_lcd_8080_close(DevLcdNode *lcd)
{
	/*8080特殊处理*/
	lcd->basenode = NULL;

	return 0;
}


static s32 bus_lcd_8080_write_data(DevLcdNode *lcd, u8 *data, u32 len)
{
	u32 i;
	u16 *p;

	p = (u16 *)data;

	for (i=0; i<len; i++) {
		*LcdData = *(p+i);	
	}

	return 0;
}

/*	  当需要连续写同一个数值时，调用本函数。
	  当前专门给8080接口的LCD驱动使用        	*/
static s32 bus_lcd_8080_w_data(DevLcdNode *lcd, u16 color, u32 len)
{
	u32 i;
	
	for(i=len; i>0; i--) {
		*LcdData = color;	
	}

	return 0;
}


static s32 bus_lcd_8080_read_data(DevLcdNode *lcd, u8 *data, u32 len)
{
	u32 i;
	

	u16 *p;
	p = (u16 *)data;
			
	for(i=0; i<len; i++){
		*(p+i) = *LcdData;	
	}

	return 0;	
}

static s32 bus_lcd_8080_write_cmd(DevLcdNode *lcd, u16 cmd)
{

	*LcdReg = cmd;

	return 0;
}

BusLcdDrv BusLcd8080Drv = {
	.bl = bus_lcd_bl,
    .rst = bus_lcd_rst, 
    .open = bus_lcd_8080_open,
    .close = bus_lcd_8080_close,
    .write_data = bus_lcd_8080_write_data,
	.w_data = bus_lcd_8080_w_data,
    .read_data = bus_lcd_8080_read_data,
    .write_cmd = bus_lcd_8080_write_cmd,
};

static DevLcdNode *bus_lcd_spi_open(DevLcdNode *lcd)
{

	char *busname;
	uint16_t bus_clk;
	PraSpiSet *spi_pra;
	SPI_MODE spi_mode;
	PDevNode *pnode;

	pnode = (PDevNode *)lcd;
	
	busname = pnode->pdev->basebus;
	

	LogLcdDrv(LOG_DEBUG, "lcd bus spi ch!\r\n");
		
	spi_pra = (PraSpiSet *)pnode->pdev->busconf;
	bus_clk = spi_pra->KHz;
	spi_mode = spi_pra->mode;
		
	lcd->basenode = (void *)bus_spich_open(busname, spi_mode, bus_clk, 0xffffffff);

	
	if (lcd->basenode == NULL) {
		LogLcdDrv(LOG_INFO, "lcd bus open base bus err!\r\n");	

	} else {
		//LogLcdDrv(LOG_DEBUG, "lcd bus open base bus OK!\r\n");
	}

	return lcd;
}

static s32 bus_lcd_spi_close(DevLcdNode *lcd)
{

	bus_spich_close((DevSpiChNode *)lcd->basenode);

	return 0;
}

static s32 bus_lcd_spi_write_data(DevLcdNode *lcd, u8 *data, u32 len)
{
	bus_spich_transfer((DevSpiChNode *)lcd->basenode,  data, NULL, len);
	return 0;
}

static s32 bus_lcd_spi_w_data(DevLcdNode *lcd, u16 color, u32 len)
{

	return 0;
}

static s32 bus_lcd_spi_read_data(DevLcdNode *lcd, u8 *data, u32 len)
{

	bus_spich_transfer((DevSpiChNode *)lcd->basenode,  NULL, data, len);

	return 0;	
}

static s32 bus_lcd_spi_write_cmd(DevLcdNode *lcd, u16 cmd)
{
	u8 tmp[2];
	
	PDevNode *pnode;
	pnode = (PDevNode *)lcd;
	DevLcd *dev = (DevLcd *)lcd->pnode.pdev;
		
	mcu_io_output_resetbit(dev->ctrlio->A0port, dev->ctrlio->A0pin);
	tmp[0] = cmd;
	bus_spich_transfer((DevSpiChNode *)lcd->basenode,  &tmp[0], NULL, 1);
	mcu_io_output_setbit(dev->ctrlio->A0port, dev->ctrlio->A0pin);

	return 0;
}
BusLcdDrv BusLcdSpiDrv = {
	.bl = bus_lcd_bl,
    .rst = bus_lcd_rst, 
    .open = bus_lcd_spi_open,
    .close = bus_lcd_spi_close,
    .write_data = bus_lcd_spi_write_data,
	.w_data = bus_lcd_spi_w_data,
    .read_data = bus_lcd_spi_read_data,
    .write_cmd = bus_lcd_spi_write_cmd,
};

static DevLcdNode *bus_lcd_i2c_open(DevLcdNode *lcd)
{

	char *busname;
	uint16_t bus_clk;
	I2CPra *i2c_pra;

	PDevNode *pnode;

	pnode = (PDevNode *)lcd;
	
	busname = pnode->pdev->basebus;
	
	LogLcdDrv(LOG_DEBUG, "lcd bus i2c!\r\n");
	///@todo I2C频率配置 */
	i2c_pra = (I2CPra *)pnode->pdev->busconf;
	bus_clk = i2c_pra->fkhz;
		
	lcd->basenode = bus_i2c_open(busname, 0xffffffff, bus_clk);

	
	if (lcd->basenode == NULL) {
		LogLcdDrv(LOG_INFO, "lcd bus open base bus err!\r\n");	

	} else {
		//LogLcdDrv(LOG_DEBUG, "lcd bus open base bus OK!\r\n");
	}

	return lcd;
}

static s32 bus_lcd_i2c_close(DevLcdNode *lcd)
{

	bus_i2c_close((DevI2cNode *)lcd->basenode);	

	return 0;
}


static s32 bus_lcd_i2c_write_data(DevLcdNode *lcd, u8 *data, u32 len)
{
	/*直接定义256字节，可能有BUG，要根据len动态申请*/
	u8 tmp[256];
	u32 i;
	
	u8 i2c_addr;
	I2CPra *i2c_pra;
	
	PDevNode *pnode;

	pnode = (PDevNode *)lcd;

	i2c_pra = (I2CPra *)pnode->pdev->busconf;
	i2c_addr = i2c_pra->addr;

	DevLcd *dev = (DevLcd *)lcd->pnode.pdev;
			
	tmp[0] = dev->i2c_data_reg;//写入数据	
	memcpy(&tmp[1], data, len);
	bus_i2c_transfer((DevI2cNode *)lcd->basenode, i2c_addr, MCU_I2C_MODE_W, tmp, len+1);


	
	return 0;
}

static s32 bus_lcd_i2c_w_data(DevLcdNode *lcd, u16 color, u32 len)
{

	return 0;
}

static s32 bus_lcd_i2c_read_data(DevLcdNode *lcd, u8 *data, u32 len)
{
	return 0;	
}

static s32 bus_lcd_i2c_write_cmd(DevLcdNode *lcd, u16 cmd)
{
	u8 tmp[2];
	u8 i2c_addr;
	I2CPra *i2c_pra;
	PDevNode *pnode;
	pnode = (PDevNode *)lcd;
	DevLcd *dev = (DevLcd *)lcd->pnode.pdev;

	i2c_pra = (I2CPra *)pnode->pdev->busconf;
	i2c_addr = i2c_pra->addr;
			
	tmp[0] = dev->i2c_cmd_reg;//写入命令
	tmp[1] = cmd;
			
	bus_i2c_transfer((DevI2cNode *)lcd->basenode, i2c_addr, MCU_I2C_MODE_W, tmp, 2);

	return 0;
}

BusLcdDrv BusLcdI2cDrv = {
	.bl = bus_lcd_bl,
    .rst = bus_lcd_rst, 
    .open = bus_lcd_i2c_open,
    .close = bus_lcd_i2c_close,
    .write_data = bus_lcd_i2c_write_data,
	.w_data = bus_lcd_i2c_w_data,
    .read_data = bus_lcd_i2c_read_data,
    .write_cmd = bus_lcd_i2c_write_cmd,
};