
#include "mcu.h"
#include "board_sysconf.h"
#include "alloc.h"
#include "log.h"
#include "bus_lcd.h"

#include "dev_lcd.h"

#include "mcu_fsmc.h"

#define DRV_NT35510_DEBUG

#ifdef DRV_NT35510_DEBUG
#define NT35510_DEBUG	wjq_log 
#else
#define NT35510_DEBUG(a, ...)
#endif

extern void Delay(__IO uint32_t nTime);


#if( LCD_DRIVER_NT35510 == 1 )


#define NT35510_CMD_WRAM 0x2c00
#define NT35510_CMD_SETX 0x2a00
#define NT35510_CMD_SETY 0x2b00

s32 drv_NT35510_init(DevLcdNode *lcd);
static s32 drv_NT35510_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
s32 drv_NT35510_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
s32 drv_NT35510_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
static s32 drv_NT35510_display_onoff(DevLcdNode *lcd, u8 sta);
s32 drv_NT35510_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
static void drv_NT35510_scan_dir(DevLcdNode *lcd, u8 dir);
void drv_NT35510_lcd_bl(DevLcdNode *lcd, u8 sta);
s32 drv_NT35510_flush(DevLcdNode *lcd, u16 *color, u32 len);
s32 drv_NT35510_update(DevLcdNode *lcd);


_lcd_drv TftLcdNT35510Drv = {
							.id = 0X5510,

							.init = drv_NT35510_init,
							.draw_point = drv_NT35510_drawpoint,
							.color_fill = drv_NT35510_color_fill,
							.fill = drv_NT35510_fill,
							.onoff = drv_NT35510_display_onoff,
							.prepare_display = drv_NT35510_prepare_display,
							.flush = drv_NT35510_flush,
							.set_dir = drv_NT35510_scan_dir,
							.backlight = drv_NT35510_lcd_bl,
							.update = drv_NT35510_update,
							};

void drv_NT35510_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_bl(node, sta);
	bus_lcd_close(node);

}
	
/**
 *@brief:      drv_NT35510_scan_dir
 *@details:    设置显存扫描方向， 本函数为竖屏角度
 *@param[in]   u8 dir  
 *@param[out]  无
 *@retval:     static
 */
static void drv_NT35510_scan_dir(DevLcdNode *lcd, u8 dir)
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
	//regval|=(1<<3);//1:GBR,0:RGB 不同驱动IC有差异

	DevLcdBusNode * node;
	node = bus_lcd_open(lcd->dev.buslcd);
	
	bus_lcd_write_cmd(node, (0x3600));
	
	u16 tmp[2];
	tmp[0] = regval;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	bus_lcd_close(node);

}

/**
 *@brief:      drv_NT35510_set_cp_addr
 *@details:    设置控制器的行列地址范围
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  无
 *@retval:     
 */
s32 drv_NT35510_set_cp_addr(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep)
{

	DevLcdBusNode * node;
	u16 tmp[4];

	node = bus_lcd_open(lcd->dev.buslcd);

	
	tmp[0] = (sc>>8);
	tmp[1] = (sc&0XFF);
	tmp[2] = (ec>>8);
	tmp[3] = (ec&0XFF);
	
	bus_lcd_write_cmd(node, NT35510_CMD_SETX);
	bus_lcd_write_data(node, (u8*)&tmp[0], 1);
	bus_lcd_write_cmd(node, NT35510_CMD_SETX+1);
	bus_lcd_write_data(node, (u8*)&tmp[1], 1);
	bus_lcd_write_cmd(node, NT35510_CMD_SETX+2);
	bus_lcd_write_data(node, (u8*)&tmp[2], 1);
	bus_lcd_write_cmd(node, NT35510_CMD_SETX+3);
	bus_lcd_write_data(node, (u8*)&tmp[3], 1);

	
	tmp[0] = (sp>>8);
	tmp[1] = (sp&0XFF);
	tmp[2] = (ep>>8);
	tmp[3] = (ep&0XFF);
	bus_lcd_write_cmd(node, (NT35510_CMD_SETY));
	bus_lcd_write_data(node, (u8*)&tmp[0], 1);
	bus_lcd_write_cmd(node, (NT35510_CMD_SETY+1));
	bus_lcd_write_data(node, (u8*)&tmp[1], 1);
	bus_lcd_write_cmd(node, (NT35510_CMD_SETY+2));
	bus_lcd_write_data(node, (u8*)&tmp[2], 1);
	bus_lcd_write_cmd(node, (NT35510_CMD_SETY+3));
	bus_lcd_write_data(node, (u8*)&tmp[3], 1);

	bus_lcd_write_cmd(node, (NT35510_CMD_WRAM));
	
	bus_lcd_close(node);
	
	return 0;
}
/**
 *@brief:      drv_NT35510_display_onoff
 *@details:    显示或关闭
 *@param[in]   u8 sta  
 *@param[out]  无
 *@retval:     static
 */
