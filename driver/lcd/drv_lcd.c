/**
 * @file            dev_lcd.c
 * @brief           LCD 中间层
 * @author          wujique
 * @date            2018年4月17日 星期二
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年4月17日 星期二
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
#include "mcu.h"
#include "petite_config.h"
#include "petite.h"
#include "board_sysconf.h"

#include "drv_lcd.h"

#include "drv_ILI9341.h"
#include "drv_str7565.h"
#include "drv_IL91874.h"
#include "drv_IL3820.h"
#include "drv_ILI9325.h"
#include "drv_st7789.h"
#include "drv_st7735r.h"
#include "drv_nt35510.h"
#include "drv_r61408.h"

//#define DEV_LCD_DEBUG

#ifdef DEV_LCD_DEBUG
#define LCD_DEBUG	wjq_log 
#else
#define LCD_DEBUG(a, ...)
#endif

/*	LCD驱动列表 */
_lcd_drv *LcdDrvList[] = {
				/* tft lcd ILI9341 */
				#if( LCD_DRIVER_9341 == 1 )
					&TftLcdILI9341Drv,   
				#endif
				/* tft lcd ILI9325 */
				#if( LCD_DRIVER_9325 == 1 )	
					&TftLcdILI9325Drv,
				#endif
				/* COG LCD  */
				#if( LCD_DRIVER_ST7565 == 1 )
					&CogLcdST7565Drv,
				#endif
				/* oled  */
				#if( LCD_DRIVER_SSD1615 == 1 )
					&OledLcdSSD1615rv,
				#endif
				/* tft lcd, 不是并口，SPI 或I2C 或8BIT  */
				#if( LCD_DRIVER_9341_8BIT == 1 )
					&TftLcdILI9341_8_Drv,
				#endif
				/* 1.44寸 中景园 tft lcd  */
				#if( LCD_DRIVER_7735 == 1 )
					&TftLcdST7735R_Drv,
				#endif
				/* 1.3寸，IPS，中景园，只有SCL&SDA的SPI接口LCD*/
				#if( LCD_DRIVER_7789 == 1 )
					&TftLcdST7789_Drv,
				#endif
				/*91874 大连佳显 2.7寸电子纸 三色屏*/
				#if( LCD_DRIVER_91874 == 1 )
					&TftLcdIL91874Drv,
				#endif
				/*3820 大连佳显 GDEH154D27*/
				#if( LCD_DRIVER_3820 == 1 )	
					&TftLcdIL3820Drv,
				#endif

				/*全动 NT35510 */
				#if( LCD_DRIVER_NT35510 == 1 )	
					&TftLcdNT35510Drv,
				#endif
				/* 好矩润 R61408*/
				#if( LCD_DRIVER_R61408 == 1 )	
					&TftLcdR61408Drv,
				#endif
				
				NULL,	
};
/*	可自动识别ID的驱动*/
_lcd_drv *LcdProbDrvList[] = {
				#if( LCD_DRIVER_9341 == 1 )
					&TftLcdILI9341Drv,
				#endif
				
				#if( LCD_DRIVER_9325 == 1 )	
					&TftLcdILI9325Drv,
				#endif
				
				/*全动 NT35510 */
				#if( LCD_DRIVER_NT35510 == 1 )	
					&TftLcdNT35510Drv,
				#endif

				#if( LCD_DRIVER_R61408 == 1 )	
					&TftLcdR61408Drv,
				#endif
				
				NULL,
};
/**
 *@brief:      dev_lcd_finddrv
 *@details:    根据ID查找设备驱动
 *@param[in]   u16 id  
 *@param[out]  无
 *@retval:     _lcd_drv
 */
static _lcd_drv *lcd_finddrv(u16 id)
{
	u8 i =0;
	
	while(1){
		if(LcdDrvList[i] == NULL) return NULL;
		
		if(LcdDrvList[i]->id == id){
			return LcdDrvList[i];
		}
		i++;
	}
}

struct list_head DevLcdRoot = {&DevLcdRoot, &DevLcdRoot};	
/**
 *@brief:      dev_lcd_register
 *@details:    注册LCD设备
 *@param[in]   
 *@param[out]  
 *@retval:     
 */
