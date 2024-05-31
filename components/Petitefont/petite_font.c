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
#include "petite_font.h"
#include "log.h"
#include "vfs.h"

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


/**
 * @brief    从字符串中按照指定编码提取1个字符的编码
 * 
 * @param   str         输入字符串
 * @param   code        输出字符编码
 * @param   CodeType    字符串编码类型
 * @return  * uint8_t   *code字符字节数
 * 
 */
uint8_t font_get_ch_from_str(char *str, uint32_t *code, char CodeType)
{
	int chr = 1;
	uint32_t chcode = 0x00000042;
	/* GBK编码分区 */
	FontHzArea area = ASC_AREA;
	
	switch (CodeType)
	{
		case FONT_ST_UTF8:
			chcode = *str;
			chr = 1;
			break;
		case FONT_ST_GBK:
			/* 这是gbk+asc编码格式的 方法 */
			area = font_gbk_get_area(*str, *(str+1));
			if(area == ASC_AREA) {
				chcode = *str;
				chr = 1;
			} else if(area == HZ_USER_AREA
				||area == HZ_NO_AREA) {
				chcode = *str;
				chcode = chcode << 8;
				chcode += *(str+1);
				chr = 2;
			} else if(area == HZ_QBYTE_AREA ) {
				chr = 4;
			} else {
				chr = 2;
				chcode = *str;
				chcode = chcode << 8;
				chcode += *(str+1);
			}
			break;
			
		case FONT_ST_BIG5:
			break;

		case FONT_ST_UNICODE:
			chr = 2;
			chcode = *(str+1);
			chcode = chcode << 8;
			chcode += *str;
			break;
			
		default:
			chcode = *str;
			chr = 1;
			break;
	}


	*code = chcode;

	return chr;
}
#if 0
/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return pointer to the bitmap of the letter
 */
const uint8_t * petite_font_get_glyph_bitmap(const petite_font_t * font_p, uint32_t letter)
{
    return font_p->get_glyph_bitmap(font_p, letter);
}

/**
 * Get the descriptor of a glyph
 * @param font_p pointer to font
 * @param dsc_out store the result descriptor here
 * @param letter an UNICODE letter code
 * @param letter_next the next letter after `letter`. Used for kerning
 * @return true: descriptor is successfully loaded into `dsc_out`.
 *         false: the letter was not found, no data is loaded to `dsc_out`
 */
bool petite_font_get_glyph_dsc(const lv_font_t * font_p, lv_font_glyph_dsc_t * dsc_out, uint32_t letter,
                           uint32_t letter_next)
{
    LV_ASSERT_NULL(font_p);
    LV_ASSERT_NULL(dsc_out);
    dsc_out->resolved_font = NULL;
    const lv_font_t * f = font_p;
    bool found = false;
    while(f) {
        found = f->get_glyph_dsc(f, dsc_out, letter, letter_next);
        if(found && !dsc_out->is_placeholder) {
            dsc_out->resolved_font = f;
            break;
        }
        f = f->fallback;
    }
    return found;
}

/**
 * Get the width of a glyph with kerning
 * @param font pointer to a font
 * @param letter an UNICODE letter
 * @param letter_next the next letter after `letter`. Used for kerning
 * @return the width of the glyph
 */
uint16_t petite_font_get_glyph_width(const lv_font_t * font, uint32_t letter, uint32_t letter_next)
{
    LV_ASSERT_NULL(font);
    lv_font_glyph_dsc_t g;
    bool ret;
    ret = lv_font_get_glyph_dsc(font, &g, letter, letter_next);
    if(ret) return g.adv_w;
    else return 0;
}

#endif

