/**
 * @file    drv_st7796u.c
 * @brief   描述
 * 
 * @author  pochard(email@xxx.com)
 * @version 0.1
 * @date    2023-12-10
 * @copyright Copyright (c) 2023..
 */
#include "mcu.h"
#include "petite_config.h"
#include "petite.h"


/*	ST7796U驱动, 3.5寸 IPS 勤 致远 320*480*/
#if 1
#include "drv_lcd.h"
#include "drv_st7796u.h"

#define DEV_ST7796U_DEBUG

#ifdef DEV_ST7796U_DEBUG
#define ST7796U_DEBUG LogLcdDrv
#else
#define ST7796U_DEBUG(a, ...)
#endif

extern void Delay(__IO uint32_t nTime);

/*7796命令定义*/
#define ST7796U_CMD_WRAM 0x2c
#define ST7796U_CMD_SETX 0x2a
#define ST7796U_CMD_SETY 0x2b

s32 drv_ST7796U_init(DevLcdNode *lcd);
static s32 drv_ST7796U_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
s32 drv_ST7796U_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
s32 drv_ST7796U_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
static s32 drv_ST7796U_display_onoff(DevLcdNode *lcd, u8 sta);
s32 drv_ST7796U_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
static void drv_ST7796U_scan_dir(DevLcdNode *lcd, u8 dir);
void drv_ST7796U_lcd_bl(DevLcdNode *lcd, u8 sta);
s32 drv_ST7796U_flush(DevLcdNode *lcd, u16 *color, u32 len);
s32 drv_ST7796U_update(DevLcdNode *lcd);


/*

	定义一个TFT LCD，使用ST7796U驱动IC的设备

*/
_lcd_drv TftLcdSt7796uDrv = {
							.id = 0X7796,

							.init = drv_ST7796U_init,
							.draw_point = drv_ST7796U_drawpoint,
							.color_fill = drv_ST7796U_color_fill,
							.fill = drv_ST7796U_fill,
							.onoff = drv_ST7796U_display_onoff,
							.prepare_display = drv_ST7796U_prepare_display,
							.flush = drv_ST7796U_flush,
							.set_dir = drv_ST7796U_scan_dir,
							.backlight = drv_ST7796U_lcd_bl,
							.update = drv_ST7796U_update,
							};

void drv_ST7796U_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	//DevLcdNode * node;
	
	//node = lcd->busdrv->open(lcd);
	lcd->busdrv->bl(lcd, sta);
	//lcd->busdrv->close(node);
}	
/**
 *@brief:      drv_ST7796U_scan_dir
 *@details:    设置显存扫描方向， 本函数为竖屏角度
 *@param[in]   u8 dir  
 *@param[out]  无
 *@retval:     static
 */
static void drv_ST7796U_scan_dir(DevLcdNode *lcd, u8 dir)
{
	u16 regval=0;

	/*设置从左边到右边还是右边到左边*/
	switch(dir)
	{
		case R2L_U2D:
		case R2L_D2U:
		case U2D_R2L:
		case D2U_R2L:
			regval|=(1<<6); 
			break;	 
	}

	/*设置从上到下还是从下到上*/
	switch(dir)
	{
		case L2R_D2U:
		case R2L_D2U:
		case D2U_L2R:
		case D2U_R2L:
			regval|=(1<<7); 
			break;	 
	}

	/*
		设置先左右还是先上下 Reverse Mode
		如果设置为1，LCD控制器已经将行跟列对调了，
		因此需要在显示中进行调整
	*/
	switch(dir)
	{
		case U2D_L2R:
		case D2U_L2R:
		case U2D_R2L:
		case D2U_R2L:
			regval|=(1<<5);
			break;	 
	}
	/*
		还可以设置RGB还是GBR
		还可以设置调转上下
	*/	
	regval|=(1<<3);

	DevLcdNode * node;
	node = lcd->busdrv->open(lcd);
	
	lcd->busdrv->write_cmd(node, (0x36));
	u16 tmp[2];
	tmp[0] = regval;
	lcd->busdrv->write_data(node, (u8*)tmp, 1);
	lcd->busdrv->close(node);

}

