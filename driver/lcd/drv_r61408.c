
#include "mcu.h"
#include "petite_config.h"
#include "petite.h"

#if 1
#include "drv_lcd.h"

#define DRV_R61408_DEBUG

#ifdef DRV_R61408_DEBUG
#define R61408_DEBUG	LogLcdDrv 
#else
#define R61408_DEBUG(a, ...)
#endif

extern void Delay(__IO uint32_t nTime);

#define R61408_CMD_WRAM 0x2c
#define R61408_CMD_SETX 0x2a
#define R61408_CMD_SETY 0x2b
#define R61408_CMD_DIR 0x36

s32 drv_R61408_init(DevLcdNode *lcd);
static s32 drv_R61408_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
s32 drv_R61408_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
s32 drv_R61408_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
static s32 drv_R61408_display_onoff(DevLcdNode *lcd, u8 sta);
s32 drv_R61408_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
static void drv_R61408_scan_dir(DevLcdNode *lcd, u8 dir);
void drv_R61408_lcd_bl(DevLcdNode *lcd, u8 sta);
s32 drv_R61408_flush(DevLcdNode *lcd, u16 *color, u32 len);
s32 drv_R61408_update(DevLcdNode *lcd);


_lcd_drv TftLcdR61408Drv = {
							.id = 0X1408,

							.init = drv_R61408_init,
							.draw_point = drv_R61408_drawpoint,
							.color_fill = drv_R61408_color_fill,
							.fill = drv_R61408_fill,
							.onoff = drv_R61408_display_onoff,
							.prepare_display = drv_R61408_prepare_display,
							.flush = drv_R61408_flush,
							.set_dir = drv_R61408_scan_dir,
							.backlight = drv_R61408_lcd_bl,
							.update = drv_R61408_update,
							};

void drv_R61408_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	//DevLcdNode * node;
	
	//node = lcd->busdrv->open(lcd);
	lcd->busdrv->bl(lcd, sta);
	//lcd->busdrv->close(node);

}
	
/**
 *@brief:      drv_R61408_scan_dir
 *@details:    设置显存扫描方向， 本函数为竖屏角度
 *@param[in]   u8 dir  
 *@param[out]  无
 *@retval:     static
 */
static void drv_R61408_scan_dir(DevLcdNode *lcd, u8 dir)
{
	u16 regval=0;

	#if 1
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
	//regval|=(1<<3);//1:GBR,0:RGB 不同驱动IC有差异

	DevLcdNode * node;
	node = lcd->busdrv->open(lcd);
	
	lcd->busdrv->write_cmd(node, (R61408_CMD_DIR));
	
	u16 tmp[2];
	tmp[0] = regval;
	lcd->busdrv->write_data(node, (u8*)tmp, 1);
	lcd->busdrv->close(node);
	#endif
}

/**
 *@brief:      drv_R61408_set_cp_addr
 *@details:    设置控制器的行列地址范围
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  无
 *@retval:     
 */
s32 drv_R61408_set_cp_addr(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep)
{

	DevLcdNode * node;
	u16 tmp[4];

	node = lcd->busdrv->open(lcd);

	tmp[0] = (sc>>8);
	tmp[1] = (sc&0XFF);
	tmp[2] = (ec>>8);
	tmp[3] = (ec&0XFF);
	
	lcd->busdrv->write_cmd(node, R61408_CMD_SETX);
	lcd->busdrv->write_data(node, (u8*)&tmp[0], 4);

	tmp[0] = (sp>>8);
	tmp[1] = (sp&0XFF);
	tmp[2] = (ep>>8);
	tmp[3] = (ep&0XFF);
	lcd->busdrv->write_cmd(node, (R61408_CMD_SETY));
	lcd->busdrv->write_data(node, (u8*)&tmp[0], 4);

	lcd->busdrv->write_cmd(node, (R61408_CMD_WRAM));
	
	lcd->busdrv->close(node);

	return 0;
}
/**
 *@brief:      drv_R61408_display_onoff
 *@details:    显示或关闭
 *@param[in]   u8 sta  
 *@param[out]  无
 *@retval:     static
 */
static s32 drv_R61408_display_onoff(DevLcdNode *lcd, u8 sta)
{
	DevLcdNode * node;
	node = lcd->busdrv->open(lcd);

	#if 0
	if(sta == 1)
		lcd->busdrv->write_cmd(node, (0x29));
	else
		lcd->busdrv->write_cmd(node, (0x28));
	#endif
	lcd->busdrv->close(node);
	
	return 0;
}