static s32 drv_NT35510_display_onoff(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	node = bus_lcd_open(lcd->dev.buslcd);

	#if 0
	if(sta == 1)
		bus_lcd_write_cmd(node, (0x29));
	else
		bus_lcd_write_cmd(node, (0x28));
	#endif
	bus_lcd_close(node);
	
	return 0;
}

typedef struct {
	u16 cmd;
	u8 data;
}NT35510_Init_CMD;


NT35510_Init_CMD NT35510_Init_List[]={
	{0xF000, 0x55},	{0xF001, 0xAA},	{0xF002, 0x52},	{0xF003, 0x08},	{0xF004, 0x01},
	//# AVDD: manual); LCD_WR_DATA(
	{0xB600, 0x34},	{0xB601, 0x34},	{0xB602, 0x34},

	{0xB000, 0x0D},	{0xB001, 0x0D},	{0xB002, 0x0D},
	//# AVEE: manual); LCD_WR_DATA( -6V
	{0xB700, 0x24},	{0xB701, 0x24},	{0xB702, 0x24},

	{0xB100, 0x0D},	{0xB101, 0x0D},	{0xB102, 0x0D},
	//#Power Control for
	//VCL
	{0xB800, 0x24},	{0xB801, 0x24},	{0xB802, 0x24},

	{0xB200, 0x00},
	//# VGH: Clamp Enable); LCD_WR_DATA(
	{0xB900, 0x24},	{0xB901, 0x24},	{0xB902, 0x24},

	{0xB300, 0x05},	{0xB301, 0x05},	{0xB302, 0x05},
	///LCD_WR_REG(0xBF00); LCD_WR_DATA(0x01);
	//# VGL(LVGL):
	{0xBA00, 0x34},	{0xBA01, 0x34},	{0xBA02, 0x34},
	//# VGL_REG(VGLO)
	{0xB500, 0x0B},	{0xB501, 0x0B},	{0xB502, 0x0B},
	//# VGMP/VGSP:
	{0xBC00, 0X00},	{0xBC01, 0xA3},	{0xBC02, 0X00},
	//# VGMN/VGSN
	{0xBD00, 0x00},	{0xBD01, 0xA3},	{0xBD02, 0x00},
	//# VCOM=-0.1
	{0xBE00, 0x00},	{0xBE01, 0x63},//4f
	//  VCOMH+0x01;
	//#R+
	{0xD100, 0x00},	{0xD101, 0x37},	{0xD102, 0x00},
	{0xD103, 0x52},	{0xD104, 0x00},	{0xD105, 0x7B},
	{0xD106, 0x00},	{0xD107, 0x99},	{0xD108, 0x00},
	{0xD109, 0xB1},	{0xD10A, 0x00},	{0xD10B, 0xD2},
	{0xD10C, 0x00},	{0xD10D, 0xF6},	{0xD10E, 0x01},
	{0xD10F, 0x27},	{0xD110, 0x01},	{0xD111, 0x4E},
	{0xD112, 0x01},	{0xD113, 0x8C},	{0xD114, 0x01},
	{0xD115, 0xBE},	{0xD116, 0x02},	{0xD117, 0x0B},
	{0xD118, 0x02},	{0xD119, 0x48},	{0xD11A, 0x02},
	{0xD11B, 0x4A},	{0xD11C, 0x02},	{0xD11D, 0x7E},
	{0xD11E, 0x02},	{0xD11F, 0xBC},	{0xD120, 0x02},
	{0xD121, 0xE1},	{0xD122, 0x03},	{0xD123, 0x10},
	{0xD124, 0x03},	{0xD125, 0x31},	{0xD126, 0x03},
	{0xD127, 0x5A},	{0xD128, 0x03},	{0xD129, 0x73},
	{0xD12A, 0x03},	{0xD12B, 0x94},	{0xD12C, 0x03},
	{0xD12D, 0x9F},	{0xD12E, 0x03},	{0xD12F, 0xB3},
	{0xD130, 0x03},	{0xD131, 0xB9},	{0xD132, 0x03},
	{0xD133, 0xC1},	
	//#G+
	{0xD200, 0x00},	{0xD201, 0x37},	{0xD202, 0x00},
	{0xD203, 0x52},	{0xD204, 0x00},	{0xD205, 0x7B},
	{0xD206, 0x00},	{0xD207, 0x99},	{0xD208, 0x00},
	{0xD209, 0xB1},	{0xD20A, 0x00},	{0xD20B, 0xD2},
	{0xD20C, 0x00},	{0xD20D, 0xF6},	{0xD20E, 0x01},
	{0xD20F, 0x27},	{0xD210, 0x01},	{0xD211, 0x4E},
	{0xD212, 0x01},	{0xD213, 0x8C},	{0xD214, 0x01},
	{0xD215, 0xBE},	{0xD216, 0x02},	{0xD217, 0x0B},
	{0xD218, 0x02},	{0xD219, 0x48},	{0xD21A, 0x02},
	{0xD21B, 0x4A},	{0xD21C, 0x02},	{0xD21D, 0x7E},
	{0xD21E, 0x02},	{0xD21F, 0xBC},	{0xD220, 0x02},
	{0xD221, 0xE1},	{0xD222, 0x03},	{0xD223, 0x10},
	{0xD224, 0x03},	{0xD225, 0x31},	{0xD226, 0x03},
	{0xD227, 0x5A},	{0xD228, 0x03},	{0xD229, 0x73},
	{0xD22A, 0x03},	{0xD22B, 0x94},	{0xD22C, 0x03},
	{0xD22D, 0x9F},	{0xD22E, 0x03},	{0xD22F, 0xB3},
	{0xD230, 0x03},	{0xD231, 0xB9},	{0xD232, 0x03},
	{0xD233, 0xC1},	
	//#B+
	{0xD300, 0x00},	{0xD301, 0x37},	{0xD302, 0x00},
	{0xD303, 0x52},	{0xD304, 0x00},	{0xD305, 0x7B},
	{0xD306, 0x00},	{0xD307, 0x99},	{0xD308, 0x00},
	{0xD309, 0xB1},	{0xD30A, 0x00},	{0xD30B, 0xD2},
	{0xD30C, 0x00},	{0xD30D, 0xF6},	{0xD30E, 0x01},
	{0xD30F, 0x27},	{0xD310, 0x01},	{0xD311, 0x4E},
	{0xD312, 0x01},	{0xD313, 0x8C},	{0xD314, 0x01},
	{0xD315, 0xBE},	{0xD316, 0x02},	{0xD317, 0x0B},
	{0xD318, 0x02},	{0xD319, 0x48},	{0xD31A, 0x02},
	{0xD31B, 0x4A},	{0xD31C, 0x02},	{0xD31D, 0x7E},
	{0xD31E, 0x02},	{0xD31F, 0xBC},	{0xD320, 0x02},
	{0xD321, 0xE1},	{0xD322, 0x03},	{0xD323, 0x10},
	{0xD324, 0x03},	{0xD325, 0x31},	{0xD326, 0x03},
	{0xD327, 0x5A},	{0xD328, 0x03},	{0xD329, 0x73},
	{0xD32A, 0x03},	{0xD32B, 0x94},	{0xD32C, 0x03},
	{0xD32D, 0x9F},	{0xD32E, 0x03},	{0xD32F, 0xB3},
	{0xD330, 0x03},	{0xD331, 0xB9},	{0xD332, 0x03},
	{0xD333, 0xC1},
	//#R-///////////////////////////////////////////
	{0xD400, 0x00},	{0xD401, 0x37},	{0xD402, 0x00},
	{0xD403, 0x52},	{0xD404, 0x00},	{0xD405, 0x7B},
	{0xD406, 0x00},	{0xD407, 0x99},	{0xD408, 0x00},
	{0xD409, 0xB1},	{0xD40A, 0x00},	{0xD40B, 0xD2},
	{0xD40C, 0x00},	{0xD40D, 0xF6},	{0xD40E, 0x01},
	{0xD40F, 0x27},	{0xD410, 0x01},	{0xD411, 0x4E},
	{0xD412, 0x01},	{0xD413, 0x8C},	{0xD414, 0x01},
	{0xD415, 0xBE},	{0xD416, 0x02},	{0xD417, 0x0B},
	{0xD418, 0x02},	{0xD419, 0x48},	{0xD41A, 0x02},
	{0xD41B, 0x4A},	{0xD41C, 0x02},	{0xD41D, 0x7E},
	{0xD41E, 0x02},	{0xD41F, 0xBC},	{0xD420, 0x02},
	{0xD421, 0xE1},	{0xD422, 0x03},	{0xD423, 0x10},
	{0xD424, 0x03},	{0xD425, 0x31},	{0xD426, 0x03},
	{0xD427, 0x5A},	{0xD428, 0x03},	{0xD429, 0x73},
	{0xD42A, 0x03},	{0xD42B, 0x94},	{0xD42C, 0x03},
	{0xD42D, 0x9F},	{0xD42E, 0x03},	{0xD42F, 0xB3},
	{0xD430, 0x03},	{0xD431, 0xB9},	{0xD432, 0x03},
	{0xD433, 0xC1},
	//#G-/////////////////////////////////////////////
	{0xD500, 0x00},	{0xD501, 0x37},	{0xD502, 0x00},
	{0xD503, 0x52},	{0xD504, 0x00},	{0xD505, 0x7B},
	{0xD506, 0x00},	{0xD507, 0x99},	{0xD508, 0x00},
	{0xD509, 0xB1},	{0xD50A, 0x00},	{0xD50B, 0xD2},
	{0xD50C, 0x00},	{0xD50D, 0xF6},	{0xD50E, 0x01},
	{0xD50F, 0x27},	{0xD510, 0x01},	{0xD511, 0x4E},
	{0xD512, 0x01},	{0xD513, 0x8C},	{0xD514, 0x01},
	{0xD515, 0xBE},	{0xD516, 0x02},	{0xD517, 0x0B},
	{0xD518, 0x02},	{0xD519, 0x48},	{0xD51A, 0x02},
	{0xD51B, 0x4A},	{0xD51C, 0x02},	{0xD51D, 0x7E},
	{0xD51E, 0x02},	{0xD51F, 0xBC},	{0xD520, 0x02},
	{0xD521, 0xE1},	{0xD522, 0x03},	{0xD523, 0x10},
	{0xD524, 0x03},	{0xD525, 0x31},	{0xD526, 0x03},
	{0xD527, 0x5A},	{0xD528, 0x03},	{0xD529, 0x73},
	{0xD52A, 0x03},	{0xD52B, 0x94},	{0xD52C, 0x03},
	{0xD52D, 0x9F},	{0xD52E, 0x03},	{0xD52F, 0xB3},
	{0xD530, 0x03},	{0xD531, 0xB9},	{0xD532, 0x03},
	{0xD533, 0xC1},
	//#B-///////////////////////////////
	{0xD600, 0x00},	{0xD601, 0x37},	{0xD602, 0x00},
	{0xD603, 0x52},	{0xD604, 0x00},	{0xD605, 0x7B},
	{0xD606, 0x00},	{0xD607, 0x99},	{0xD608, 0x00},
	{0xD609, 0xB1},	{0xD60A, 0x00},	{0xD60B, 0xD2},
	{0xD60C, 0x00},	{0xD60D, 0xF6},	{0xD60E, 0x01},
	{0xD60F, 0x27},	{0xD610, 0x01},	{0xD611, 0x4E},
	{0xD612, 0x01},	{0xD613, 0x8C},	{0xD614, 0x01},
	{0xD615, 0xBE},	{0xD616, 0x02},	{0xD617, 0x0B},
	{0xD618, 0x02},	{0xD619, 0x48},	{0xD61A, 0x02},
	{0xD61B, 0x4A},	{0xD61C, 0x02},	{0xD61D, 0x7E},
	{0xD61E, 0x02},	{0xD61F, 0xBC},	{0xD620, 0x02},
	{0xD621, 0xE1},	{0xD622, 0x03},	{0xD623, 0x10},
	{0xD624, 0x03},	{0xD625, 0x31},	{0xD626, 0x03},
	{0xD627, 0x5A},	{0xD628, 0x03},	{0xD629, 0x73},
	{0xD62A, 0x03},	{0xD62B, 0x94},	{0xD62C, 0x03},
	{0xD62D, 0x9F},	{0xD62E, 0x03},	{0xD62F, 0xB3},
	{0xD630, 0x03},	{0xD631, 0xB9},	{0xD632, 0x03},
	{0xD633, 0xC1},
	//#Enable Page0
	{0xF000, 0x55},	{0xF001, 0xAA},	{0xF002, 0x52},
	{0xF003, 0x08},	{0xF004, 0x00},
	//# RGB I/F Setting
	{0xB000, 0x08},	{0xB001, 0x05},	{0xB002, 0x02},
	{0xB003, 0x05},	{0xB004, 0x02},
	//## SDT:
	{0xB600, 0x08},	{0xB500, 0x50},//0x6b ???? 480x854		 0x50 ???? 480x800
	//## Gate EQ:
	{0xB700, 0x00},	{0xB701, 0x00},
	//## Source EQ:
	{0xB800, 0x01},	{0xB801, 0x05},	{0xB802, 0x05},
	{0xB803, 0x05},
	//# Inversion: Column inversion (NVT)
	{0xBC00, 0x00},	{0xBC01, 0x00},	{0xBC02, 0x00},
	//# BOE's Setting(default)
	{0xCC00, 0x03},	{0xCC01, 0x00},	{0xCC02, 0x00},
	//# Display Timing:
	{0xBD00, 0x01},	{0xBD01, 0x84},	{0xBD02, 0x07},
	{0xBD03, 0x31},	{0xBD04, 0x00},

	{0xBA00, 0x01},

	{0xFF00, 0xAA},	{0xFF01, 0x55},	{0xFF02, 0x25},
	{0xFF03, 0x01},

	{0x3500, 0x00},	{0x3600, 0x00},	{0x3a00, 0x55},  

};



