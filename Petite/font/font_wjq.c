/*
	
	
	*/

#include "mcu.h"
#include "petite_config.h"
#include "font/font.h"

#include "log.h"
#include "board_sysconf.h"

#include "vfs.h"

int32_t font_index_gbk_wjq(FontHzArea area, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4)
{
	return (c1-0x81)*197 + (c2-0x40);
}


int font_dot_wjq_addr(FontHead * font, char *Ch, FontHzArea area)
{
	int addr;

	if(area == HZ_QBYTE_AREA ) return -1;
	
	addr = font_index_gbk_wjq(area, *Ch, *(Ch+1), 0, 0);
	addr = addr * (font->datac);
	addr += font->shift;

	return addr;
}



