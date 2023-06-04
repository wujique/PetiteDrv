/**
 * @file    font_wjq.c
 * @brief   屋脊雀工作室制作的GBK等宽高点阵取点算法
 * 
 * @author  wujique(xxx@wujique.com)
 * @version 0.1
 * @date    2023-06-04
 * @copyright Copyright (c) 2023..
 * @note    屋脊雀工作室工具转换的点阵
 *			中文、GBK编码、不带ASC字符、不支持4字节汉字
 *			ASC字符需要用内置字符
 */

#include "mcu.h"
#include "petite_config.h"
#include "petite_font.h"
#include "mem/p_malloc.h"
#include "log.h"
#include "board_sysconf.h"

#include "vfs.h"

typedef struct _sFontWjqDsc{
	PetiteFontDsc pfDsc;
}FontWjqDsc;



static int32_t font_index_gbk_wjq(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4)
{
	return (c2-0x81)*197 + (c1-0x40);
}



bool font_wjq_get_glyph_dsc(const struct _petite_font_t *pfont, petite_font_glyph_dsc_t *glyph, uint32_t letter, uint32_t letter_next)
{
	bool ret;
	
	if(letter <0x80) {
		ret = pfont->fallback->get_glyph_dsc(pfont->fallback, glyph, letter, letter_next);
		if (ret == true) {
			/* 同步基线 */
			glyph->ofs_y += pfont->base_line - pfont->fallback->base_line;
		}
		return ret;
	}

	glyph->adv_w = pfont->line_height;
	glyph->box_w = pfont->line_height;

	glyph->box_h = pfont->line_height;
	
	glyph->ofs_x = 0;
	glyph->ofs_y = 0;

	glyph->bpp = 1;

	return true;
}


const uint8_t *font_wjq_get_glyph_bitmap(const petite_font_t * pfont, uint32_t letter)
{
	uint32_t addr;
	uint8_t *dotbuf;
	uint16_t bitmap_size;
	
	if(letter <0x80) {
		return pfont->fallback->get_glyph_bitmap(pfont->fallback, letter);
	}

	unsigned char c1, c2;

	if (letter > 0xffff) return NULL;

	c1 = (letter&0xff);
	c2 = (letter>>8)&0xff;

	FontWjqDsc *dsc;
	PetiteFontDsc *pfdsc;
	
	dsc = (FontWjqDsc *)pfont->dsc;
	pfdsc = (PetiteFontDsc *)dsc;
	/* 找到地址 */
	bitmap_size = ((pfont->line_height+7)/8*(pfont->line_height));
	//uart_printf("bitmap size:%d\r\n", bitmap_size);
	addr = font_index_gbk_wjq(c1, c2, 0, 0);
	//uart_printf("addr :%d\r\n", addr);
	addr = addr * bitmap_size;
	//uart_printf("addr :%d\r\n", addr);
	
	dotbuf = pfdsc->bitmap;

	int res;
	
	res = vfs_lseek(pfdsc->fd, addr, SEEK_SET);
	res = vfs_read(pfdsc->fd, dotbuf, bitmap_size);

	if (res <= 0) {
		uart_printf("wjq read font err!\r\n");	
	}

	//PrintFormat(dotbuf, bitmap_size);

	return (const uint8_t *)dotbuf;

}


petite_font_t *font_wjq_create_from_file(const char * path, uint16_t line_height)
{
	PetiteFontDsc *pfdsc;

	
	FontWjqDsc *dsc;
	dsc = (FontWjqDsc *)wjq_malloc(sizeof(FontWjqDsc));
	if (dsc == NULL){
		return NULL;
	}

	pfdsc = (PetiteFontDsc *)dsc;
	pfdsc->fd = vfs_open(path , O_RDONLY);
	if (pfdsc->fd == NULL) {
		wjq_free(dsc);
		return NULL;
	}

	petite_font_t *pfont;

	pfont = (petite_font_t *)wjq_malloc(sizeof(petite_font_t));
	if (pfont == NULL) {
		wjq_free(dsc);
		return NULL;
	}

	pfont->get_glyph_dsc = font_wjq_get_glyph_dsc;
	pfont->get_glyph_bitmap = font_wjq_get_glyph_bitmap;

	pfont->line_height = line_height;
	
	if (line_height == 12) {
		pfont->base_line = 10;
		pfont->underline_position = 11;
	} else if (line_height == 16) {
		pfont->base_line = 14;
		pfont->underline_position = 15;
	} else if (pfont->base_line == 24) {
		pfont->base_line = 20;
		pfont->underline_position = 23;
	} else {
		pfont->base_line = line_height*10/12;
		pfont->underline_position = line_height -1;
	}
	pfont->subpx = 0;	
	pfont->underline_thickness = 1;
	pfont->dsc = dsc;
	pfont ->fallback = &FontAsc6X12;


	uint16_t bitmap_size;
	bitmap_size = ((line_height+7)/8)*line_height;

	pfdsc->bitmap = (void *)wjq_malloc(bitmap_size);
	pfdsc->bmsize = bitmap_size;

	pfdsc->fdt = FONT_H_H_L_R_U_D;
	pfdsc->st = FONT_ST_GBK;
	pfdsc->stc = FONT_DOT_WJQ;
	
	/* 设置关联ASC字库*/
	if (line_height == 12) {
		pfont ->fallback = &FontAsc6X12;
	} else if(line_height == 16 || line_height == 24) {
		pfont ->fallback = &FontVGA8x16;
	}

	wjq_log(LOG_DEBUG, "wjq font create success!\r\n");

	return pfont;
}




