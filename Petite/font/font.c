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
#include "petite_config.h"
#include "font/font.h"
#include "log.h"
#include "board_sysconf.h"

#include "vfs.h"
/*

	目标：一个获取指定字体点阵的模块、

	情景：
		1. 点阵保存方式：直接存FLASH，文件系统，或者两者共存
		2. 点阵有多种字体（宋体、黑体、）、多种尺寸（1212 1616 2424 ）

	用法：
		1. 配置：如何获取对应字库的数据，读接口。
		2. 输入要读哪种字体，哪种尺寸，字符， 取模方式
		3. 返回 点阵数据结构体：


	备注：这些点阵都是GBK的，所有都是GBK编码
		  如何支持多国语言？ 通过设置codepage？
*/


/*-------------------GBK 共用函数------------------------------*/
/*
	区分字符在GBK哪个区间
输入：
    str:输入的双字节的区位码(两个BYTE)
    C1为首字节 C2是尾字节
返回：
	FontHzArea
*/
static FontHzArea font_gbk_get_area(char c1, char c2)
{
	/* ASC 区*/
	if(c1 < 0x80) return ASC_AREA;
	
	if(c2 == 0x7f) {
		/* 无定义区 */
		return HZ_NO_AREA;
	}
	
	if((c1>=0xA1 && c1 <= 0xA9) 
		&& (c2 >= 0xa1 && c2 <= 0xfe)) {
		/*双字节 1 区， 符号区 (本区码位 846 A1A1—A9FE)*/
		return HZ_DBYTE_1AREA;
	}

	if ((c1>=0xb0 && c1 <= 0xf7)
		&& (c2>=0xa1 && c2 <= 0xfe)) {
		/*双字节 2  区， 汉字区 (本区码位6768 B0A1—F7FE)*/
 		return HZ_DBYTE_2AREA;
	} 

	if((c1 >= 0x81 && c1<= 0xa0)
			&&(c2 >= 0x40 && c2<= 0xfe)){
		/*  双字节 3区， 汉字区 (本区字符6080 8140—A0FE) */
		return HZ_DBYTE_3AREA;
	} 

	if((c1>=0xaa && c2<= 0xa0)
			&&(c2 >= 0x40 && c2<= 0xa0)){
		/*双字节 4 区  汉字区      (本区字符8160 AA40—FEA0)*/
 		return HZ_DBYTE_4AREA;
	} 

	if ((c1>=0xa8 && c1 <= 0xa9 )
	&& (c2 >= 0x40 && c2 <= 0xa0)) {
		/* 双字节 5 符号区 (本区字符192 A840—A9A0)*/
  		return HZ_DBYTE_5AREA;
	}

	if(c2>=0x30 && c2<=0x39) {
		/* Extended Section (With 4 BYTES InCode)    (本区字符6530 0x81308130--0x8439FE39)*/
		return HZ_QBYTE_AREA;
	} 

    return HZ_USER_AREA;
}
/*-------------------------------------------------------------------------------

	如何支持多国语言？ codepage， GBK、big5、其他语言， codepage是应用层的概念？
*/

extern FontHead *FontListN[FONT_LIST_MAX];

struct _FontStr {
	FontHead *head;
	int fd;
};

struct _FontStr FontStr[FONT_LIST_MAX];
/*
	*/
void font_init(void)
{
	u8 fontnum;
	u8 i;

	FontHead *font = NULL;
	
	fontnum = sizeof(FontListN)/sizeof(FontHead *);
	i = 0;
	while(1) {
		if(i >= fontnum) break;
		FontStr[i].head = FontListN[i];
		FontStr[i].fd = NULL;
		i++;
	}	

	return ;
}



/* 查询系统字体 */
void *font_find_font(char *name)
{
	u8 fontnum;
	u8 i;

	struct _FontStr *fontstr = NULL;
	
	fontnum = sizeof(FontListN)/sizeof(FontHead *);
	i = 0;
	while(1) {
		if(i >= fontnum) break;
		if(0 == strcmp(FontListN[i]->name, name)) {
			fontstr = &FontStr[i];
			if(fontstr->fd == NULL) {
				fontstr->fd = vfs_open(FontListN[i]->path, O_RDONLY);
				/* 并且对字库进行初始化 */
			}
			break;
		}
		i++;
	}	

	return (void *)fontstr;
}
/*
	一次只是转换一个字符的点阵
	
	CodeType: "utf-8","GBK","BIG5","unicode"
	
	*/
