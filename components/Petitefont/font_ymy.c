/*
	ZY点阵的定位算法，不要公开！
	
	*/

#include "mcu.h"
#include "petite_config.h"
#include "petite_font.h"

#include "log.h"
#include "board_sysconf.h"

#include "vfs.h"


/*
	点阵字库由<字库制作软件>生成
	点阵定位请参考《点阵字库生成器使用说明.pdf》
	输入汉字双字节内码，
	返回汉字在点阵字库文件中的索引
*/
#if 0
int32_t font_pos_1_gbk(FontHzArea area, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4)
{
	uint8_t hcode,lcode;
	uint32_t index;

	hcode = c1;
	lcode = c2;
	
	if ((hcode < 0x81) 
		|| (hcode > 0xfe)
		) {
		//uart_printf("no china hz\r\n");
		return -1;
	}

	//PRINTF("hz code:%02x, %02x\r\n", hcode, lcode);
	index = (hcode-0x81)*190;
	if (lcode<0x7f) {
		index = index+lcode-0x40;	
	}else{
		index = index+lcode-0x41;	
	}
	
	return index;

}
/*
	点阵字库由<字库制作软件>生成
	点阵定位请参考《点阵字库生成器使用说明.pdf》
*/
int32_t font_pos_1_big5(FontHzArea area, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4)
{
	uint8_t hcode,lcode;
	uint32_t index;

	hcode = c1;
	lcode = c2;
	
	if((hcode < 0xa1) 
	|| (hcode > 0xf9)
		)
	{
		//uart_printf("no big5 hz\r\n");
		return -1;
	}

	index = (hcode-0xa1)*157;
	if((lcode<=0x7e) && (lcode >= 0x40)){
		index = index+lcode-0x40;	
	}else if((lcode<=0xfe) && (lcode >= 0xa1)){
		index = index+lcode-0x62;	
	}else{
		//uart_printf("no big5 hz\r\n");
		return -1;
	}
	//uart_printf(" big5 code:%02x, %02x, addr:%x\r\n", hcode, lcode, addr);
	
	return index;
}


int font_dot_ymy_addr(FontHead * font, char *Ch, FontHzArea area)
{
	int addr;
	
	if(font->st == FONT_ST_GB18030
		|| font->st == FONT_ST_GB2312)	
		addr = font_pos_1_gbk(area, *Ch, *(Ch+1), 0, 0);
	else if(font->st == FONT_ST_BIG5)
		addr = font_pos_1_big5(area, *Ch, *(Ch+1), 0, 0);
	else
		addr =  -1;

	if(addr < 0 ) return -1;
	
	addr = addr * (font->datac);
	addr += font->shift;

	return addr;
}
#endif