s32 lcd_dev_register(const DevLcd *dev)
{
	struct list_head *listp;
	DevLcdNode *lcdnode;
	s32 ret;
	
	listp = DevLcdRoot.next;
	while(1) {
		if (listp == &DevLcdRoot) break;

		lcdnode = list_entry(listp, DevLcdNode, list);

		if (strcmp(dev->pnode.name, lcdnode->dev.pnode.name) == 0) {
			wjq_log(LOG_ERR, "lcd dev name err!\r\n");
			return -1;
		}
	
		listp = listp->next;
	}

	/*  申请一个节点空间      	*/
	lcdnode = (DevLcdNode *)wjq_malloc(sizeof(DevLcdNode));
	list_add(&(lcdnode->list), &DevLcdRoot);
	
	/*复制设备信息*/
	memcpy((u8 *)&lcdnode->dev, (u8 *)dev, sizeof(DevLcd));
	lcdnode->gd = -1;
	lcdnode->fb = 0;

	/* 进行bus初始化 */
	bus_lcd_IO_init(dev->bus);
	
	if (dev->id == NULL) {
		/* 没有指定lcd id，通过porb 初始化 */
		LCD_DEBUG(LOG_DEBUG, "prob LCD id\r\n");

		u8 j = 0;
		ret = -1;
		while(1) {
			/* 简单粗暴的方法，尝试初始化*/
			if (LcdProbDrvList[j] == NULL) break;
			
			ret = LcdProbDrvList[j]->init(lcdnode);
			if (ret == 0) {
				LCD_DEBUG(LOG_DEBUG, "lcd drv prob ok!\r\n");	
				lcdnode->drv = LcdProbDrvList[j];
				break;
			}

			j++;
		}
	}else {
		/* 设备指定了 id，根据id找驱动 */
		
		
		ret = -1;
		lcdnode->drv = lcd_finddrv(dev->id);
		if (lcdnode->drv != NULL) {
			LCD_DEBUG(LOG_DEBUG, "find lcd drv, id:%04x...suc\r\n", dev->id);
			ret = lcdnode->drv->init(lcdnode);
		} else {
			LCD_DEBUG(LOG_WAR, "find lcd drv, id:%04x...fail\r\n", dev->id);
		}
	}

	if(ret == 0) {
		lcdnode->gd = -1;
		
		lcdnode->dir = H_LCD;
		
		lcdnode->height = lcdnode->dev.height;
		lcdnode->width = lcdnode->dev.width;
		
		lcd_setdir(lcdnode, W_LCD, L2R_U2D);
		
		lcdnode->drv->onoff((lcdnode),1);
		lcdnode->drv->color_fill(lcdnode, 0, lcdnode->width, 0, lcdnode->height, BLUE);
		lcdnode->drv->update(lcdnode);
		lcdnode->drv->backlight(lcdnode, 1);
		wjq_log(LOG_INFO, "lcd init OK\r\n");
	} else {
		lcdnode->gd = -2;
		wjq_log(LOG_ERR, "lcd drv init err!\r\n");
	}
	
	return 0;
}


/**
 *@brief:      dev_lcd_open
 *@details:    打开LCD
 *@param[in]   char *name  
 *@param[out]  无
 *@retval:     DevLcd
 */
DevLcdNode *lcd_open(char *name)
{

	DevLcdNode *node;
	struct list_head *listp;
	
	//LCD_DEBUG(LOG_INFO, "lcd open:%s!\r\n", name);

	listp = DevLcdRoot.next;
	node = NULL;
	
	while(1) {
		if (listp == &DevLcdRoot) break;

		node = list_entry(listp, DevLcdNode, list);
		//LCD_DEBUG(LOG_INFO, "lcd name:%s!\r\n", node->dev.name);
		
		if (strcmp(name, node->dev.pnode.name) == 0) {
			//LCD_DEBUG(LOG_INFO, "lcd dev get ok!\r\n");
			break;
		} else {
			node = NULL;
		}
		
		listp = listp->next;
	}

	if (node != NULL) {
		if (node->gd > (-2)) node->gd++;
		else return NULL;
	}
	
	return node;
}

/**
 *@brief:      dev_lcd_close
 *@details:    关闭LCD
 *@param[in]   DevLcd *dev  
 *@param[out]  无
 *@retval:     
 */
s32 lcd_close(DevLcdNode *node)
{
	if(node->gd <0)	return -1;
	else{
		node->gd -= 1;
		return 0;
	}
}
/*
	坐标-1 是坐标原点的变化，

	在APP层，原点是（1，1），这样更符合常人逻辑。

	到驱动层就换为(0,0)，无论程序还是控制器显存，都是从（0，0）开始

*/
s32 lcd_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	if (lcd == NULL)	return -1;
	
	return lcd->drv->draw_point(lcd, x-1, y-1, color);
}

