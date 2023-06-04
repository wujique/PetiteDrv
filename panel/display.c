/**
 * @file            dev_lcd.c
 * @brief           LCD 中间层
 * @author          wujique
 * @date            2018年4月17日 星期二
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年4月17日 星期二
 *   作    者:         wujique
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
#include "petite.h"
#include "board_sysconf.h"
#include "drv_lcd.h"

u16 PenColor = BLACK;
u16 BackColor = BLUE;



/* 

从tslib拷贝一些显示函数到这里
这些函数可以归为GUI
*/

/**
 *@brief:      line
 *@details:    画一条线
 *@param[in]   int x1           
               int y1           
               int x2           
               int y2           
               unsigned colidx  
 *@param[out]  无
 *@retval:     
 */
void line (DevLcdNode *lcd, int x1, int y1, int x2, int y2, unsigned colidx)
{
	int tmp;
	int dx = x2 - x1;
	int dy = y2 - y1;

	if (lcd == NULL) return;

	if (abs (dx) < abs (dy)) {
		if (y1 > y2) {
			tmp = x1; x1 = x2; x2 = tmp;
			tmp = y1; y1 = y2; y2 = tmp;
			dx = -dx; dy = -dy;
		}
		x1 <<= 16;
		/* dy is apriori >0 */
		dx = (dx << 16) / dy;
		while (y1 <= y2){
			lcd_drawpoint(lcd, x1 >> 16, y1, colidx);
			x1 += dx;
			y1++;
		}
	} else {
		if (x1 > x2) {
			tmp = x1; x1 = x2; x2 = tmp;
			tmp = y1; y1 = y2; y2 = tmp;
			dx = -dx; dy = -dy;
		}
		
		y1 <<= 16;
		dy = dx ? (dy << 16) / dx : 0;
		while (x1 <= x2) {
			lcd_drawpoint(lcd, x1, y1 >> 16, colidx);
			y1 += dy;
			x1++;
		}
	}
}

/**
 *@brief:     put_cross
 *@details:   画十字
 *@param[in]  int x            
              int y            
              unsigned colidx  
 *@param[out]  无
 *@retval:     
 */
void put_cross(DevLcdNode *lcd, int x, int y, unsigned colidx)
{
	if (lcd == NULL) return;
	
	line (lcd, x - 10, y, x - 2, y, colidx);
	line (lcd, x + 2, y, x + 10, y, colidx);
	line (lcd, x, y - 10, x, y - 2, colidx);
	line (lcd, x, y + 2, x, y + 10, colidx);

	line (lcd, x - 6, y - 9, x - 9, y - 9, colidx + 1);
	line (lcd, x - 9, y - 8, x - 9, y - 6, colidx + 1);
	line (lcd, x - 9, y + 6, x - 9, y + 9, colidx + 1);
	line (lcd, x - 8, y + 9, x - 6, y + 9, colidx + 1);
	line (lcd, x + 6, y + 9, x + 9, y + 9, colidx + 1);
	line (lcd, x + 9, y + 8, x + 9, y + 6, colidx + 1);
	line (lcd, x + 9, y - 6, x + 9, y - 9, colidx + 1);
	line (lcd, x + 8, y - 9, x + 6, y - 9, colidx + 1);

}
/**
 *@brief:      put_char
 *@details:    显示一个英文
 *@param[in]   int x       
               int y       
               int c       
               int colidx  
 *@param[out]  无
 *@retval:     
 */
void put_char(DevLcdNode *lcd, int x, int y, int c, int colidx)
{
	int i,j,bits;
	u8* p;
	
	if (lcd == NULL) return;	
	
	p = (u8*)font_vga_8x8.path;//need fix
	for (i = 0; i < font_vga_8x8.height; i++) {
		bits =  p[font_vga_8x8.height * c + i];
		for (j = 0; j < font_vga_8x8.width; j++, bits <<= 1) {
			if (bits & 0x80) {
				lcd->drv->draw_point(lcd, x + j, y + i, colidx);
			}
		}
	}
}
/**
 *@brief:      put_string
 *@details:    显示一个字符串
 *@param[in]   int x            
               int y            
               char *s          
               unsigned colidx  
 *@param[out]  无
 *@retval:     
 */