typedef struct {
	u16 cmd;
	u8 len;
	u16 data[32];
}strR61408Init;

const strR61408Init R61408Init[]={

	#if 0
	{0xB3, 2, {0x02, 0x00}},
	
	{0xC1, 15,{0x23, 0x31, 0x99, 0x21,
				   0x20, 0x00, 0x10, 0x28,
				   0x0c, 0x0a, 0x00, 0x00,
				   0x00, 0x21, 0x01}},
				   
	{0xC2, 6, {0x00, 0x06, 0x06, 0x01,
				0x03, 0x00}},
				
	{0xC8, 24,{0x01, 0x0a, 0x12, 0x1c,
			   0x2b, 0x45, 0x3f, 0x29,
			   0x17, 0x13, 0x0f, 0x04, 
			   0x01, 0x0a, 0x12, 0x1c,
			   0x2b, 0x45, 0x3f, 0x29,
			   0x17, 0x13, 0x0f, 0x04}},
			   
	{0xC9, 24,{0x01, 0x0a, 0x12, 0x1c,
				0x2b, 0x45, 0x3f, 0x29,
				0x17, 0x13, 0x0f, 0x04,
				0x01, 0x0a, 0x12, 0x1c,
			   	0x2b, 0x45, 0x3f, 0x29,
			   	0x17, 0x13, 0x0f, 0x04}},

	{0xCA, 24,{0x01, 0x0a, 0x12, 0x1c,
				0x2b, 0x45, 0x3f, 0x29,
				0x17, 0x13, 0x0f, 0x04,
				0x01, 0x0a, 0x12, 0x1c,
			   	0x2b, 0x45, 0x3f, 0x29,
			   	0x17, 0x13, 0x0f, 0x04}},

	{0xD0, 16,{0x99, 0x03, 0xCe, 0xA6,
				0x00, 0x43, 0x20, 0x10,
				0x01, 0x00, 0x01, 0x01,
				0x00, 0x03, 0x01, 0x00}},

	{0xD3, 1, {0x33}},

	{0xD5, 2, {0x2a, 0x2a}},
	
	{0xD6, 1, {0x28}},

	{0xDE, 2,{0x01, 0x4f}},
	
	{0xE6, 1,{0x4f}},
	
	{0xFA, 1,{0x03}},
	/*-----------------------------*/
	{0x2A, 4,{0x00, 0x00, 0x01, 0xDF}},
	{0x2B, 4,{0x00, 0x00, 0x03, 0x1F}},
	#endif
	#if 1
	//{0xB0, 1, {0x04}},
	{0xB3, 2, {0x02, 0x00}},
	{0xB6, 2, {0x52, 0x83}},
	{0xB7, 4, {0x80,0x72,0x11,0x25}},
	{0xB8, 20,{0x00, 0x0F, 0x0F, 0xFF, 
				0xFF, 0xC8, 0xC8, 0x02, 
				0x18, 0x10, 0x10, 0x37,
				0x5A, 0x87, 0xBE, 0xFF,
				0x00, 0x00, 0x00, 0x00}},
	{0xB9, 4, {0x00, 0x00, 0x00, 0x00}},
	{0xBD, 1, {0x00}},
	{0xC0, 2, {0x02, 0x76}},
	{0xC1, 15,{0x63, 0x31, 0x00, 0x27,
			   0x27, 0x32, 0x12, 0x28,
			   0x4E, 0x10, 0xA5, 0x0F,
			   0x58, 0x21, 0x01}},
	{0xC2, 6, {0x28, 0x06, 0x06, 0x01,
				0x03, 0x00}},
	{0xC3, 3, {0x40, 0x00, 0x03}},
	{0xC4, 2, {0x00, 0x01}},
	{0xC6, 2, {0x00, 0x00}},
	{0xC7, 5, {0x11, 0x8D, 0xA0, 0xF5, 0x27}},
	{0xC8, 24,{0x02, 0x13, 0x18, 0x25,
			   0x34, 0x4E, 0x36, 0x23,
			   0x17, 0x0E, 0x0C, 0x02, 
			   0x02, 0x13, 0x18, 0x25,
			   0x34, 0x4E, 0x36, 0x23,
			   0x17, 0x0E, 0x0C, 0x02}},
	{0xC9, 24,{0x02, 0x13, 0x18, 0x25,
				0x34, 0x4E, 0x36, 0x23,
				0x17, 0x0E, 0x0C, 0x02,
				0x02, 0x13, 0x18, 0x25,
				0x34, 0x4E, 0x36, 0x23,
				0x17, 0x0E, 0x0C, 0x02}},
	{0xCA, 24,{0x02, 0x13, 0x18, 0x25,
				0x34, 0x4E, 0x36, 0x23,
				0x17, 0x0E, 0x0C, 0x02,
				0x02, 0x13, 0x18, 0x25,
				0x34, 0x4E, 0x36, 0x23,
				0x17, 0x0E, 0x0C, 0x02}},
	{0xD0, 16,{0xA9, 0x03, 0xCC, 0xA5,
				0x00, 0x53, 0x20, 0x10,
				0x01, 0x00, 0x01, 0x01,
				0x00, 0x03, 0x01, 0x00}},
	{0xD1, 7, {0x18, 0x0C, 0x23, 0x03,
				0x75, 0x02, 0x50}},
	{0xD3, 1, {0x33}},
	{0xD5, 2, {0x2a, 0x2a}},
	{0xD6, 1, {0x28}},
	{0xD7, 15,{0x01, 0x00, 0xAA, 0xC0,
				0x2A, 0x2C, 0x22, 0x12,
				0x71, 0x0A, 0x12, 0x00,
				0xA0, 0x00, 0x03}},
	{0xD8, 8,{0x44, 0x44, 0x22, 0x44,
				0x21, 0x46, 0x42, 0x40}},
	{0xD9, 3,{0xCF, 0x2D, 0x51}},
	{0xDA, 1,{0x01}},
	{0xDE, 2,{0x01, 0x51}},
	{0xE1, 6,{0x00, 0x00, 0x00, 0x00,
				0x00, 0x00}},
	{0xE6, 1,{0x55}},
	{0xF3, 5,{0x06, 0x00, 0x00, 0x24,
				0x00}},
	{0xF8, 1,{0x00}},
	{0xFA, 1,{0x01}},
	{0xFB, 3,{0x00, 0x00, 0x00}},
	{0xFC, 5,{0x00, 0x00, 0x00, 0x00,
				0x00}},
	{0xFD, 13,{0x00, 0x00, 0x70, 0x00,
				0x72, 0x31, 0x37,0x70,
				0x32, 0x31, 0x07, 0x00,
				0x00}},
	{0xFE, 5,{0x00, 0x00, 0x00, 0x00,
				0x20}},
	{0xB0, 1,{0x04}},
	/*-----36------*/
	{0x35, 1,{0x00}},
	{0x44, 1,{0x00}},
	{0x36, 1,{0x00}},
	{0x3A, 1,{0x55}},
	{0x2A, 4,{0x00, 0x00, 0x01, 0xDF}},
	{0x2B, 4,{0x00, 0x00, 0x03, 0x1F}},
	{0x29, 1,{0x00}},
	/*-----43------*/
	#endif
};


