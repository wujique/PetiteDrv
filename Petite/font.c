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
#include "font.h"
#include "log.h"
#include "board_sysconf.h"


/* ------------------------------重构-------------------------------------------- */
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


/* 定义一种字体 */
typedef struct _strFontHead
{
	char name[16];//song heiti 
	char size;//字号, 只做等宽字体，汉字12号就是1212， 24号就是2424， ASC12号就是0612，24号就是1224
	char path[64];//字库名字或路径，用于打开
	/* ----  */
	FontS st;//标准 ，主要用于区分18030或GB2312，如果是2312，遇到生僻字，就要从关联字库取
	FontDotType dt;//取模方式
	unsigned char datac;//1汉字点阵数据字节数

	unsigned char w;//宽
	unsigned char h;//高

	/* 	>0 ， ch偏移
		<0, 失败*/
	int (*getdot)(char *Ch, FontDot *Dot);
}FontHead;

/*-------------------共用函数------------------------------*/
typedef enum{
	ASC_AREA = 0,		//单字节， ASC字符
	HZ_DBYTE_1AREA = 1, //符号区，双字节1区 A1A1--A9FE
	HZ_DBYTE_2AREA = 2, //汉字区，双字节2区 属于GB2312区间
	HZ_DBYTE_3AREA = 3, //汉字区，双字节3区 8140--A0FE
	HZ_DBYTE_4AREA = 4, //汉字区，双字节4区 AA40--FEA0
	HZ_DBYTE_5AREA = 5, //符号区，双字节5区 A840--A9A0
	HZ_QBYTE_AREA  = 6, //汉字区，四字节区
	HZ_USER_AREA   = 7, //用户自定义区
	HZ_NO_AREA	   = 8, //无定义区域
}FontHzArea;

/*
	区分字符在哪个区间
输入：
    str:输入的双字节的区位码(两个BYTE)
返回：
	FontHzArea
*/
static FontHzArea font_get_area(char c1, char c2)
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

/*-------------------字体定位算法------------------------------*/

/*
	点阵字库由<字库制作软件>生成
	点阵定位请参考《点阵字库生成器使用说明.pdf》
*/
int32_t font_pos_1_gbk(char c1, char c2)
{
	uint8_t hcode,lcode;
	uint32_t addr;

	hcode = c1;
	lcode = c2;
	
	if ((hcode < 0x81) 
		|| (hcode > 0xfe)
		) {
		//uart_printf("no china hz\r\n");
		return -1;
	}

	//PRINTF("hz code:%02x, %02x\r\n", hcode, lcode);
	addr = (hcode-0x81)*190;
	if (lcode<0x7f) {
		addr = addr+lcode-0x40;	
	}else{
		addr = addr+lcode-0x41;	
	}
	
	return addr;

}
/*
	点阵字库由<字库制作软件>生成
	点阵定位请参考《点阵字库生成器使用说明.pdf》
*/
int32_t font_pos_1_big5(char c1, char c2)
{
	uint8_t hcode,lcode;
	uint32_t addr;

	hcode = c1;
	lcode = c2;
	
	if((hcode < 0xa1) 
	|| (hcode > 0xf9)
		)
	{
		//uart_printf("no big5 hz\r\n");
		return -1;
	}

	addr = (hcode-0xa1)*157;
	if((lcode<=0x7e) && (lcode >= 0x40)){
		addr = addr+lcode-0x40;	
	}else if((lcode<=0xfe) && (lcode >= 0xa1)){
		addr = addr+lcode-0x62;	
	}else{
		//uart_printf("no big5 hz\r\n");
		return -1;
	}
	//uart_printf(" big5 code:%02x, %02x, addr:%x\r\n", hcode, lcode, addr);
	
	return addr;
}
/*--------------------------------------------------------*/
/* 
	思源宋体 用字模工具生成的18030字库
	从矢量字体转来的，某些字效果非常不好
	仅供测试

	保存在文件系统
	
*/
int font_syst1212_getdot(char *Ch, FontDot *Dot);

FontHead SYSongTi1212 ={

	.name = "SYsongti_12",//名字
	.size = 12,
	.path = "mtd0/1:/songti1212.DZK",//路径
		//"mtd0/1:/shscn1212.DZK",
	.st = FONT_ST_GB18030,
	.dt = FONT_V_H_U_D_L_R,
	
	.datac = 24,
	.w = 12,
	.h = 12,
	
	.getdot = font_syst1212_getdot,
	};