void put_string(DevLcdNode *lcd, int x, int y, char *s, unsigned colidx)
{
	int i;
	
	if (lcd == NULL)	return;	
	
	for (i = 0; *s; i++, x += font_vga_8x8.width, s++)
		put_char(lcd, x, y, *s, colidx);
}
/**
 *@brief:      put_string_center
 *@details:    居中显示一个字符串
 *@param[in]   int x            
               int y            
               char *s          
               unsigned colidx  
 *@param[out]  无
 *@retval:     
 */
void put_string_center(DevLcdNode *lcd, int x, int y, char *s, unsigned colidx)
{
	int sl = strlen (s);
	
	if(lcd == NULL)	return;	
	
    put_string (lcd, x - (sl / 2) * font_vga_8x8.width,
                y - font_vga_8x8.height / 2, s, colidx);
}

/**
 *@brief:      rect
 *@details:    画一个矩形框
 *@param[in]   int x1           
               int y1           
               int x2           
               int y2           
               unsigned colidx  
 *@param[out]  无
 *@retval:     
 */
void rect (DevLcdNode *lcd, int x1, int y1, int x2, int y2, unsigned colidx)
{
	if (lcd == NULL) return;

	line (lcd, x1, y1, x2, y1, colidx);
	line (lcd, x2, y1, x2, y2, colidx);
	line (lcd, x2, y2, x1, y2, colidx);
	line (lcd, x1, y2, x1, y1, colidx);
}

/*
	在framebuff中填充字符,

	用户需求：
		在指定LCD的某个位置，用指定的字体，显示一串字符。
	冰山下需求：
		1.都有什么样的LCD？
		2.字体，有GBK排布的点阵字库，也有UNICODE排布的矢量字库。
		3.输入的字符串有GBK编码，也有utf8编码。
	*/
s32 display_put_str_to_framebuff(DevLcdNode *lcd, petite_font_t *font, int x, int y, char *s, unsigned colidx)
{
	u16 slen;
	
	s32 res;
	int chr;
	u16 sidx;
	u16 col,row,lcdx,lcdy,yshift;
	
	char tmp;;
	
	const uint8_t *bitmap;
	unsigned color;

	char *chbuf=NULL;
	char *disstr;
		
	petite_font_glyph_dsc_t glyph_dsc;
	FontDotType fdt;	//取模方式
	FontSt	st;	
	PetiteFontDsc *pfdsc;
	
	if (lcd == NULL) return -1;

	pfdsc = (PetiteFontDsc *)font->dsc;
	fdt = pfdsc->fdt;
	st = pfdsc->st;
	
	/* 如果指定的字库编码方式和字符串编码方式不一致，则进行编码转换 */
	if (st == FONT_ST_GBK) {
		slen = strlen(s);
		chbuf = wjq_malloc(slen*2);
		memset(chbuf, 0, slen*2);
		utf8_2_gbk(s, chbuf, slen);
		slen = strlen(chbuf);
		disstr = chbuf;
	} else if (st == FONT_ST_UNICODE){
		slen = strlen(s);
		chbuf = wjq_malloc(slen*2);
		memset(chbuf, 0, slen*2);
		slen = utf8_2_utf16(s, chbuf, slen);
		disstr = chbuf;
		
	} else {
		disstr = s;
		slen = strlen(s);
	}
	
	sidx = 0;
	lcdx = x;
	lcdy = y;
	/*获取点阵，并转化为LCD像素*/
	while(1) {
		if (sidx >= slen) {
			uart_printf("sidx %d, slen %d\r\n", sidx, slen);
			break;
		}
		uint32_t chcode;
	
		/* 从字符串中取一个字符出来 */
		chr = font_get_ch_from_str(disstr+sidx, &chcode, st);
		uart_printf("chr %d, %x", chr, chcode);

		font->get_glyph_dsc(font, &glyph_dsc, chcode, NULL);
		bitmap = font->get_glyph_bitmap(font, chcode);
		
		/* 填点 left 和 top 两个参数都会出现负数 */
		lcdx += glyph_dsc.ofs_x;
		
		if (glyph_dsc.ofs_y > font->base_line) yshift = 0;
		else yshift = glyph_dsc.ofs_y;
		
		if (yshift >= font->line_height) yshift = font->line_height-1;
	
		for (row = 0; row < glyph_dsc.box_h; row++) {
			for (col = 0; col < glyph_dsc.box_w ;col++) {
				
				if(fdt == FONT_V_H_U_D_L_R) {
					tmp = bitmap[col*2+row/8]&(0x80>>(row%8));
				} else if(fdt == FONT_V_L_L_R_U_D) {
					tmp = bitmap[col+row/8*glyph_dsc.box_w]&(0x01<<(row%8));
				} else if(fdt == FONT_H_H_L_R_U_D) {
					tmp = bitmap[col/8 + row*((glyph_dsc.box_w+7)/8)]&(0x80>>(col%8));
				}
				
				
				if (tmp != 0) {
					color = colidx;
				} else {
					color = BackColor;
				}
				res = lcd_drawpoint(lcd, lcdx + col, lcdy + row + yshift, color);
				if (res == -1) continue;
			}

		}	

		lcdx += glyph_dsc.box_w;
		sidx += chr;
	}
	if( chbuf != NULL)
		wjq_free(chbuf);
	return 0;	
}