int font_getdot(void *fontstr_in, char *Ch, FontBitMap *Dot, char *CodeType)
{
	int addr;
	int res;
	u8* fp;
	FontHead *font;
	struct _FontStr *fontstr;
	
	FontHzArea area = ASC_AREA;
	int chr = 1;
	uint32_t chcode = 0x00000042;
	
	fontstr = (struct _FontStr *)fontstr_in;
	font = fontstr->head;

	//uart_printf("codetype:%s\r\n", CodeType);

	if(strcmp(CodeType, "utf-8")==0){
		/* 如果字库是GBK索引，那么就将UTF-8转为gbk*/
	}

	if (strcmp(CodeType, "GBK")==0) {
		/* 这是gbk+asc编码格式的 方法 */
		area = font_gbk_get_area(*Ch, *(Ch+1));
		if(area == ASC_AREA) {
			chcode = *Ch;
			chr = 1;
		} else if(area == HZ_USER_AREA
			||area == HZ_NO_AREA) {
			chcode = *Ch;
			chcode = chcode << 8;
			chcode += *(Ch+1);
			chr = 2;
		} else if(area == HZ_QBYTE_AREA ) {
			chr = 4;
		} else {
			chr = 2;
			chcode = *Ch;
			chcode = chcode << 8;
			chcode += *(Ch+1);
		}
	}
	
	//uart_printf("chcode:%04x\r\n", chcode);
	
	if(area == ASC_AREA) {
		#if 1
		if (font->size == 12) {
			#if 1
			fp = (u8*)(font_vga_6x12.path) + chcode * font_vga_6x12.size;
			memcpy(Dot->dot, fp, font_vga_6x12.size);
			
			Dot->head.rows = font_vga_6x12.height;
			Dot->head.width = font_vga_6x12.width;
			Dot->head.pitch = 1;
			Dot->head.left = 0;
			Dot->head.top = 10;
			#else
			/* 内置的1206英文点阵*/
			font_bmp_getdata(&(Dot->head) ,Dot->dot, chcode);
			#endif
			
			Dot->dt = FONT_H_H_L_R_U_D;
		} else {
			fp = (u8*)(font_vga_8x16.path) + chcode * font_vga_8x16.size;
			memcpy(Dot->dot, fp, font_vga_8x16.size);
		
			Dot->head.rows = font_vga_8x16.height;
			Dot->head.width = font_vga_8x16.width;
			Dot->head.pitch = 1;
			Dot->head.left = 0;
			Dot->head.top = 14;
			
			Dot->dt = FONT_H_H_L_R_U_D;
		}
		#endif
		return chr;
	}


	if(fontstr->fd == NULL) {
		return -1;
	}
	
	/*查询读地址，返回-1， 则说明不存在字符 */
	switch(font->type)
	{
		case FONT_DOT_WJQ:
			addr = font_dot_wjq_addr(font, chcode);
			break;
		case FONT_DOT_ZY:
			//addr = font_dot_zy_addr(font, Ch, area);
			break;

		case FONT_DOT_YMY:
			//addr = font_dot_ymy_addr(font, Ch, area);
			break;
		case FONT_DOT_BMP:
			break;
			
		default:
			addr = -1;
			break;
	}

	if(addr < 0) {return -1;}
	
	res = vfs_lseek(fontstr->fd, addr, SEEK_SET);
	res = vfs_read(fontstr->fd, (const void *)Dot->dot, font->datac);

	Dot->head.rows = font->bitmap.rows;
	Dot->head.width = font->bitmap.width;
	Dot->head.pitch = font->bitmap.pitch;
	Dot->head.left = font->bitmap.left;
	Dot->head.top = font->bitmap.top;
			
	Dot->dt = font->dt;

	return chr;

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
s32 font_get_hw(char *fontname, u16 *h, u16 *w)
{
	FontHead * font;
	int res;
	struct _FontStr *fontstr;
	
	/* 判断系统是否存在字体 并返回字体的信息指针 */
	fontstr = font_find_font(fontname);
	if(fontstr == NULL) {
		/* 返回一个常用值，防止意外*/
		*w = 12;
		*h = 12;
		return -1;
	}
	font = fontstr->head;
	/* 汉字两个字符， 转为1个字符宽度， 后续做codepage兼容再重新设计*/
	*w = font->size/2;
	*h = font->size;

	return 0;
}

/*---------------------------------------------------------------
	重构一个点阵管理系统
	*/

/*
	根据名字打开一个字库文件
	返回字库句柄
	*/
int font_get_font(char *name)
{
	
}
/*
	从字库文件中获bitmap和bitmap参数

	输入的字符的编码？进行编码转换？
	ASC，codepage, GBK, Unicode, BIG5
	codepage还要指定page？
	GBK其实也是cp的其中一个page
	*/
int font_get_bitmap()
{

}


/* 编码 函数 
	GBK unicode, big5
	*/

void font_test(void)
{
/*
	英文所有字符
	
	!"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
*/

}