/**
 *@brief:      drv_R61408_init
 *@details:    初始化FSMC，并且读取NT35510的设备ID
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 drv_R61408_init(DevLcdNode *lcd)
{
	u16 data;
	DevLcdNode * node;
	u16 tmp[16];
	u16 i;
	
	R61408_DEBUG(LOG_DEBUG, "%s\r\n", __FUNCTION__);
	
	node = lcd->busdrv->open(lcd);

	lcd->busdrv->rst(node, 1);
	Delay(10);
	lcd->busdrv->rst(node, 0);
	Delay(55);
	lcd->busdrv->rst(node, 1);
	Delay(55);

	//exit_sleep_mode
	lcd->busdrv->write_cmd(node, (0x11));
	lcd->busdrv->write_data(node, "\x00", 1);
	Delay(40);
	
	//Manufacturer Command Access Protect 解锁一些命令
	lcd->busdrv->write_cmd(node, (0xB0));
	lcd->busdrv->write_data(node, "\x04", 1);
	
	lcd->busdrv->write_cmd(node, (0x00bf));
	lcd->busdrv->read_data(node, (u8*)tmp, 5);
	
	data = tmp[3]; 
	data<<=8;
	data |= tmp[4];

	R61408_DEBUG(LOG_DEBUG, "read reg:%04x\r\n", data);

	if(data != TftLcdR61408Drv.id)
	{
		R61408_DEBUG(LOG_DEBUG, "lcd drive no R61408\r\n");	
		lcd->busdrv->close(node);
		return -1;
	}

	i=0;
	while(1) {

		lcd->busdrv->write_cmd(node, (R61408Init[i].cmd));
		lcd->busdrv->write_data(node, (u8*)R61408Init[i].data, R61408Init[i].len);
		i++;
		if(i>= 36) break;
	}

	Delay(100);
	while(1) {

		lcd->busdrv->write_cmd(node, (R61408Init[i].cmd));
		lcd->busdrv->write_data(node, (u8*)R61408Init[i].data, R61408Init[i].len);
		i++;
		if(i>= 43) break;
	}    

	#if 1
	Delay(180);
	lcd->busdrv->write_cmd(node, (0x2c));
	lcd->busdrv->write_data(node, "\x00", 1);
	Delay(10);
	lcd->busdrv->write_cmd(node, (0x36));
	lcd->busdrv->write_data(node, "\x68", 1);
	#endif
	lcd->busdrv->close(node);
	
	Delay(50);
	
	return 0;
}
/**
 *@brief:      drv_R61408_xy2cp
 *@details:    将xy坐标转换为CP坐标
 *@param[in]   无
 *@param[out]  无
 *@retval:     
 */