/*
	没有framebuff的lcd的显示方法
	未测试，
	针对8080接口的，没有framebuff的LCD
	*/
s32 display_put_str_to_lcd(DevLcdNode *lcd, petite_font_t *font, int x, int y, char *s, unsigned colidx)
{
	u16 slen;

	int chr;
	u16 sidx;
	u16 col,row,lcdx,lcdy;
	s16 yshift;
	u16 buff_index = 0;

	u16 xlen,ylen;
	u16 fontw,fonth;
	u16 *framebuff;//样点缓冲，按照L2R_U2D格式填充
	
	char tmp;;
	
	const uint8_t *bitmap;
	u8 *dotbuf;//字符点阵缓冲
	unsigned color;

	char *chbuf = NULL;
	char *disstr;
	
	petite_font_glyph_dsc_t glyph_dsc;
	FontDotType fdt;	//取模方式
	FontSt	st;	
	PetiteFontDsc *pfdsc;
	
	if (lcd == NULL) return -1;
	
	pfdsc = (PetiteFontDsc *)font->dsc;
	fdt = pfdsc->fdt;
	st = pfdsc->st;
	
	/* 如果指定的字库编码方式和字符串编码方式不一致，则进行编码转换 */
	if (st == FONT_ST_GBK) {
		slen = strlen(s);
		chbuf = wjq_malloc(slen*2);
		memset(chbuf, 0, slen*2);
		utf8_2_gbk(s, chbuf, slen);
		slen = strlen(chbuf);
		disstr = chbuf;
	} else if (st == FONT_ST_UNICODE){
		slen = strlen(s);
		chbuf = wjq_malloc(slen*2);
		memset(chbuf, 0, slen*2);
		slen = utf8_2_utf16(s, chbuf, slen);
		disstr = chbuf;
		
	} else {
		disstr = s;
		slen = strlen(s);
	}

	fonth = font->line_height;
	fontw = font->line_height/2;
	/*	根据字符串长度计算刷新区域长宽	*/
	xlen = slen*fontw;
	ylen = fonth;
	uart_printf("framebuff %d, %d, %d\r\n", fonth, fontw, slen);
	uart_printf("framebuff %d, %d\r\n", xlen, ylen);
	
	framebuff = (u16*)wjq_malloc(xlen*ylen*sizeof(u16));//样点缓冲
	
	sidx = 0;
	/* framebuff 坐标*/
	s16 fbx, fby;
	fbx = 0;
	fby = 0;
	
	/*获取点阵，并转化为LCD像素*/
	while(1) {
		if (sidx >= slen) break;

		uint32_t chcode;
	
		/* 从字符串中取一个字符出来 */
		chr = font_get_ch_from_str(disstr+sidx, &chcode, st);
		uart_printf("chr %d, %x", chr, chcode);

		font->get_glyph_dsc(font, &glyph_dsc, chcode, NULL);
		bitmap = font->get_glyph_bitmap(font, chcode);

		/* 填点 left 和 top 两个参数都会出现负数 */
		fbx += glyph_dsc.ofs_x;
		
		if (glyph_dsc.ofs_y > font->base_line) yshift = 0;
		else yshift = glyph_dsc.ofs_y;
		if (yshift >= font->line_height) yshift = font->line_height-1;
		
		uart_printf("yshift %d", yshift);
		
		for (row = 0; row < glyph_dsc.box_h; row++) {
			
			buff_index = (row + yshift)*xlen + fbx;

			for (col = 0; col < glyph_dsc.box_w ; col++) {
				
				if(fdt == FONT_V_H_U_D_L_R) {
					tmp = dotbuf[col*2+row/8]&(0x80>>(row%8));
				} else if(fdt == FONT_V_L_L_R_U_D) {
					tmp = dotbuf[col+row/8*glyph_dsc.box_w]&(0x01<<(row%8));
				} else if(fdt == FONT_H_H_L_R_U_D) {
					tmp = dotbuf[col/8 + row*((glyph_dsc.box_w+7)/8)]&(0x80>>(col%8));
				}

				
				if (tmp != 0) {
					color = colidx;
				} else {
					color = BackColor;
				}
				
				framebuff[ buff_index + col ] = color;
			}
		}	

		fbx += glyph_dsc.box_w;
		
		sidx += chr;
	}

	if ( y + ylen > lcd->height) {
		/*显示超出屏幕*/
		ylen = lcd->height - y+1;//假设height = 240,y = 240, 也就意味着只显示一行
	}
	
	if (x + xlen >= lcd->width) {
		/*显示超出屏幕宽度*/
		col = lcd->width - x + 1;
		
		/*调整数据*/
		row = 1;
		while(1) {
			if (row >= ylen) break;
			memcpy(framebuff + row*col, framebuff+ row*xlen, 2*col);
			row++;
		}
		xlen = col;
	}

	lcd_fill(lcd, x, x + xlen-1, y, y + ylen-1, framebuff);

	wjq_free(framebuff);
	
	if (chbuf != NULL)
		wjq_free(chbuf);

	return 0;
}

