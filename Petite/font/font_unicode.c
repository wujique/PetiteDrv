/*
	本文件支持的字库说明
	1 字库是用自主开发的工具生成，不是全字库，是需要哪个字符就包含哪个字符
	2 字库结构4部分
		1.block0: 头部说明,指明类型,版本，----struct _StrBitmapFontHead
		2.block1: 按序排列的unicode表，每个字符的unicode占两个自己
		3.block2: 和unicode表顺序一致的bitmap参数表，------struct _strBitmapHead
		4.block3: 字符bitmap数据，也就是点阵，字节数不定

	所谓的bitmap，就是用freetype将适量字体渲染后得到的点阵数据
	
	本文件功能，根据输入的unicode码，获取字库文件读点阵的偏移和数据长度

	*/

#include "mcu.h"
#include "petite_config.h"
#include "font/font.h"
#include "log.h"
#include "board_sysconf.h"
#include "vfs.h"
#include "petite.h"
#include "drv_config.h"


/*
	bitmap字库文件头
	*/
struct _StrBitmapFontHead{
	//int headlen;//头长度
	char type[32];
	char ver[32];
	/* 字符宽度和高度，在进行渲染时设置freetype
		需要注意的是，转换后得到的bitmap不一定是这个W和H*/
	int pixelw;
	int pixelh;
	int total;//字符总个数
	int unicode_tab;//unicode表起始位置
	int bitmap_head;//bitmap参数起始位置，每个字符都有一个_strBitmapHead，长度固定
	int bitmap;//字符点阵数据在文件中的起始位置

	int rev[16-6];
	
};
#if 0
/*
	每个字符的bitmap都有一个头，
	为了取点阵方便，头长度是固定的，所有将所有的头放在一起
	*/
struct _strBitmapHead{
	uint16_t rev;//保留字节，默认是回车换行，为了4字节对齐而定义
	/* 以下五个数据在LCD描字时需要 */
	uint16_t rows;//bitmap行数
	uint16_t width;//bitmap宽度，单位像素
	uint16_t pitch;//每row占字节数
	int16_t left;//bitmap距离左边的距离，单位像素，可能为负数
	int16_t top;//bitmap右上角距离基线的距离，可能为负数，例如下划线'_'
	/*	读bitmap数据的偏移地址  	 */
	uint32_t index;
};
#endif
/*
	输入一个unicode内码取点阵的方法
	1 从block1:unicode tab中查询，得到位置索引------多次读文件-----是否可以用二分法提高速度？
	2 使用位置索引从block2:bitmap_head中读取bitmap头信息
		从而的到bitmap数据偏移和bitmap数据长度
	3 从block3：bitmap data中读取点阵数据

	如果，将block2和block3揉在一起：
	1 从bloc1:unicode tab表中查询，得到bitmap偏移
	2 根据偏移，读出bitmap head和bitmap data, 问题是bitmap data 的长度是要根据bitmap head算出的。
	*/
/*	基线
	1212字符的基线是10
	1616字符的基线是14 
	比如要在(0,0)开始显示1212字符，那么基线就是(x, 10)        
	*/
#define BASE_LINE 10
	/* 行宽 */
#define LINE_HIGHT 12


struct _StrBitmapFontHead BitmapFontHead;
int fd_fontfile = 0;

int font_unicode_bitmap_init(void)
{
	int rlen = 0;
	
	fd_fontfile = vfs_open("mtd0/0:font/font_file.bin", O_RDONLY);
	if (fd_fontfile == NULL) {
		uart_printf("bitmap font open err!\r\n");
	}

	rlen = vfs_read(fd_fontfile, &BitmapFontHead, sizeof(BitmapFontHead));
	uart_printf("rlen:%d\r\n", rlen);
	uart_printf("%s\r\n", BitmapFontHead.type);
	uart_printf("%s\r\n", BitmapFontHead.ver);

	uart_printf("shift:\r\n");
	uart_printf("\t pixelw:%d\r\n", BitmapFontHead.pixelw);
	uart_printf("\t pixelh:%d\r\n", BitmapFontHead.pixelh);
	uart_printf("\t total:%d\r\n", BitmapFontHead.total);
	uart_printf("\t unicode_tab:%d\r\n", BitmapFontHead.unicode_tab);
	uart_printf("\t bitmap_head:%d\r\n", BitmapFontHead.bitmap_head);
	uart_printf("\t bitmap:%d\r\n", BitmapFontHead.bitmap);

}
/*
	根据Unicode码找出点阵索引
	*/