/**
 *@brief:      drv_NT35510_init
 *@details:    初始化FSMC，并且读取NT35510的设备ID
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 drv_NT35510_init(DevLcdNode *lcd)
{
	u16 data;
	DevLcdBusNode * node;
	u16 tmp[16];
	u16 i;
	
	NT35510_DEBUG(LOG_DEBUG, "%s\r\n", __FUNCTION__);
	
	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_rst(node, 1);
	Delay(50);
	bus_lcd_rst(node, 0);
	Delay(50);
	bus_lcd_rst(node, 1);
	Delay(50);

	/*  初始化 读ID */
	bus_lcd_write_cmd(node, (0xF000));
	bus_lcd_write_data(node, "\x55", 1);
	bus_lcd_write_cmd(node, (0xF001));
	bus_lcd_write_data(node, "\xaa", 1);
	bus_lcd_write_cmd(node, (0xF002));
	bus_lcd_write_data(node, "\x52", 1);
	bus_lcd_write_cmd(node, (0xF003));
	bus_lcd_write_data(node, "\x08", 1);
	bus_lcd_write_cmd(node, (0xF004));
	bus_lcd_write_data(node, "\x01", 1);

	bus_lcd_write_cmd(node, (0xC500));
	bus_lcd_read_data(node, (u8*)tmp, 1);
	bus_lcd_write_cmd(node, (0xC501));
	bus_lcd_read_data(node, (u8*)&tmp[1], 1);
	
	data = tmp[0]; 
	data<<=8;
	data |= tmp[1];

	NT35510_DEBUG(LOG_DEBUG, "read reg:%04x\r\n", data);

	if(data != TftLcdNT35510Drv.id)
	{
		NT35510_DEBUG(LOG_DEBUG, "lcd drive no 9341\r\n");	
		bus_lcd_close(node);
		return -1;
	}

	i = 0;
	while(1) {
		if(i >= sizeof(NT35510_Init_List)/sizeof(NT35510_Init_CMD))
			break;

		bus_lcd_write_cmd(node, NT35510_Init_List[i].cmd);
		bus_lcd_write_data(node, &(NT35510_Init_List[i].data), 1);
		i++;
	}
	bus_lcd_write_cmd(node, (0x1100));
	Delay(120); 
	bus_lcd_write_cmd(node, (0x2900));
	bus_lcd_write_cmd(node, (0x2c00));

	bus_lcd_close(node);
	
	Delay(50);
	
	return 0;
}
/**
 *@brief:      drv_NT35510_xy2cp
 *@details:    将xy坐标转换为CP坐标
 *@param[in]   无
 *@param[out]  无
 *@retval:     
 */