/**
 *@brief:      dev_lcd_put_string
 *@details:    显示字符串，支持中文
 *@param[in]   无
 *@param[out]  无
 *@retval:       
 *@note          
 */
s32 display_lcd_put_string(DevLcdNode *lcd, petite_font_t *font, int x, int y, char *str, unsigned colidx)
{
	s32 res;

	if (lcd->fb == LCD_HAVE_FRAMEBUFF) {
		//uart_printf("lcd have framebuff!\r\n");
		display_put_str_to_framebuff(lcd, font, x, y, str, colidx);
	} else {
		//uart_printf("lcd no framebuff!\r\n");
		display_put_str_to_framebuff(lcd, font, x, y, str, colidx);
		//res = display_put_str_to_lcd(lcd, font, x, y, str, colidx);
	}
	return 0;	
}
extern void Delay(__IO uint32_t nTime);


/* BMP图片解析容器 */
typedef struct tagBITMAPFILEHEADER  //文件头  14B  
{ 
    u16  bfType;   //0x424d, "BM"
    u32  bfSize;   //文件大小，包含文件头
    u16  bfReserved1;   //保留字节
    u16  bfReserved2;   //保留字节
    u32  bfOffBits;   	//从文件头到实际位图数据的偏移
} BITMAPFILEHEADER; 

typedef struct tagBITMAPINFOHEADER  //位图信息头
{ 
    u32 biSize;   //本结构长度，也即是40
    s32 biWidth;  //图像宽度   
    s32 biHeight; //图像高度    
    u16 biPlanes; //1  
    u16 biBitCount;//1黑白二色图，4 16位色，8 256色，24 真彩色 
    u32 biCompression;   //是否压缩
    u32 biSizeImage;   //实际位图数据字节数
    s32 biXPelsPerMeter;  //目标设备水平分辨率 
    s32 biYPelsPerMeter;   //目标设备垂直分辨率
    u32 biClrUsed;  //图像实际用到颜色数，如为0，则用到的颜色数为2的biBitCount次方
    u32 biClrImportant;  //指定本图象中重要的颜色数，如果该值为零，则认为所有的颜色都是重要的
} BITMAPINFOHEADER;

/*调色板每个元素*/
typedef struct tagRGBQUAD
{ 
	u8    rgbBlue; //蓝色分量  
	u8    rgbGreen; //绿色分量    
	u8    rgbRed;   //红色分量  
	u8    rgbReserved;    
} RGBQUAD; 


#define WIDTHBYTES(i) ((i+31)/32*4)

/* 图片解析容器 */
struct _strBmpParse{
	BITMAPFILEHEADER    bf;
    BITMAPINFOHEADER    bi;

	/* 文件句柄 */
	int fd;
	u16 LineBytes;
	u16 NumColors;
    u32 ImgSize;
	u8 *palatte;
	/* 数据偏移 */
	int datashift;
};
/*
	创建解析容器 */