/**
 *@brief:      drv_ST7796U_set_cp_addr
 *@details:    设置控制器的行列地址范围
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  无
 *@retval:     
 */
s32 drv_ST7796U_set_cp_addr(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep)
{

	DevLcdNode * node;
	u16 tmp[4];

	node = lcd->busdrv->open(lcd);

	lcd->busdrv->write_cmd(node, ST7796U_CMD_SETX);
	tmp[0] = (sc>>8);
	tmp[1] = (sc&0XFF);
	tmp[2] = (ec>>8);
	tmp[3] = (ec&0XFF);
	lcd->busdrv->write_data(node, (u8*)tmp, 4);

	lcd->busdrv->write_cmd(node, (ST7796U_CMD_SETY));
	tmp[0] = (sp>>8);
	tmp[1] = (sp&0XFF);
	tmp[2] = (ep>>8);
	tmp[3] = (ep&0XFF);
	lcd->busdrv->write_data(node, (u8*)tmp, 4);

	lcd->busdrv->write_cmd(node, (ST7796U_CMD_WRAM));
	
	lcd->busdrv->close(node);
	
	return 0;
}

/**
 *@brief:      drv_ST7796U_display_onoff
 *@details:    显示或关闭
 *@param[in]   u8 sta  
 *@param[out]  无
 *@retval:     static
 */
static s32 drv_ST7796U_display_onoff(DevLcdNode *lcd, u8 sta)
{
	DevLcdNode * node;
	node = lcd->busdrv->open(lcd);
	///@todo  cmd 确认
	if(sta == 1)
		lcd->busdrv->write_cmd(node, (0x29));
	else
		lcd->busdrv->write_cmd(node, (0x28));

	lcd->busdrv->close(node);
	
	return 0;
}

/**
 *@brief:      drv_ST7796U_init
 *@details:    初始化FSMC，并且读取ST7796U的设备ID
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 drv_ST7796U_init(DevLcdNode *lcd)
{
	BusLcdDrv *busdrv;
	u16 data;
	DevLcdNode * node;
	u16 tmp[16];
	
	busdrv = lcd->busdrv;

	node = busdrv->open(lcd);
	
	busdrv->rst(node, 1);
	Delay(50);
	busdrv->rst(node, 0);
	Delay(50);
	busdrv->rst(node, 1);
	Delay(50);

	busdrv->write_cmd(node, (0xd3));
	/*读4个字节，第一个字节是dummy read， 第二字节是0x00， 第三字节是93，第四字节是41*/
	busdrv->read_data(node, (u8*)tmp, 4);
	
	data = tmp[2]; 
	data<<=8;
	data |= tmp[3];

	ST7796U_DEBUG(LOG_DEBUG, "read reg:%04x\r\n", data);

	if(data != TftLcdSt7796uDrv.id)
	{
		ST7796U_DEBUG(LOG_DEBUG, "lcd drive no 9341\r\n");	
		busdrv->close(node);
		return -1;
	}

	Delay(120);                
	busdrv->write_cmd(node, (0x11));    
	Delay(120);

	busdrv->write_cmd(node, (0x36));
	tmp[0] = 0x48;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->write_cmd(node, (0x3a));
	tmp[0] = 0x55;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->write_cmd(node, (0xf0));
	tmp[0] = 0xc3;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->write_cmd(node, (0xf0));
	tmp[0] = 0x96;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->write_cmd(node, (0xb4));
	tmp[0] = 0x01;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->write_cmd(node, (0xb7));
	tmp[0] = 0xc6;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->write_cmd(node, (0xc0));
	tmp[0] = 0x80;
	tmp[1] = 0x45;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->write_cmd(node, (0xc1));
	tmp[0] = 0x13;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->write_cmd(node, (0xc2));
	tmp[0] = 0x07;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->write_cmd(node, (0xc5));
	tmp[0] = 0x0a;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->write_cmd(node, (0xE8));
	tmp[0] = 0x40; tmp[1] = 0x8A; tmp[2] = 0x00; tmp[3] = 0x00;
	tmp[4] = 0x29; tmp[5] = 0x19; tmp[6] = 0xa5; tmp[7] = 0x33;
	busdrv->write_data(node, (u8*)tmp, 8);

	busdrv->write_cmd(node, (0xE0));
	tmp[0] = 0xd0; tmp[1] = 0x08; tmp[2] = 0x0f; tmp[3] = 0x06;
	tmp[4] = 0x06; tmp[5] = 0x33; tmp[6] = 0x30; tmp[7] = 0x33;
	tmp[8] = 0x47; tmp[9] = 0x17; tmp[10] = 0x13; tmp[11] = 0x13;
	tmp[12] = 0x2b; tmp[13] = 0x31; 
	busdrv->write_data(node, (u8*)tmp, 14);

	busdrv->write_cmd(node, (0xE1));
	tmp[0] = 0xd0; tmp[1] = 0x0a; tmp[2] = 0x11; tmp[3] = 0x0b;
	tmp[4] = 0x09; tmp[5] = 0x07; tmp[6] = 0x2f; tmp[7] = 0x33;
	tmp[8] = 0x47; tmp[9] = 0x38; tmp[10] = 0x15; tmp[11] = 0x16;
	tmp[12] = 0x2c; tmp[13] = 0x32; 
	busdrv->write_data(node, (u8*)tmp, 14);

	busdrv->write_cmd(node, (0xf0));
	tmp[0] = 0x3c;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->write_cmd(node, (0xf0));
	tmp[0] = 0x69;
	busdrv->write_data(node, (u8*)tmp, 1);

	Delay(120);
	busdrv->write_cmd(node, (0x21));
	busdrv->write_cmd(node, (0x29));
	busdrv->write_cmd(node, (0x2c));

	busdrv->write_cmd(node, (0x360));
	tmp[0] = 0x48;
	busdrv->write_data(node, (u8*)tmp, 1);

	busdrv->close(node);
	
	Delay(50);
	
	return 0;
}
/**
 *@brief:      drv_ST7796U_xy2cp
 *@details:    将xy坐标转换为CP坐标
 *@param[in]   无
 *@param[out]  无
 *@retval:     
 */