s32 lcd_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	if(lcd == NULL)	return -1;
	
	return lcd->drv->prepare_display(lcd, sx-1, ex-1, sy-1, ey-1);
}


s32 lcd_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{	
	if(lcd == NULL)	return -1;
	
	return lcd->drv->fill(lcd, sx-1,ex-1,sy-1,ey-1,color);
}
s32 lcd_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{
	if(lcd == NULL)	return -1;
	
	return lcd->drv->color_fill(lcd, sx-1,ex-1,sy-1,ey-1,color);
}
s32 lcd_backlight(DevLcdNode *lcd, u8 sta)
{
	if(lcd == NULL)	return -1;
	
	lcd->drv->backlight(lcd, sta);
	return 0;
}
s32 lcd_display_onoff(DevLcdNode *lcd, u8 sta)
{
	if(lcd == NULL)	return -1;

	return lcd->drv->onoff(lcd, sta);
}

s32 lcd_flush(DevLcdNode *lcd, u16 *color, u32 len)
{
	if(lcd == NULL)	return -1;

	return lcd->drv->flush(lcd, color, len);	
}

s32 lcd_update(DevLcdNode *lcd)
{
	if(lcd == NULL)	return -1;

	return lcd->drv->update(lcd);
}
/**
 *@brief:      dev_lcd_setdir
 *@details:    设置横屏或竖屏，扫描方向
 *@param[in]   u8 dir       0 竖屏1横屏
               u8 scan_dir  参考宏定义L2R_U2D       
 *@param[out]  无
 *@retval:     
 */
s32 lcd_setdir(DevLcdNode *node, u8 dir, u8 scan_dir)
{
	u16 temp;
	u8 scan_dir_tmp;

	if (node == NULL) return -1;

	//切换屏幕方向	
	if (dir != node->dir) {
		
		node->dir = node->dir^0x01;
		temp = node->width;
		node->width = node->height;
		node->height = temp;
		LCD_DEBUG(LOG_DEBUG, "set dir w:%d, h:%d\r\n", node->width, node->height);
	}
	
	//横屏，扫描方向映射转换
	if(node->dir == W_LCD) {
		/*
			横屏	 竖屏
			LR----UD
			RL----DU
			UD----RL
			DU----LR
			UDLR----LRUD
		*/
		scan_dir_tmp = 0;
		if ((scan_dir&LRUD_BIT_MASK) == 0) {
			scan_dir_tmp += LRUD_BIT_MASK;
		}

		if ((scan_dir&LR_BIT_MASK) == LR_BIT_MASK) {
			scan_dir_tmp += UD_BIT_MASK;	
		}

		if ((scan_dir&UD_BIT_MASK) == 0) {
			scan_dir_tmp += LR_BIT_MASK;
		}
	}else {
		scan_dir_tmp = scan_dir;
	}
	
	node->scandir = scan_dir_tmp;
	
	node->drv->set_dir(node, node->scandir);
	
	return 0;
}