struct _strBmpParse *bmp_parse_creat(void)
{
	struct _strBmpParse *pbmpparse;

	pbmpparse = wjq_malloc(sizeof(struct _strBmpParse));

	return pbmpparse;
}
/*
	打开图片 */
int bmp_parse_open(struct _strBmpParse *pbmp, char *bmpname)
{
	u16 buf[40];;
	int rlen;
	
	wjq_log(LOG_DEBUG, "bmp open file:%s\r\n", bmpname);
	pbmp->fd = vfs_open(bmpname, O_RDONLY);

	if (pbmp->fd == NULL) {
		wjq_log(LOG_DEBUG, "bmp open file err\r\n");
		return -1;
	}
	rlen = vfs_read(pbmp->fd, (void *)buf, 14);
    
	pbmp->bf.bfType      = buf[0];
    pbmp->bf.bfSize      = buf[2];
    pbmp->bf.bfSize = (pbmp->bf.bfSize<<16)+buf[1];
    pbmp->bf.bfReserved1 = buf[3];
    pbmp->bf.bfReserved2 = buf[4];
    pbmp->bf.bfOffBits   = buf[6];
    pbmp->bf.bfOffBits = (pbmp->bf.bfOffBits<<16)+buf[5];
	
	wjq_log(LOG_DEBUG, "bf.bfType:%x\r\n", pbmp->bf.bfType);	
	wjq_log(LOG_DEBUG, "bf.bfSize:%d\r\n", pbmp->bf.bfSize);
	wjq_log(LOG_DEBUG, "bf.bfOffBits:%d\r\n", pbmp->bf.bfOffBits);

	rlen = vfs_read(pbmp->fd, (void *)buf, 40);

	pbmp->bi.biSize           = (unsigned long) buf[0];
    pbmp->bi.biWidth          = (long) buf[2];
    pbmp->bi.biHeight         = (long) buf[4];
    pbmp->bi.biPlanes         = buf[6];
    pbmp->bi.biBitCount       = buf[7];
    pbmp->bi.biCompression    = (unsigned long) buf[8];
    pbmp->bi.biSizeImage      = (unsigned long) buf[10];
    pbmp->bi.biXPelsPerMeter  = (long) buf[12];
    pbmp->bi.biYPelsPerMeter  = (long) buf[14];
    pbmp->bi.biClrUsed        = (unsigned long) buf[16];
    pbmp->bi.biClrImportant   = (unsigned long) buf[18];

	wjq_log(LOG_DEBUG, "bi.biSize:%d\r\n", pbmp->bi.biSize);	
	wjq_log(LOG_DEBUG, "bi.biWidth:%d\r\n", pbmp->bi.biWidth);
	wjq_log(LOG_DEBUG, "bi.biHeight:%d\r\n", pbmp->bi.biHeight);
	wjq_log(LOG_DEBUG, "bi.biPlanes:%d\r\n", pbmp->bi.biPlanes);
	wjq_log(LOG_DEBUG, "bi.biBitCount:%d\r\n", pbmp->bi.biBitCount);
	wjq_log(LOG_DEBUG, "bi.biCompression:%d\r\n", pbmp->bi.biCompression);
	wjq_log(LOG_DEBUG, "bi.biSizeImage:%d\r\n", pbmp->bi.biSizeImage);
	wjq_log(LOG_DEBUG, "bi.biXPelsPerMeter:%d\r\n", pbmp->bi.biXPelsPerMeter);
	wjq_log(LOG_DEBUG, "bi.biYPelsPerMeter:%d\r\n", pbmp->bi.biYPelsPerMeter);
	wjq_log(LOG_DEBUG, "bi.biClrUsed:%d\r\n", pbmp->bi.biClrUsed);
	wjq_log(LOG_DEBUG, "bi.biClrImportant:%d\r\n", pbmp->bi.biClrImportant);

	/*8个像素占用一个字节，不足一个字节补足一个字节*/
	/*单色图片四字节对齐*/
	pbmp->LineBytes = WIDTHBYTES(pbmp->bi.biWidth * pbmp->bi.biBitCount);
    pbmp->ImgSize   = (unsigned long) pbmp->LineBytes  * pbmp->bi.biHeight;

    wjq_log(LOG_DEBUG, "bmp w:%d,h:%d, bitcount:%d, linebytes:%d\r\n", 
				pbmp->bi.biWidth, pbmp->bi.biHeight, pbmp->bi.biBitCount, pbmp->LineBytes);
	
	if(pbmp->bi.biClrUsed!=0)
		pbmp->NumColors=(unsigned long)pbmp->bi.biClrUsed;//如果 bi.biClrUsed 不为零，就是本图象实际用到的颜色
	else {
	    switch(pbmp->bi.biBitCount)
	    {
	    //黑白屏用到两个调色板，一个是黑一个是白
	    case 1: pbmp->NumColors=2; break;
	        
	    case 4: pbmp->NumColors=16; break;
	        
	    case 8: pbmp->NumColors=256; break;
	        
	    case 24: pbmp->NumColors=0; break;
	        
	    default: vfs_close(pbmp->fd); return 2;
	    }
	}

	/* 读调色板 */
	if (pbmp->NumColors != 0) {
		pbmp->palatte = wjq_malloc(4*pbmp->NumColors);
		rlen = vfs_read(pbmp->fd, (void *)(pbmp->palatte), 4*pbmp->NumColors);
	} else {
		pbmp->palatte = NULL;
	}	

	pbmp->datashift = 14 + 40 + 4*pbmp->NumColors;

	return 0;
}

