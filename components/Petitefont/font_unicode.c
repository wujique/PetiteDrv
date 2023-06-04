/*
	本文件支持的字库：
	1 字库是用屋脊雀自主开发的工具生成，不是全字库，是需要哪个字符就包含哪个字符
	2 字库结构4部分：
		1.block0: 头部说明,指明类型,版本，对应：struct _StrBitmapFontHead
		2.block1: 按序排列的unicode表，每个字符的unicode占两个自己
		3.block2: 和unicode表顺序一致的bitmap参数表，对应struct _strBitmapHead
		4.block3: 字符bitmap数据，也就是点阵，字节数不定。

	所谓的bitmap，指用freetype将矢量字体渲染后得到的点阵数据
	
	本文件功能，根据输入的unicode码，获取字库文件读点阵的偏移和数据长度

	*/

#include "mcu.h"
#include "petite_config.h"
#include "petite_font.h"
#include "log.h"
#include "board_sysconf.h"
#include "vfs.h"
#include "petite.h"
#include "drv_config.h"


/**
 * @brief    bitmap字库头
 * 
 * 
 */
struct _StrBitmapFontHead{
	char type[32];
	char ver[32];
	/* 字符宽度和高度，在进行渲染时设置freetype的参数
		需要注意的是，转换后得到的bitmap不一定是这个W和H*/
	int pixelw;
	int pixelh;
	int total;///字符总个数
	int unicode_tab;///unicode表偏移位置
	int bitmap_head;///bitmap参数偏移位置，每个字符都有一个_strBitmapHead，长度固定
	int bitmap;///字符点阵数据在文件中的起始位置

	int rev[16-6];
	
};

/**
 * @brief   每个字符的bitmap都需要一个说明
 * @note    长度固定
 * 
 */
struct _strBitmapHead{
	uint16_t advance;	// 字宽
	
	uint16_t rows;	//bitmap行数
	uint16_t width;//bitmap宽度，单位像素
	uint16_t pitch;//每row占字节数
	int16_t left;//bitmap距离左边的距离，单位像素，可能为负数
	int16_t top;//bitmap右上角距离基线的距离，可能为负数，例如下划线'_'
	/*	读bitmap数据的偏移地址  	 */
	uint32_t index;
};
/**
 * @brief   描述
 * 
 * 
 */
typedef struct _sBitmapFontDsc{
	PetiteFontDsc pfDsc;
	
	struct _StrBitmapFontHead BitmapFontHead;
	uint32_t lettercache;
	struct _strBitmapHead headcache;
}BitmapFontDsc;

/**
	根据unicode内码取点阵的方法
	1 从block1:unicode tab中查询，得到位置索引------多次读文件-----是否可以用二分法提高速度？
	2 使用位置索引从block2:bitmap_head中读取bitmap头信息
		从而的到bitmap数据偏移和bitmap数据长度
	3 从block3：bitmap data中读取点阵数据
*/
/**
 * @brief   获取指定字符的bitmap头
 * 
 * @param   dsc         参数描述
 * @param   unicode     字符unicode 16 码
 * @return  int 
 * 
 */
static int bitmapfont_get_bitmaphead(BitmapFontDsc *dsc, uint16_t unicode)
{

	int rlen = 0;
	static uint16_t uindex;
	uint16_t uindex_f, uindex_l;
	uint16_t utmp;

	if (dsc->lettercache == unicode) return uindex;

	uindex_f = 0;
	uindex_l = dsc->BitmapFontHead.total-1;

	PetiteFontDsc *pfdsc;
	pfdsc = (PetiteFontDsc *)dsc;
	
	//uart_printf("unicode: %04x\r\n", unicode);
	while(uindex_f <= uindex_l){
		uindex = uindex_f + (uindex_l - uindex_f)/2;
		vfs_lseek(pfdsc->fd, dsc->BitmapFontHead.unicode_tab + uindex*2, 0);
		rlen = vfs_read(pfdsc->fd, &utmp, 2);
		if(utmp == unicode){

			//uart_printf("index: %d\r\n\r\n", uindex);
			vfs_lseek(pfdsc->fd, dsc->BitmapFontHead.bitmap_head + uindex*sizeof(struct _strBitmapHead), 0);
			rlen = vfs_read(pfdsc->fd, (void *)&(dsc->headcache), sizeof(struct _strBitmapHead));
			
			dsc->lettercache = unicode;
			
			return uindex;
		}else if(utmp > unicode) {
			uindex_l = uindex-1;
		} else {
			uindex_f = uindex+1;
		}
	}
	//uart_printf("uindex %d\r\n", uindex);

	return -1;
}


/**
 * @brief   描述
 * 
 * @param   pfont       参数描述
 * @param   glyph       参数描述
 * @param   letter      参数描述
 * @param   letter_next 参数描述
 * @return  bool 
 * 
 */
