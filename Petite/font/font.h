#ifndef _FONT_H
#define _FONT_H

/* github 上有一个 DisplayCore, 计划参考它来做字库管理 
	将asc字符内嵌再代码中，以便没汉字点阵时做显示 */

struct fbcon_font_desc 
{
    char *name;//字库名称
	char *path;//字库路径
    u16 width, height;//字库宽度与高度
	u16 size;//每个字符字节数
};

extern struct fbcon_font_desc font_vga_8x8;
extern struct fbcon_font_desc font_vga_8x16;
extern struct fbcon_font_desc font_vga_6x12;
/*-------------------------------------------------------------*/

/*
	字库标准*/
typedef enum{
	FONT_ST_GB2312 = 1,
	FONT_ST_GB18030,	
	FONT_ST_BIG5,
	FONT_ST_ASC,
}FontS;
/*
	字库类型，匹配取点阵算法
	*/
typedef enum{
	/* 屋脊雀字库，使用双字节码码位排布 */
	FONT_DOT_WJQ = 1,
	/* 中易点阵字库 */
	FONT_DOT_ZY,	
	/* 使用易木雨工作室软件 制作的点阵 */
	FONT_DOT_YMY,
}FontStcType;

	
/* 取模方式*/
typedef enum{
	FONT_DOT_HL = 1, /*横向取模，字节正序*/
	FONT_DOT_VL, /*纵向取模，字节正序*/	
	/* 	纵向取模,     高位在前, 左上-左下-右上-右下*/
	FONT_V_H_U_D_L_R, 
	/* 	纵向取模,     低位在前, 左上-右上-左下-右下*/
	FONT_V_L_L_R_U_D, 
	/* 	横向取模,     高位在前, 左上-右上-左下-右下*/
	FONT_H_H_L_R_U_D, 
}FontDotType;
	
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

typedef struct{
	FontDotType dt;//取模方式
	unsigned char datac;//1汉字点阵数据字节数

	char w;
	char h;
	char *dot;//2424点阵是72字节
}FontDot;

/* 定义一种字体 */
typedef struct _strFontHead
{
	char name[16];//song heiti 
	char size;//字号, 只做等宽字体，汉字12号就是1212， 24号就是2424， ASC12号就是0612，24号就是1224
	char path[64];//字库名字或路径，用于打开
	/* ----  */
	FontS st;//标准 ，主要用于区分18030或GB2312，如果是2312，遇到生僻字，就要从关联字库取
	FontStcType type;
	
	unsigned int shift;//点阵在文件中的开始地址
	
	unsigned char pitch;
	unsigned char datac;//1汉字点阵数据字节数

	unsigned char w;//宽
	unsigned char h;//高
	FontDotType dt;//取模方式
	
	/* 求索引 */
	//int fd;
}FontHead;

extern void font_init(void);

extern void *font_find_font(char *name);
extern int font_getdot(void *fontstr_in, char *Ch, FontDot *Dot);
extern int font_get_dotdata(char *fontname, char *str, FontDot *Dot);
extern s32 font_get_hw(char *font, u16 *h, u16 *w);


#endif /* _VIDEO_FONT_H */