/* 返回的数据是RGB565
	读n行数据 
	图片取模:横向,左高右低
	sline：起始行
	linenum：本次读行数
	pdata：RGB565数据保存缓冲，size不能小于linenum*bi.biWidth个U16
	*/
int bmp_parse_read(struct _strBmpParse *pbmp, int sline, int linenum, u16 *pdata)
{
	u32 m,i, l, k;
	u16 r,g,b;
	u8 c;
	char *pcc;
	int rlen;
	
	pcc = wjq_malloc(pbmp->LineBytes*linenum);
	
	/* 读数据 */
	vfs_lseek(pbmp->fd, pbmp->datashift + sline*pbmp->LineBytes, SEEK_SET);
	rlen = vfs_read(pbmp->fd, (void *)pcc, pbmp->LineBytes*linenum);

	if(rlen <= 0) return -1;
	
	switch(pbmp->bi.biBitCount)
	{
	case 1:
		/*
			一个字节8个像素，高位在前
			调色板有256种颜色 		*/
		l = 0;	
		while (l < linenum) {
			k = l*pbmp->LineBytes;
			for(i=0;i < pbmp->bi.biWidth; i++) {	
				c = pcc[k + i/8]&(0x80>>(i%8));
				
				if(c != 0)
					*(pdata+l*pbmp->bi.biWidth + i) = WHITE;
				else
					*(pdata+l*pbmp->bi.biWidth + i) = BLACK;
			}
			l++;
		}
		break;
		
	case 4:
		/*4个bit 1个像素，要进行对U16的转换
			rgb565
		#define BLUE			 0x001F  
		#define GREEN			 0x07E0
		#define RED 			 0xF800
			*/
		l = 0;	
		while (l < linenum) {
			k = l*pbmp->LineBytes;
			for(i=0;i < pbmp->bi.biWidth; i++) {
				
				m = *(pcc+k+i/2);
			
				if (i%2 == 0)
					m = ((m>>4)&0x0f);
				else
					m = (m&0x0f);
				
				m = m*4;
				
				r = (pbmp->palatte[m+2] & 0xF8)>>3;
				g = (pbmp->palatte[m+1] & 0xFC)>>2;
				b = (pbmp->palatte[m] & 0xF8)>>3;
				
				*(pdata+l*pbmp->bi.biWidth + i) = (r<<11)|(g<<5)|(b<<0);
			}
			l++;
		}
		break;

	case 8:

		/*1个字节1个像素，要进行对U16的转换
		rgb565
	#define BLUE         	 0x001F  
	#define GREEN         	 0x07E0
	#define RED           	 0xF800
		*/
		l = 0;	
		while (l < linenum) {
			k = l*pbmp->LineBytes;
			for(i=0;i < pbmp->bi.biWidth; i++) {
				
				m = *(pcc+k);
				k++;
				m = m*4;
				
				r = (pbmp->palatte[m+2] & 0xF8)>>3;
				g = (pbmp->palatte[m+1] & 0xFC)>>2;
				b = (pbmp->palatte[m] & 0xF8)>>3;
				
				*(pdata+l*pbmp->bi.biWidth + i)  = (r<<11)|(g<<5)|(b<<0);

			}
			l++;
		}
		break;

	case 16:

		break;
		
	case 24://65K真彩色		

		/*3个字节1个像素，要进行对U16的转换
			rgb565
		#define BLUE         	 0x001F  
		#define GREEN         	 0x07E0
		#define RED           	 0xF800
		*/
		l = 0;	
		while (l < linenum) {
			k = l*pbmp->LineBytes;
			for(i=0;i < pbmp->bi.biWidth; i++) {
				
				b = pcc[k++];
				g = pcc[k++];
				r = pcc[k++];
				
				r = ((r<<8)&0xf800);
				g = ((g<<3)&0x07e0);
				b = ((b>>3)&0x001f);
				*(pdata+l*pbmp->bi.biWidth + i) = r+g+b;
			}
			l++;
		}
		
		break;

	case 32:
		break;
		
	default:
		break;
	} 

	wjq_free(pcc);

	return 0;
}
/* 
	删除bmp解析容器*/