int font_find_index(uint16_t unicode)
{

	int rlen = 0;
	uint16_t uindex, uindex_f, uindex_l;
	uint16_t utmp;

	uindex_f = 0;
	uindex_l = BitmapFontHead.total-1;
	
	//uart_printf("unicode: %04x\r\n", unicode);
	while(uindex_f <= uindex_l){
		uindex = uindex_f + (uindex_l - uindex_f)/2;
		vfs_lseek(fd_fontfile, BitmapFontHead.unicode_tab + uindex*2, 0);
		rlen = vfs_read(fd_fontfile, &utmp, 2);
		if(utmp == unicode){
			//uart_printf("index: %d\r\n\r\n", uindex);
			return uindex;
		}else if(utmp > unicode) {
			uindex_l = uindex-1;
		} else {
			uindex_f = uindex+1;
		}
	}
	//uart_printf("uindex %d\r\n", uindex);

	return -1;
}

/*
	输入UNICODE获取bitmap点阵数据
	*/
int font_bitmap_getdata(struct _strBitmapHead* head, uint8_t *dotbuf, uint16_t unicode)
{
	int uindex;
	int rlen = 0;
	
	uindex = font_find_index(unicode);
	if (uindex == -1){
		return -1;
	}
	/* 读头 */
	vfs_lseek(fd_fontfile, BitmapFontHead.bitmap_head + uindex*sizeof(struct _strBitmapHead), 0);
	rlen = vfs_read(fd_fontfile, head, sizeof(struct _strBitmapHead));

	/* 读数据 */
	vfs_lseek(fd_fontfile, BitmapFontHead.bitmap +  head->index, 0);
	rlen = vfs_read(fd_fontfile, dotbuf, head->pitch*head->rows);

	return 0;
}


/*--------------------以下为测试程序 ------------------------*/


/* 
	在指定位置显示字符串
	输入的字符串是utf16格式
	*/
int font_bitmap_showstr_unicode(DevLcdNode *lcd, uint16_t x, uint16_t y, uint16_t *unicodestr)
{
	uint16_t *pStr;
	uint16_t lcdx,lcdy;
	uint8_t row, col,yshift;
	uint8_t dotdata;
	struct _strBitmapHead BitmapHead;
	uint8_t bitmaptmp[256];
	
	pStr = unicodestr;

	lcdx = x;
	lcdy = y;

	while((*pStr) != 0){

		font_bitmap_getdata(&BitmapHead, bitmaptmp,*pStr);

		/* 填点 left 和 top 两个参数都会出现负数 */
		lcdx += BitmapHead.left;
		
		if (BitmapHead.top > BASE_LINE) yshift = 0;
		else yshift = BASE_LINE - BitmapHead.top;
		//uart_printf("yshift:%d\r\n", yshift);
		if (yshift >= LINE_HIGHT) yshift = LINE_HIGHT-1;
		
		
		for(row=0; row<BitmapHead.rows; row++){
			
			for(col=0; col<BitmapHead.width; col++){
				dotdata = bitmaptmp[row * BitmapHead.pitch + col/8];
				
				if((dotdata & (0x80>>(col%8)) )!=0 ){
					//uart_printf("*");
					drv_ST7565_drawpoint(lcd, lcdx + col, lcdy + row + yshift, BLACK);
				}else{
					//uart_printf("-");
					drv_ST7565_drawpoint(lcd, lcdx + col, lcdy + row + yshift, WHITE);
				}
			}
			//uart_printf("\r\n");
		}
		
		lcdx += BitmapHead.width;

		pStr++;
	}

	uart_printf("update\r\n");
	drv_ST7565_update(lcd);
	
	return 0;
}
/*
	Windows环境通常就是这种模式：asc+gbk
	*/
int font_bitmap_showstr_ansi(DevLcdNode *lcd, uint16_t x, uint16_t y, char *str)
{
	char *pStr;
	uint16_t lcdx,lcdy;
	uint8_t row, col,yshift;
	uint8_t dotdata;
	struct _strBitmapHead BitmapHead;
	uint8_t bitmaptmp[256];
	uint16_t unicode;
	
	pStr = str;

	lcdx = x;
	lcdy = y;

	while((*pStr) != 0){

		/*	如果是中文，自己添加
			*/
		unicode = *pStr;
		font_bitmap_getdata(&BitmapHead, bitmaptmp, unicode);
		/* 填点 left 和 top 两个参数都会出现负数 */
		lcdx += BitmapHead.left;
		
		if (BitmapHead.top > BASE_LINE) yshift = 0;
		else yshift = BASE_LINE - BitmapHead.top;
		//uart_printf("yshift:%d\r\n", yshift);
		if (yshift >= LINE_HIGHT) yshift = LINE_HIGHT-1;
		
		
		for(row=0; row<BitmapHead.rows; row++){
			
			for(col=0; col<BitmapHead.width; col++){
				dotdata = bitmaptmp[row * BitmapHead.pitch + col/8];
				
				if((dotdata & (0x80>>(col%8)) )!=0 ){
					//uart_printf("*");
					drv_ST7565_drawpoint(lcd, lcdx + col, lcdy + row + yshift, BLACK);
				}else{
					//uart_printf("-");
					drv_ST7565_drawpoint(lcd, lcdx + col, lcdy + row + yshift, WHITE);
				}
			}
			//uart_printf("\r\n");
		}
		
		lcdx += BitmapHead.width;

		pStr++;
	}

	uart_printf("update\r\n");
	drv_ST7565_update(lcd);
	
	return 0;
}

