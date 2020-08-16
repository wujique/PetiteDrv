#ifndef _FONT_H
#define _FONT_H

/* github 上有一个 DisplayCore, 计划参考它来做字库管理 */

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


extern s32 font_get_hw(char *font, u16 *h, u16 *w);


/*------------------- 重构 --------------*/
/*
	字库标准*/
typedef enum{
	FONT_ST_GB2312 = 1,
	FONT_ST_GB18030,	
	FONT_ST_BIG5,
}FontS;
	
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

typedef struct{
	FontDotType dt;//取模方式
	unsigned char datac;//1汉字点阵数据字节数

	char w;
	char h;
	char *dot;//2424点阵是72字节
}FontDot;



#endif /* _VIDEO_FONT_H */