s32 drv_ST7796U_xy2cp(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey, u16 *sc, u16 *ec, u16 *sp, u16 *ep)
{
	/*
		显示XY轴范围
	*/
	if(sx >= lcd->width)
		sx = lcd->width-1;
	
	if(ex >= lcd->width)
		ex = lcd->width-1;
	
	if(sy >= lcd->height)
		sy = lcd->height-1;
	
	if(ey >= lcd->height)
		ey = lcd->height-1;
	/*
		XY轴，实物角度来看，方向取决于横屏还是竖屏
		CP轴，是控制器显存角度，
		XY轴的映射关系取决于扫描方向
	*/
	if(
		(((lcd->scandir&LRUD_BIT_MASK) == LRUD_BIT_MASK)
		&&(lcd->dir == H_LCD))
		||
		(((lcd->scandir&LRUD_BIT_MASK) == 0)
		&&(lcd->dir == W_LCD))
		){
			*sc = sy;
			*ec = ey;
			*sp = sx;
			*ep = ex;
	} else {
		*sc = sx;
		*ec = ex;
		*sp = sy;
		*ep = ey;
	}
	
	return 0;
}
/**
 *@brief:      drv_ST7796U_drawpoint
 *@details:    画点
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  无
 *@retval:     static
 */
static s32 drv_ST7796U_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	u16 sc,ec,sp,ep;

	drv_ST7796U_xy2cp(lcd, x, x, y, y, &sc,&ec,&sp,&ep);
	drv_ST7796U_set_cp_addr(lcd, sc, ec, sp, ep);

	DevLcdNode * node;
	node = lcd->busdrv->open(lcd);
	
	u16 tmp[2];
	tmp[0] = color;
	lcd->busdrv->write_data(node, (u8*)tmp, 1);
	lcd->busdrv->close(node);
 
	return 0;
}
/**
 *@brief:      drv_ST7796U_color_fill
 *@details:    将一块区域设定为某种颜色
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  无
 *@retval:     
 */
