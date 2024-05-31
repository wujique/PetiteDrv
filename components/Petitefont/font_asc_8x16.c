

#include "mcu.h"
#include "petite_font.h"
#include "log.h"

/*
	屋脊雀工作室制作的ASC点阵。
	大小写统一基线，每一个字符都经过人工优化，在COG LCD上显示效果非常出色。
	*/
const uint8_t font_asc_1608[]=
{
	0xff
	/* todo */
};


bool FontAsc8X16_get_glyph_dsc(const struct _petite_font_t *pfont, petite_font_glyph_dsc_t *glyph, uint32_t letter, uint32_t letter_next)
{

	glyph->adv_w = 8;
	glyph->box_w = 8;

	glyph->box_h = 16;
	
	glyph->ofs_x = 0;
	glyph->ofs_y = 0;

	glyph->bpp = 1;

	return true;
}


const uint8_t *FontAsc8X16_get_glyph_bitmap(const petite_font_t * pfont, uint32_t letter)
{
	uint32_t shift;
	uint16_t code;

	//uart_printf("get bmp font:%04x\r\n", letter);
	/* */
	if ((letter >0x7f) || (letter < 0x20)) return NULL;
	
	code = letter - 0x20;
	
	shift = code * 16;

	return (const uint8_t *)&font_asc_1608[shift];
}

PetiteFontDsc FontAsc8x16Pfd={
	.fdt = FONT_H_H_L_R_U_D,
	.st = FONT_ST_ASC,
	.stc = FONT_DOT_NULL,
};


petite_font_t FontAsc8X16 ={

	.get_glyph_dsc = FontAsc8X16_get_glyph_dsc,
	.get_glyph_bitmap = FontAsc8X16_get_glyph_bitmap,

	.line_height = 16,
	.base_line = 13,
	.subpx = 0,

	.underline_position = 15,
	.underline_thickness = 1,

	.dsc = (void *)&FontAsc8x16Pfd,

	.fallback = NULL,
	.user_data = NULL,
};