int bmp_parse_del(struct _strBmpParse *pbmp)
{
	vfs_close(pbmp->fd);
	wjq_free(pbmp->palatte);
	wjq_free(pbmp);

	return 0;
}

/*
	在lcd上显示图片
*/
s32 dev_lcd_show_bmp(DevLcdNode *lcd, u16 x, u16 y, u16 xlen, u16 ylen, s8 *BmpFileName)
{

	struct _strBmpParse *pbmpparse;
	u8 linecnt = 20;//一次读多行，加快速度
	u16 *pdata;
	u32 j, k;
	int res;
	
	/* 创建一个BMP容器 */
	pbmpparse = bmp_parse_creat();
	if(pbmpparse == NULL) {
		Uprintf("creat bmp parse err!\r\n");
		return -1;
	}
	/* 配置容器的文件操作接口          need fix 直接调用*/
	
	/* 用容器打开一张图片 */
	res = bmp_parse_open(pbmpparse, BmpFileName);
	if(res != 0) return -1;
	/* 各种读数据接口 */

	if (xlen > pbmpparse->bi.biWidth) xlen = pbmpparse->bi.biWidth;
    if (ylen > pbmpparse->bi.biHeight) ylen = pbmpparse->bi.biHeight;

	pdata = wjq_malloc(pbmpparse->bi.biWidth*linecnt*sizeof(u16));
	
	lcd_prepare_display(lcd, x, x+xlen-1, y, y+ylen-1);	

	for (j=0; j<ylen;) {
		if(j+linecnt >= ylen)
			linecnt = ylen-j;
		
		res = bmp_parse_read(pbmpparse, j, linecnt, pdata);
		if(res < 0) break;
		
		k = 0;
		while(1) {
			if(k >= linecnt) break;
			lcd_flush(lcd, pdata + k* pbmpparse->bi.biWidth, xlen);
			k++;
		}
		
		j += linecnt;
	}
	
	lcd_update(lcd);
	wjq_free(pdata);
	
	/* 删除容器 */
	bmp_parse_del(pbmpparse);

	return 0;
}