s32 drv_NT35510_xy2cp(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey, u16 *sc, u16 *ec, u16 *sp, u16 *ep)
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
 *@brief:      drv_NT35510_drawpoint
 *@details:    画点
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  无
 *@retval:     static
 */
static s32 drv_NT35510_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	u16 sc,ec,sp,ep;

	drv_NT35510_xy2cp(lcd, x, x, y, y, &sc,&ec,&sp,&ep);
	drv_NT35510_set_cp_addr(lcd, sc, ec, sp, ep);

	DevLcdBusNode * node;
	node = bus_lcd_open(lcd->dev.buslcd);
	
	u16 tmp[2];
	tmp[0] = color;
	bus_lcd_write_data(node, (u8*)tmp, 1);
	bus_lcd_close(node);
 
	return 0;
}
/**
 *@brief:      drv_NT35510_color_fill
 *@details:    将一块区域设定为某种颜色
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  无
 *@retval:     
 */
s32 drv_NT35510_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{

	u16 height,width;
	u16 i,j;
	u16 hsa,hea,vsa,vea;

	drv_NT35510_xy2cp(lcd, sx, ex, sy, ey, &hsa,&hea,&vsa,&vea);
	drv_NT35510_set_cp_addr(lcd, hsa, hea, vsa, vea);

	width = hea - hsa + 1;//得到填充的宽度
	height = vea - vsa + 1;//高度
	
	//uart_printf("ili9325 width:%d, height:%d\r\n", width, height);
	
	DevLcdBusNode * node;
	u32 cnt;
	
	cnt = height*width;
	
	node = bus_lcd_open(lcd->dev.buslcd);
	
	bus_lcd_w_data(node, color, cnt);

	bus_lcd_close(node);
	return 0;

}