bool bitmapfont_get_glyph_dsc(const struct _petite_font_t *pfont, petite_font_glyph_dsc_t *glyph, uint32_t letter, uint32_t letter_next)
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

	BitmapFontDsc *dsc;
	dsc = (BitmapFontDsc *)pfont->dsc;
	int index;
	index = bitmapfont_get_bitmaphead(dsc, letter);
	if ( index == -1) {
		return false;
	}
	
	struct _strBitmapHead *head;

	head = &(dsc->headcache);
	
	glyph->adv_w = head->advance;
	glyph->box_w = head->width;
	glyph->box_h = head->rows;
	
	glyph->ofs_x = head->left;
	glyph->ofs_y = pfont->base_line - head->top;

	glyph->bpp = 1;

	return true;
}


const uint8_t *bitmapfont_get_glyph_bitmap(const petite_font_t * pfont, uint32_t letter)
{
	uint8_t *dotbuf;

	if(letter <0x80) {
		return pfont->fallback->get_glyph_bitmap(pfont->fallback, letter);
	}
	
	BitmapFontDsc *dsc;
	PetiteFontDsc *pfdsc;
	int uindex;
	int rlen = 0;
	int index;
	
	dsc = (BitmapFontDsc *)pfont->dsc;
	pfdsc = (PetiteFontDsc *)dsc;
	
	index = bitmapfont_get_bitmaphead(dsc, letter);
	if ( index == -1) {
		return NULL;
	}

	struct _strBitmapHead *head;
	head = &(dsc->headcache);

	uint16_t bitmap_size;
	bitmap_size = head->pitch*head->rows;

	if (pfdsc->bmsize < bitmap_size) {

		wjq_free(pfdsc->bitmap);

		pfdsc->bitmap = (void *)wjq_malloc(bitmap_size);
		pfdsc->bmsize = bitmap_size;
	}
	dotbuf = pfdsc->bitmap;
	
	/* 读数据 */
	vfs_lseek(pfdsc->fd, dsc->BitmapFontHead.bitmap +  head->index, 0);
	rlen = vfs_read(pfdsc->fd, dotbuf, bitmap_size);

	//PrintFormat(dotbuf, bitmap_size);

	return (const uint8_t *)dotbuf;

}

extern petite_font_t FontAsc6X12;
extern petite_font_t FontVGA8x16;

petite_font_t *bitmapfont_create_from_file(const char * path, uint16_t line_height)
{
	BitmapFontDsc *dsc;
	PetiteFontDsc *pfdsc;
	int rlen = 0;
	
	dsc = (BitmapFontDsc *)wjq_malloc(sizeof(BitmapFontDsc));
	if (dsc == NULL){
		return NULL;
	}

	pfdsc = (PetiteFontDsc *)dsc;

	pfdsc->fd = vfs_open(path , O_RDONLY);
	if (pfdsc->fd == NULL) {
		wjq_free(dsc);
		return NULL;
	}
	/* 读头 */
	vfs_read(pfdsc->fd, &dsc->BitmapFontHead, sizeof(struct _StrBitmapFontHead));
	uart_printf("rlen:%d\r\n", rlen);
	uart_printf("%s\r\n", dsc->BitmapFontHead.type);
	uart_printf("%s\r\n", dsc->BitmapFontHead.ver);

	uart_printf("shift:\r\n");
	uart_printf("\t pixelw:%d\r\n", dsc->BitmapFontHead.pixelw);
	uart_printf("\t pixelh:%d\r\n", dsc->BitmapFontHead.pixelh);
	uart_printf("\t total:%d\r\n", dsc->BitmapFontHead.total);
	uart_printf("\t unicode_tab:%d\r\n", dsc->BitmapFontHead.unicode_tab);
	uart_printf("\t bitmap_head:%d\r\n", dsc->BitmapFontHead.bitmap_head);
	uart_printf("\t bitmap:%d\r\n", dsc->BitmapFontHead.bitmap);

	petite_font_t *pfont;

	pfont = (petite_font_t *)wjq_malloc(sizeof(petite_font_t));
	if (pfont == NULL) {
		wjq_free(dsc);
		return NULL;
	}

	pfont->get_glyph_dsc = bitmapfont_get_glyph_dsc;
	pfont->get_glyph_bitmap = bitmapfont_get_glyph_bitmap;

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
	
	pfont ->fallback = &FontAsc6X12;

	uint16_t bitmap_size;
	bitmap_size = ((line_height+7)/8)*line_height;

	pfdsc->bitmap = (void *)wjq_malloc(bitmap_size);
	pfdsc->bmsize = bitmap_size;

	pfdsc->fdt = FONT_H_H_L_R_U_D;
	pfdsc->st = FONT_ST_UNICODE;
	pfdsc->stc = NULL;
	
	dsc->lettercache = NULL;
	pfont->dsc = dsc;
	
	if (line_height == 12) {
		pfont ->fallback = &FontAsc6X12;
	} else if(line_height == 16 || line_height == 24) {
		pfont ->fallback = &FontVGA8x16;
	}

	wjq_log(LOG_DEBUG, "wjq font create success!\r\n");

	return pfont;
}