s32 drv_ST7796U_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
#if 0
{

	u16 height,width;
	u16 i,j;
	u16 sc,ec,sp,ep;

	drv_ST7796U_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	drv_ST7796U_set_cp_addr(lcd, sc, ec, sp, ep);

	width=(ec+1)-sc;//得到填充的宽度 +1是因为坐标从0开始
	height=(ep+1)-sp;//高度
	
	//uart_printf("ST7796U width:%d, height:%d\r\n", width, height);

	DevLcdBusNode * node;
	
#define TMP_BUF_SIZE 32
	u16 tmp[TMP_BUF_SIZE];
	u32 cnt;

	for(cnt = 0; cnt < TMP_BUF_SIZE; cnt ++)
	{
		tmp[cnt] = color;
	}
	
	cnt = height*width;
	
	node = lcd->busdrv->open(lcd->dev.buslcd);

	while(1)
	{
		if(cnt < TMP_BUF_SIZE)
		{
			lcd->busdrv->write_data(node, (u8 *)tmp, cnt);
			cnt -= cnt;
		}
		else
		{
			lcd->busdrv->write_data(node, (u8 *)tmp, TMP_BUF_SIZE);
			cnt -= TMP_BUF_SIZE;
		}

		if(cnt <= 0)
			break;
	}
	
	lcd->busdrv->close(node);

	return 0;

}
#else
{

	u16 height,width;
	//u16 i,j;
	u16 hsa,hea,vsa,vea;

	drv_ST7796U_xy2cp(lcd, sx, ex, sy, ey, &hsa,&hea,&vsa,&vea);
	drv_ST7796U_set_cp_addr(lcd, hsa, hea, vsa, vea);

	width = hea - hsa + 1;//得到填充的宽度
	height = vea - vsa + 1;//高度
	
	ST7796U_DEBUG(LOG_DEBUG, "fill width:%d, height:%d\r\n", width, height);

	DevLcdNode * node;
	u32 cnt;
	
	cnt = height*width;
	
	node = lcd->busdrv->open(lcd);
	
	lcd->busdrv->w_data(node, color, cnt);

	lcd->busdrv->close(node);
	return 0;

}
#endif

/**
 *@brief:      drv_ST7796U_color_fill
 *@details:    填充矩形区域
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  每一个点的颜色数据
 *@param[out]  无
 *@retval:     
 */
s32 drv_ST7796U_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{

	u16 height,width;
	//u32 i,j;
	u16 sc,ec,sp,ep;

	//ST7796U_DEBUG(LOG_DEBUG, "%d %d %d %d\r\n",  sx, ex, sy, ey);

	drv_ST7796U_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);

	//ST7796U_DEBUG(LOG_DEBUG, "%d %d %d %d\r\n",  sc,ec,sp,ep);

	drv_ST7796U_set_cp_addr(lcd, sc, ec, sp, ep);

	width=(ec+1)-sc;
	height=(ep+1)-sp;

	//ST7796U_DEBUG(LOG_DEBUG, "fill width:%d, height:%d\r\n", width, height);
	
	DevLcdNode * node;

	node = lcd->busdrv->open(lcd);
	lcd->busdrv->write_data(node, (u8 *)color, height*width);	
	lcd->busdrv->close(node);	 
	
	return 0;

} 

s32 drv_ST7796U_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	u16 sc,ec,sp,ep;
	
	ST7796U_DEBUG(LOG_DEBUG, "XY:-%d-%d-%d-%d-\r\n", sx, ex, sy, ey);
	drv_ST7796U_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	ST7796U_DEBUG(LOG_DEBUG, "cp:-%d-%d-%d-%d-\r\n", sc, ec, sp, ep);
	drv_ST7796U_set_cp_addr(lcd, sc, ec, sp, ep);	
	return 0;
}

s32 drv_ST7796U_flush(DevLcdNode *lcd, u16 *color, u32 len)
{
	DevLcdNode * node = lcd;

	node = lcd->busdrv->open(lcd);
	lcd->busdrv->write_data(node, (u8 *)color,  len);	
	lcd->busdrv->close(node);
	return 0;
} 

s32 drv_ST7796U_update(DevLcdNode *lcd)
{
	return 0;	
}

#endif