/**
 *@brief:      drv_NT35510_color_fill
 *@details:    填充矩形区域
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  每一个点的颜色数据
 *@param[out]  无
 *@retval:     
 */
s32 drv_NT35510_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{

	u16 height,width;
	u32 i,j;
	u16 sc,ec,sp,ep;

	drv_NT35510_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	drv_NT35510_set_cp_addr(lcd, sc, ec, sp, ep);

	width=(ec+1)-sc;
	height=(ep+1)-sp;

	wjq_log(LOG_DEBUG, "fill width:%d, height:%d\r\n", width, height);
	
	DevLcdBusNode * node;

	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_write_data(node, (u8 *)color, height*width);	
	bus_lcd_close(node);	 
	
	return 0;

} 

s32 drv_NT35510_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	u16 sc,ec,sp,ep;
	
	wjq_log(LOG_DEBUG, "XY:-%d-%d-%d-%d-\r\n", sx, ex, sy, ey);
	drv_NT35510_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	wjq_log(LOG_DEBUG, "cp:-%d-%d-%d-%d-\r\n", sc, ec, sp, ep);
	drv_NT35510_set_cp_addr(lcd, sc, ec, sp, ep);	
	return 0;
}

s32 drv_NT35510_flush(DevLcdNode *lcd, u16 *color, u32 len)
{
	lcd->busnode = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_write_data(lcd->busnode, (u8 *)color,  len);	
	bus_lcd_close(lcd->busnode);
	return 0;
} 
s32 drv_NT35510_update(DevLcdNode *lcd)
{
	return 0;	
}

#endif


