/**
 * @file            font.c
 * @brief           字库管理
 * @author          wujique
 * @date            2018年3月2日 星期五
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年3月2日 星期五
 *   作    者:       屋脊雀工作室
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
#include "log.h"
#include "vfs.h"
#include "board_sysconf.h"
#include "font.h"
/*
	最好的方案其实是让字库自举，也就是在字库的头部包含字库信息。
	暂时不做这么复杂了。
*/

/*

	asc字符点阵，使用横库。
	ASC字库内置在源代码中。
	FontAscList的定义要跟汉字库定义排列一致
*/
const struct fbcon_font_desc *FontAscList[FONT_LIST_MAX]=
		{
			&font_vga_8x16,
			&font_vga_6x12,
			&font_vga_8x16,
			&font_vga_6x12
		};

s32 FontInit = -1;		

int FontFd[FONT_LIST_MAX];

u8 font_find_index(char *font)
{
	u8 i =0;

	while(1)
	{
		if(0 == strcmp(FontList[i]->name, font))
        {
        	return i;
		}
		
		i++;
		if(i>=FONT_LIST_MAX)
			break;
	}
	return i;
	
}
/**
 *@brief:      font_check_hzfont
 *@details:    检查字库，主要是汉字库
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 font_hzfont_init(void)
{
	u8 i;

	i = 0;
	while(1)
	{
		if(i>= FONT_LIST_MAX)
			break;

		wjq_log(LOG_INFO, "font:%s , path:%s,", FontList[i]->name, FontList[i]->path);
		
		FontFd[i] = vfs_open(FontList[i]->path, O_RDONLY);
		
		if(FontFd[i] == NULL)
		{
			wjq_log(LOG_INFO, "err\r\n");
		}
		else
		{
			wjq_log(LOG_INFO, "ok!\r\n");	
		}
		
		i++;
	}

	FontInit = 0;
	return 0;
}

/**
 *@brief:      font_get_hz
 *@details:    获取汉字点阵
 *@param[in]   FontType type  
               char *ch       
               char *buf      
 *@param[out]  无
 *@retval:     
 */
s32 font_get_hz(char *font, u8 *ch, u8 *buf)
{
	unsigned int len;
	u32 addr;
	u8 hcode,lcode;
	int res;
	u8 i = 0;

	if(FontInit == -1)
		font_hzfont_init();

	i = font_find_index(font);
	
	if(i >= FONT_LIST_MAX)
		return -1;
	
	hcode = *ch;
	lcode = *(ch+1);
	
	if((hcode < 0x81) 
		|| (hcode > 0xfe)
		)
	{
		//uart_printf("no china hz\r\n");
		return -1;
	}

	//uart_printf("hz code:%02x, %02x\r\n", hcode, lcode);

	addr = (hcode-0x81)*190;
	if(lcode<0x7f)
	{
		addr = addr+lcode-0x40;	
	}
	else
	{
		addr = addr+lcode-0x41;	
	}
	addr = addr*FontList[i]->size;
	//uart_printf("adr:%08x\r\n", addr);
	
	vfs_lseek(FontFd[i], addr, SEEK_SET);
	res = vfs_read(FontFd[i], (const void *)buf, FontList[i]->size);
	

	if(res != FontList[i]->size)
	{
		//uart_printf("font read err\r\n");
		return -1;
	}
	
	return 0;
	
}
/**
 *@brief:      font_get_asc
 *@details:    获取ASC字符点阵数据
 *@param[in]   FontType type  
               char *ch       
               char *buf      
 *@param[out]  无
 *@retval:     
 */
s32 font_get_asc(char *font, u8 *ch, u8 *buf)
{
	u8* fp;
	u8 i;
	
	if(*ch >= 0x80)
		return -1;
	
	if(FontInit == -1)
			font_hzfont_init();
	
	i = font_find_index(font);
	
	if(i >= FONT_LIST_MAX)
		return -1;

	fp = (u8*)FontAscList[i]->path + (*ch)*FontAscList[i]->size;
	//wjq_log(LOG_DEBUG, "dot data\r\n");
	//PrintFormat(fp, 16);
	
	memcpy(buf, fp, FontAscList[i]->size);

	return 0;
}
/**
 *@brief:      font_get_hw
 *@details:    获取字体长宽
 *@param[in]   FontType type  
               u8 *h       
               u8 *w      
 *@param[out]  无
 *@retval:     
 			返回的是单个字符长宽，也就是对应的ASC宽度，汉字算两个字符宽度
 */

s32 font_get_hw(char *font, u16 *h, u16 *w)
{
	u8 i;
	
	if(FontInit == -1)
		font_hzfont_init();

	i = font_find_index(font);
	
	if(i >= FONT_LIST_MAX)
		return -1;

	*w = FontAscList[i]->width;
	*h = FontAscList[i]->height;

	return 0;
}

