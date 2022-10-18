/*
	
	
	*/

#include "mcu.h"
#include "petite_config.h"
#include "font/font.h"

#include "log.h"
#include "board_sysconf.h"

#include "vfs.h"

static int32_t font_index_gbk_wjq(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4)
{
	return (c2-0x81)*197 + (c1-0x40);
}


int font_dot_wjq_addr(FontHead * font, uint32_t ChCode)
{
	int addr;
	unsigned char c1, c2;

	if (ChCode > 0xffff) return -1;

	c1 = (ChCode&0xff);
	c2 = (ChCode>>8)&0xff;
	
	addr = font_index_gbk_wjq(c1, c2, 0, 0);
	addr = addr * (font->datac);
	addr += font->shift;

	return addr;
}