#if 0
/**
 *@brief:      dev_lcd_test
 *@details:    LCD测试函数
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void dev_lcd_test(void)
{
	DevLcdNode *LcdCog = NULL;
	DevLcdNode *LcdOled = NULL;
	DevLcdNode *LcdOledI2C = NULL;
	DevLcdNode *LcdTft = NULL;

	/*  打开三个设备 */
	LcdCog = dev_lcd_open("spicoglcd");
	if (LcdCog==NULL)
		wjq_log(LOG_FUN, "open cog lcd err\r\n");

	LcdOled = dev_lcd_open("vspioledlcd");
	if (LcdOled==NULL)
		wjq_log(LOG_FUN, "open oled lcd err\r\n");
	
	LcdTft = dev_lcd_open("tftlcd");
	if (LcdTft==NULL)
		wjq_log(LOG_FUN, "open tft lcd err\r\n");

	LcdOledI2C = dev_lcd_open("i2coledlcd");
	if (LcdOledI2C==NULL)
		wjq_log(LOG_FUN, "open oled i2c lcd err\r\n");
	
	/*打开背光*/
	dev_lcd_backlight(LcdCog, 1);
	dev_lcd_backlight(LcdOled, 1);
	dev_lcd_backlight(LcdOledI2C, 1);
	dev_lcd_backlight(LcdTft, 1);

	#if 0/*不支持汉字时*/
	put_string(LcdCog, 5, 5, "spi cog lcd", BLACK);
	put_string(LcdOled, 5, 5, "vspi oled lcd", BLACK);
	put_string(LcdOledI2C, 5, 5, "i2c oled lcd", BLACK);
	put_string(LcdTft, 5, 5, "2.8 tft lcd", BLACK);
	#endif

	#if 1
	dev_lcd_put_string(LcdOled, "songti12", 10,1, "ABC-abc，", BLACK);
	dev_lcd_put_string(LcdOled, "siyuan16", 1, 13, "这是oled lcd", BLACK);
	dev_lcd_put_string(LcdOled, "songti12", 10,30, "www.wujique.com", BLACK);
	dev_lcd_put_string(LcdOled, "siyuan16", 1, 47, "屋脊雀工作室", BLACK);
	dev_lcd_update(LcdOled);
	dev_lcd_put_string(LcdCog, "songti12", 10,1, "ABC-abc，", BLACK);
	dev_lcd_put_string(LcdCog, "siyuan16", 1, 13, "这是cog lcd", BLACK);
	dev_lcd_put_string(LcdCog, "songti12", 10,30, "www.wujique.com", BLACK);
	dev_lcd_put_string(LcdCog, "siyuan16", 1, 47, "屋脊雀工作室", BLACK);
	dev_lcd_update(LcdCog);
	dev_lcd_put_string(LcdTft, "songti12", 20,30, "ABC-abc，", RED);
	dev_lcd_put_string(LcdTft, "siyuan16", 20,60, "这是tft lcd", RED);
	dev_lcd_put_string(LcdTft, "songti12", 20,100, "www.wujique.com", RED);
	dev_lcd_put_string(LcdTft, "siyuan16", 20,150, "屋脊雀工作室", RED);
	dev_lcd_update(LcdTft);
	dev_lcd_put_string(LcdOledI2C, "songti12", 10,1, "ABC-abc，", BLACK);
	dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,13, "这是LcdOledI2C", BLACK);
	dev_lcd_put_string(LcdOledI2C, "songti12", 10,30, "www.wujique.com", BLACK);
	dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,47, "屋脊雀工作室", BLACK);
	dev_lcd_update(LcdOledI2C);
	#endif
	
	while(1);
}



void dev_i2coledlcd_test(void)
{

	DevLcdNode *LcdOledI2C = NULL;

	wjq_log(LOG_FUN, "dev_i2coledlcd_test\r\n");

	LcdOledI2C = dev_lcd_open("i2coledlcd");
	if (LcdOledI2C==NULL)
		wjq_log(LOG_FUN, "open oled i2c lcd err\r\n");
	else
		wjq_log(LOG_FUN, "open oled i2c lcd suc\r\n");
	/*打开背光*/
	dev_lcd_backlight(LcdOledI2C, 1);

	dev_lcd_put_string(LcdOledI2C, "songti12", 10,1, "ABC-abc，", BLACK);
	dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,13, "这是LcdOledI2C", BLACK);
	dev_lcd_put_string(LcdOledI2C, "songti12", 10,30, "www.wujique.com", BLACK);
	dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,47, "屋脊雀工作室", BLACK);
	dev_lcd_update(LcdOledI2C);

	LcdOledI2C = dev_lcd_open("i2coledlcd2");
	if (LcdOledI2C==NULL)
		wjq_log(LOG_FUN, "open oled i2c2 lcd err\r\n");
	
	/*打开背光*/
	dev_lcd_backlight(LcdOledI2C, 1);
	while(1) {
		dev_lcd_put_string(LcdOledI2C, "songti12", 10,1, "ABC-abc，", BLACK);
		dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,13, "这是LcdOledI2C", BLACK);
		dev_lcd_put_string(LcdOledI2C, "songti12", 10,30, "www.wujique.com", BLACK);
		dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,47, "屋脊雀工作室", BLACK);
		dev_lcd_update(LcdOledI2C);
		Delay(1000);
		dev_lcd_color_fill(LcdOledI2C, 1, 1000, 1, 1000, WHITE);
		dev_lcd_update(LcdOledI2C);
		Delay(1000);
	}

}

#endif


/*----end-----*/