char teststr1[]="The Enter Key!";
char teststr2[]="The 012349 Num!";

void font_unicode_bitmap_test(DevLcdNode *lcd)
{
	font_unicode_bitmap_init();
	font_bitmap_showstr_ansi(lcd, 0, 0, teststr1);
	font_bitmap_showstr_ansi(lcd, 0, LINE_HIGHT, teststr2);

	while(1){	}
	
}

/*
	按顺序将整个字库打印出来
	*/
void font_unicode_bitmap_test1(DevLcdNode *lcd)
{
	int res;
	int rlen = 0;
	int uindex;
	uint16_t lcdx,lcdy;
	uint8_t row, col,yshift;
	uint8_t dotdata;
	struct _strBitmapHead BitmapHead;
	uint8_t bitmaptmp[256];

	font_unicode_bitmap_init();

	uindex = 0;
	
	lcdx = 0;
	lcdy = 0;

	#if 1
	while(1){
		uart_printf("unicode index:%d\r\n", uindex);
		res = vfs_lseek(fd_fontfile, BitmapFontHead.bitmap_head + (uindex * sizeof(BitmapHead)), 0);
		uart_printf("lseek res:%d\r\n", res);
		rlen = vfs_read(fd_fontfile, &BitmapHead, sizeof(BitmapHead));
		uart_printf("read res:%d\r\n", rlen);
		{
		
		uart_printf("rows:%d\r\n", BitmapHead.rows);
		uart_printf("width:%d\r\n", BitmapHead.width);
		uart_printf("pitch:%d\r\n", BitmapHead.pitch);
		uart_printf("left:%d\r\n", BitmapHead.left);
		uart_printf("top:%d\r\n", BitmapHead.top);
		uart_printf("index:%d\r\n", BitmapHead.index);

		vfs_lseek(fd_fontfile, BitmapFontHead.bitmap +  BitmapHead.index, 0);
		rlen = vfs_read(fd_fontfile, bitmaptmp, BitmapHead.pitch*BitmapHead.rows);
		//PrintFormat(bitmaptmp, BitmapHead.pitch*BitmapHead.rows);

		if (lcdx + BitmapHead.width >= 128) {
			lcdx = 0;
			lcdy += LINE_HIGHT + 1;
			if(lcdy + LINE_HIGHT > 64){
				lcdy = 0;
				uart_printf(" lcd update\r\n");
				drv_ST7565_update(lcd);
				osDelay(2000);
				drv_ST7565_color_fill(lcd, 0, 200, 0, 200, WHITE);
			}
		}

		/* 填点 left 和 top 两个参数都会出现负数 */
		if (BitmapHead.left < 0 || BitmapHead.top < 0){
			uart_printf("left:%d, top:%d\r\n", BitmapHead.left, BitmapHead.top);
		}
		lcdx += BitmapHead.left;
		
		if (BitmapHead.top > BASE_LINE) yshift = 0;
		else yshift = BASE_LINE - BitmapHead.top;
		
		if (yshift >= LINE_HIGHT) yshift = LINE_HIGHT-1;
		
    	for(row=0; row<BitmapHead.rows; row++){
            
			for(col=0; col<BitmapHead.width; col++){
				dotdata = bitmaptmp[row * BitmapHead.pitch + col/8];
				
            	if((dotdata & (0x80>>(col%8)) )!=0 ){
                	//uart_printf("*");
					drv_ST7565_drawpoint(lcd, lcdx + col, lcdy + row + yshift, BLACK);
            	}else{
                	//uart_printf("-");
					drv_ST7565_drawpoint(lcd, lcdx + col, lcdy + row + yshift, WHITE);
            	}
			}
        	//uart_printf("\r\n");
    	}
		
		lcdx += BitmapHead.width;

    	//uart_printf(">>>>>>>>>>>>>>>>>>");
	}

		#if 1
		uindex++;
		if (uindex >=  BitmapFontHead.total){
			drv_ST7565_update(lcd);
			osDelay(2000);
			uindex = 0;
			drv_ST7565_color_fill(lcd, 0, 200, 0, 200, WHITE);
			break;
		}
		#endif
	}
	#endif
	while(1){
		uart_printf(" finish \r\n");
		osDelay(5000);	
	}
}