int font_syst1212_getdot(char *Ch, FontDot *Dot)
{
	uint32_t addr;
	uint32_t base_addr;
	uint8_t i;
	uint8_t *p;
	FontHzArea area;
	int res;
	u8* fp;
	
	static int font = NULL;

	if(font == NULL) {
		font = vfs_open(SYSongTi1212.path, O_RDONLY);
		if (font == NULL) return -1;
	}

	area = font_get_area(*Ch, *(Ch+1));

	if(area == ASC_AREA) {
		res = 1;
		
		fp = (u8*)(font_vga_6x12.path) + (*Ch)*font_vga_6x12.size;
		memcpy(Dot->dot, fp, font_vga_6x12.size);
		
		Dot->datac = font_vga_6x12.size;
		Dot->dt = FONT_H_H_L_R_U_D;     //内置ASC码格式
		Dot->w = font_vga_6x12.width;
		Dot->h = font_vga_6x12.height;
			
	} else if(area == HZ_USER_AREA
		||area == HZ_NO_AREA) {
		res = 2;
	} else if(area == HZ_QBYTE_AREA ) {
		res = 4;
	} else {
		res = 2;
		
		addr = font_pos_1_gbk(*Ch, *(Ch+1));
		addr = addr * SYSongTi1212.datac;

		vfs_lseek(font, addr, SEEK_SET);
		vfs_read(font, (const void *)Dot->dot, SYSongTi1212.datac);

		Dot->datac = SYSongTi1212.datac;
		Dot->dt = SYSongTi1212.dt;
		Dot->w = SYSongTi1212.w;
		Dot->h = SYSongTi1212.h;	
	}
	

	/*只支持双字节汉字*/
	return res;

}

/*------------------------------------------------------*/

int font_syst1616_getdot(char *Ch, FontDot *Dot);

FontHead SYSongTi1616 ={

	.name = "SYsongti_16",//名字
	.size = 16,
	.path = "mtd0/1:/shscn1616.DZK",//路径
			//"mtd0/1:/songti1616.DZK",
	.st = FONT_ST_GB18030,
	.dt = FONT_V_H_U_D_L_R,
	
	.datac = 32,
	.w = 16,
	.h = 16,
	
	.getdot = font_syst1616_getdot,
	};

int font_syst1616_getdot(char *Ch, FontDot *Dot)
{
	uint32_t addr;
	uint32_t base_addr;
	uint8_t i;
	uint8_t *p;
	FontHzArea area;
	int res;
	u8* fp;
	
	static int font = NULL;

	if(font == NULL) {
		font = vfs_open(SYSongTi1616.path, O_RDONLY);
		if (font == NULL) return -1;
	}

	area = font_get_area(*Ch, *(Ch+1));

	if(area == ASC_AREA) {
		res = 1;
		
		fp = (u8*)(font_vga_8x16.path) + (*Ch)*font_vga_8x16.size;
		memcpy(Dot->dot, fp, font_vga_8x16.size);
		
		Dot->datac = font_vga_8x16.size;
		Dot->dt = FONT_H_H_L_R_U_D; 	//内置ASC码格式
		Dot->w = font_vga_8x16.width;
		Dot->h = font_vga_8x16.height;
			
	} else if(area == HZ_USER_AREA
		||area == HZ_NO_AREA) {
		res = 2;
	} else if(area == HZ_QBYTE_AREA ) {
		res = 4;
	} else {
		res = 2;
		
		addr = font_pos_1_gbk(*Ch, *(Ch+1));
		addr = addr * SYSongTi1616.datac;

		vfs_lseek(font, addr, SEEK_SET);
		vfs_read(font, (const void *)Dot->dot, SYSongTi1616.datac);

		Dot->datac = SYSongTi1616.datac;
		Dot->dt = SYSongTi1616.dt;
		Dot->w = SYSongTi1616.w;
		Dot->h = SYSongTi1616.h;	
	}
	

	/*只支持双字节汉字*/
	return res;

}

/*-------------------------------------------------------------------------------

	如何支持多国语言？ codepage， GBK、big5、其他语言， codepage是应用层的概念？
*/

FontHead *FontListN[] = {
	&SYSongTi1212,
	&SYSongTi1616,	
	};

/* 查询系统字体 */
FontHead * font_find_font(char *name)
{
	u8 fontnum;
	u8 i;

	FontHead *font = NULL;
	
	fontnum = sizeof(FontListN)/sizeof(FontHead *);
	i = 0;
	while(1) {
		if(i >= fontnum) break;
		if(0 == strcmp(FontListN[i]->name, name)) {
			font = FontListN[i];
			break;
		}
		i++;
	}	

	return font;
}
/*

return <0 err
	   >0 输入字符串偏移	ASC返回1，汉字2，四字节汉字返回4。

	   说明：
	   		传入是字体名字。
	   		不同的字体会根据自己支持的语言判断？
*/
int font_get_dotdata(char *fontname, char *str, FontDot *Dot)
{
	FontHead * font;
	int res;
	
	/* 判断系统是否存在字体 并返回字体的信息指针 */
	font = font_find_font(fontname);
	if(font == NULL)
		return -1;

	/* 读取字库 */
	res = font->getdot(str, Dot);
	return res;
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
	
	/* 判断系统是否存在字体 并返回字体的信息指针 */
	font = font_find_font(fontname);
	if(font == NULL) {
		/* 返回一个常用值，防止意外*/
		*w = 12;
		*h = 12;
		return -1;
	}

	/* 汉字两个字符， 转为1个字符宽度， 后续做codepage兼容在重新设计*/
	*w = font->w/2;
	*h = font->h;

	return 0;
}


