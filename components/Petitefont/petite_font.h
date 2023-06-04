#ifndef _FONT_H
#define _FONT_H


#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


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
	编码标准 */
typedef enum{
	FONT_ST_ASC = 1,
	FONT_ST_GBK,
	FONT_ST_BIG5,
	FONT_ST_UTF8,
	FONT_ST_UNICODE,
	FONT_ST_CODEPAGE,
}FontSt;

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
	/* bmp 贴图式点阵 */
	FONT_DOT_BMP,
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

/*	
	这是GBK编码的分区定义    	*/
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


typedef struct _PetiteFontDsc{
	int fd;//字库文件句柄
	
	uint32_t bmsize;
	uint8_t *bitmap;

	FontDotType fdt;	//取模方式
	FontSt	st;			// 字库编码格式，gbk, unicode ,big 5
	FontStcType stc;	//字库类型 wjq, ymy, zy
}PetiteFontDsc;
/*---------------------------------------------------------------------------------------------*/

struct _petite_font_t;
/** Describes the properties of a glyph.*/
typedef struct {
    const struct _petite_font_t *resolved_font; /**< Pointer to a font where the gylph was actually found after handling fallbacks*/
	/*  = advance,  字宽 */
    uint16_t adv_w; /**< The glyph needs this space. Draw the next glyph after this width.*/
    /*  box_w = width, 也就是bitmap 宽度 
		box_h = rows, bitmap 行数 */
    uint16_t box_w; /**< Width of the glyph's bounding box*/
    uint16_t box_h; /**< Height of the glyph's bounding box*/
	/*  矢量字库渲染出来的bitmap，用left和top表示，
		left： bitmap距离左边的距离，单位像素，可能为负数
		top： bitmap右上角距离基线的距离，可能为负数，例如下划线'_'
		lvgl处理成ofs_x和ofs_y, 可以简化填点时的操作*/
    int16_t ofs_x;  /**< x offset of the bounding box*/
    int16_t ofs_y;  /**< y offset of the bounding box*/
	/* */
    uint8_t bpp: 4;  /**< Bit-per-pixel: 1, 2, 4, 8*/
    uint8_t is_placeholder: 1; /** Glyph is missing. But placeholder will still be displayed */
} petite_font_glyph_dsc_t;


/** 本结构体是一个字库定义
	从lvgl拷贝改名而得，如你的系统使用lvgl，本结构体等同 lv_font_t*/
typedef struct _petite_font_t {
    /** Get a glyph's descriptor from a font*/
    bool (*get_glyph_dsc)(const struct _petite_font_t *, petite_font_glyph_dsc_t *, uint32_t letter, uint32_t letter_next);

    /** Get a glyph's bitmap from a font*/
    const uint8_t *(*get_glyph_bitmap)(const struct _petite_font_t *, uint32_t);

    /*Pointer to the font in a font pack (must have the same line height)*/
	/* 字高 */
    int16_t line_height;         /**< The real line height where any text fits*/
	/* 基线 */
    int16_t base_line;           /**< Base line measured from the top of the line_height*/
	/* 默认赋值 0*/
    uint8_t subpx  : 2;             /**< An element of `lv_font_subpx_t`*/
	/* 下划线位置？*/
    int8_t underline_position;      /**< Distance between the top of the underline and base line (< 0 means below the base line)*/
	/*  默认赋值0*/
	int8_t underline_thickness;     /**< Thickness of the underline*/
	/*  不同种类字库初始化时需要的数据
		比如tiny ttf就是struct ttf_font_desc，里面包含了文件句柄，数据流句柄等 */
    const void * dsc;               /**< Store implementation specific or run_time data or caching here*/
    const struct _petite_font_t * fallback;   /**< Fallback font for missing glyph. Resolved recursively */
	
    void * user_data;               /**< Custom user data for font.*/

} petite_font_t;


extern petite_font_t FontAsc6X12;
extern petite_font_t FontVga6X12;
extern petite_font_t FontVGA8x16;
extern petite_font_t FontVga8x8;

extern petite_font_t *font_wjq_create_from_file(const char * path, uint16_t line_height);
extern petite_font_t *bitmapfont_create_from_file(const char * path, uint16_t line_height);


#endif /* _VIDEO_FONT_H */