s32 drv_R61408_xy2cp(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey, u16 *sc, u16 *ec, u16 *sp, u16 *ep)
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
		)
	{
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
 *@brief:      drv_R61408_drawpoint
 *@details:    画点
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  无
 *@retval:     static
 */
static s32 drv_R61408_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	u16 sc,ec,sp,ep;

	drv_R61408_xy2cp(lcd, x, x, y, y, &sc,&ec,&sp,&ep);
	drv_R61408_set_cp_addr(lcd, sc, ec, sp, ep);

	DevLcdNode * node;
	node = lcd->busdrv->open(lcd);
	
	u16 tmp[2];
	tmp[0] = color;
	lcd->busdrv->write_data(node, (u8*)tmp, 1);
	lcd->busdrv->close(node);
 
	return 0;
}
/**
 *@brief:      drv_R61408_color_fill
 *@details:    将一块区域设定为某种颜色
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  无
 *@retval:     
 */
s32 drv_R61408_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{

	u16 height,width;
	u16 i,j;
	u16 hsa,hea,vsa,vea;

	drv_R61408_xy2cp(lcd, sx, ex, sy, ey, &hsa,&hea,&vsa,&vea);
	drv_R61408_set_cp_addr(lcd, hsa, hea, vsa, vea);

	width = hea - hsa + 1;//得到填充的宽度
	height = vea - vsa + 1;//高度
	
	//uart_printf("ili9325 width:%d, height:%d\r\n", width, height);
	
	DevLcdNode * node;
	u32 cnt;
	
	cnt = height*width;
	
	node = lcd->busdrv->open(lcd);
	
	lcd->busdrv->w_data(node, color, cnt);

	lcd->busdrv->close(node);
	return 0;

}

/**
 *@brief:      drv_R61408_color_fill
 *@details:    填充矩形区域
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  每一个点的颜色数据
 *@param[out]  无
 *@retval:     
 */
s32 drv_R61408_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{

	u16 height,width;
	u32 i,j;
	u16 sc,ec,sp,ep;

	drv_R61408_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	drv_R61408_set_cp_addr(lcd, sc, ec, sp, ep);

	width=(ec+1)-sc;
	height=(ep+1)-sp;

	LogLcdDrv(LOG_DEBUG, "fill width:%d, height:%d\r\n", width, height);
	
	DevLcdNode * node;

	node = lcd->busdrv->open(lcd);
	lcd->busdrv->write_data(node, (u8 *)color, height*width);	
	lcd->busdrv->close(node);	 
	
	return 0;

} 

s32 drv_R61408_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	u16 sc,ec,sp,ep;
	
	LogLcdDrv(LOG_DEBUG, "XY:-%d-%d-%d-%d-\r\n", sx, ex, sy, ey);
	drv_R61408_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	LogLcdDrv(LOG_DEBUG, "cp:-%d-%d-%d-%d-\r\n", sc, ec, sp, ep);
	drv_R61408_set_cp_addr(lcd, sc, ec, sp, ep);	
	return 0;
}

s32 drv_R61408_flush(DevLcdNode *lcd, u16 *color, u32 len)
{
	DevLcdNode * node = lcd;
	node = lcd->busdrv->open(lcd);
	lcd->busdrv->write_data(node, (u8 *)color,  len);	
	lcd->busdrv->close(node);
	return 0;
} 
s32 drv_R61408_update(DevLcdNode *lcd)
{
	return 0;	
}

#endif