#if 0
/**
 *@brief:      dev_lcd_test
 *@details:    LCD测试函数
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void dev_lcd_test(void)
{
	DevLcdNode *LcdCog = NULL;
	DevLcdNode *LcdOled = NULL;
	DevLcdNode *LcdOledI2C = NULL;
	DevLcdNode *LcdTft = NULL;

	/*  打开三个设备 */
	LcdCog = dev_lcd_open("spicoglcd");
	if (LcdCog==NULL)
		wjq_log(LOG_FUN, "open cog lcd err\r\n");

	LcdOled = dev_lcd_open("vspioledlcd");
	if (LcdOled==NULL)
		wjq_log(LOG_FUN, "open oled lcd err\r\n");
	
	LcdTft = dev_lcd_open("tftlcd");
	if (LcdTft==NULL)
		wjq_log(LOG_FUN, "open tft lcd err\r\n");

	LcdOledI2C = dev_lcd_open("i2coledlcd");
	if (LcdOledI2C==NULL)
		wjq_log(LOG_FUN, "open oled i2c lcd err\r\n");
	
	/*打开背光*/
	dev_lcd_backlight(LcdCog, 1);
	dev_lcd_backlight(LcdOled, 1);
	dev_lcd_backlight(LcdOledI2C, 1);
	dev_lcd_backlight(LcdTft, 1);

	#if 0/*不支持汉字时*/
	put_string(LcdCog, 5, 5, "spi cog lcd", BLACK);
	put_string(LcdOled, 5, 5, "vspi oled lcd", BLACK);
	put_string(LcdOledI2C, 5, 5, "i2c oled lcd", BLACK);
	put_string(LcdTft, 5, 5, "2.8 tft lcd", BLACK);
	#endif

	#if 1
	dev_lcd_put_string(LcdOled, "songti12", 10,1, "ABC-abc，", BLACK);
	dev_lcd_put_string(LcdOled, "siyuan16", 1, 13, "这是oled lcd", BLACK);
	dev_lcd_put_string(LcdOled, "songti12", 10,30, "www.wujique.com", BLACK);
	dev_lcd_put_string(LcdOled, "siyuan16", 1, 47, "屋脊雀工作室", BLACK);
	dev_lcd_update(LcdOled);
	dev_lcd_put_string(LcdCog, "songti12", 10,1, "ABC-abc，", BLACK);
	dev_lcd_put_string(LcdCog, "siyuan16", 1, 13, "这是cog lcd", BLACK);
	dev_lcd_put_string(LcdCog, "songti12", 10,30, "www.wujique.com", BLACK);
	dev_lcd_put_string(LcdCog, "siyuan16", 1, 47, "屋脊雀工作室", BLACK);
	dev_lcd_update(LcdCog);
	dev_lcd_put_string(LcdTft, "songti12", 20,30, "ABC-abc，", RED);
	dev_lcd_put_string(LcdTft, "siyuan16", 20,60, "这是tft lcd", RED);
	dev_lcd_put_string(LcdTft, "songti12", 20,100, "www.wujique.com", RED);
	dev_lcd_put_string(LcdTft, "siyuan16", 20,150, "屋脊雀工作室", RED);
	dev_lcd_update(LcdTft);
	dev_lcd_put_string(LcdOledI2C, "songti12", 10,1, "ABC-abc，", BLACK);
	dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,13, "这是LcdOledI2C", BLACK);
	dev_lcd_put_string(LcdOledI2C, "songti12", 10,30, "www.wujique.com", BLACK);
	dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,47, "屋脊雀工作室", BLACK);
	dev_lcd_update(LcdOledI2C);
	#endif
	
	while(1);
}



void dev_i2coledlcd_test(void)
{

	DevLcdNode *LcdOledI2C = NULL;

	wjq_log(LOG_FUN, "dev_i2coledlcd_test\r\n");

	LcdOledI2C = dev_lcd_open("i2coledlcd");
	if (LcdOledI2C==NULL)
		wjq_log(LOG_FUN, "open oled i2c lcd err\r\n");
	else
		wjq_log(LOG_FUN, "open oled i2c lcd suc\r\n");
	/*打开背光*/
	dev_lcd_backlight(LcdOledI2C, 1);

	dev_lcd_put_string(LcdOledI2C, "songti12", 10,1, "ABC-abc，", BLACK);
	dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,13, "这是LcdOledI2C", BLACK);
	dev_lcd_put_string(LcdOledI2C, "songti12", 10,30, "www.wujique.com", BLACK);
	dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,47, "屋脊雀工作室", BLACK);
	dev_lcd_update(LcdOledI2C);

	LcdOledI2C = dev_lcd_open("i2coledlcd2");
	if (LcdOledI2C==NULL)
		wjq_log(LOG_FUN, "open oled i2c2 lcd err\r\n");
	
	/*打开背光*/
	dev_lcd_backlight(LcdOledI2C, 1);
	while(1) {
		dev_lcd_put_string(LcdOledI2C, "songti12", 10,1, "ABC-abc，", BLACK);
		dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,13, "这是LcdOledI2C", BLACK);
		dev_lcd_put_string(LcdOledI2C, "songti12", 10,30, "www.wujique.com", BLACK);
		dev_lcd_put_string(LcdOledI2C, "siyuan16", 1,47, "屋脊雀工作室", BLACK);
		dev_lcd_update(LcdOledI2C);
		Delay(1000);
		dev_lcd_color_fill(LcdOledI2C, 1, 1000, 1, 1000, WHITE);
		dev_lcd_update(LcdOledI2C);
		Delay(1000);
	}

}

#endif


/*----end-----*/


