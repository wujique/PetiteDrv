
#include "mcu.h"
#include "log.h"
#include "board_sysconf.h"

#include <ft2build.h>
#include <freetype\freetype.h>
#include <freetype\ftglyph.h>

/*
	使用freetype加载矢量字库，ttf字体文件，通过字符的unicode，load一个字符的点阵！
	汉字的unicode使用32bit既可表达，比如‘’我‘’的unicode是0x6211
*/
#if 0
int FreeTypeTest1(int ucode[],int lengh)
{
	FT_Library    pFTLib         =  NULL;
	FT_Face        pFTFace         =  NULL;
	FT_Error    error         =   0 ;
	int           n;
	int num_chars;

	FT_Glyph    glyph;
	FT_BitmapGlyph    bitmap_glyph;
	FT_Bitmap bitmap;
				
	int  i ,j;
	int a=0;
	FT_UInt glyph_index;

	num_chars=lengh;
	num_chars = 1;
	
	uart_printf("FreeTypeTest\r\n");
	error  =  FT_Init_FreeType( & pFTLib);
	uart_printf("FT_Init_FreeType error=%d\r\n",error);
	
	/* 使用矢量字库有两种方式 ：1 矢量字库是文件，2 矢量字库保存在RAM*/
	error  =  FT_New_Face(pFTLib, "1:/Codepage.ttf",0,& pFTFace);
	//error = FT_New_Memory_Face(pFTLib, 	TTF, 64*1024, 0,&pFTFace);
	
	uart_printf("FT_New_Memory_Face error=%d\r\n",error);
	/*选择编码方式*/
	//FT_Select_Charmap(pFTFace, FT_ENCODING_GB2312);
	
	if ( ! error) {
        //error = FT_Set_Char_Size(pFTFace,  16 << 6 ,  16 << 6 ,  100 , 100 );
		error = FT_Set_Pixel_Sizes(pFTFace, 0, 16);
		uart_printf("FT_Set_Pixel_Sizes error=%d\r\n",error);
		for ( n = 0; n < num_chars; n++ ) {
			#if 1
			glyph_index = FT_Get_Char_Index( pFTFace, 0x0041 );
			uart_printf("FT_Get_Char_Index glyph_index=%d\r\n",glyph_index);
			
			error = FT_Load_Glyph( pFTFace, glyph_index, FT_LOAD_DEFAULT );
			uart_printf("FT_Load_Glyph error=%d\r\n",error);
			
			error  =  FT_Get_Glyph(pFTFace -> glyph,  & glyph);
			uart_printf("FT_Get_Glyph error=%d\r\n",error);
			
			error  =  FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL,  0 ,  1 );
			uart_printf("FT_Glyph_To_Bitmap error=%d\r\n",error);
			
			bitmap_glyph  =  (FT_BitmapGlyph)glyph;
			bitmap  =  bitmap_glyph -> bitmap;
			
			#else
			//error = FT_Load_Char( pFTFace, ucode[n], FT_LOAD_RENDER | FT_LOAD_MONOCHROME );
			//uart_printf("FT_Load_Char error=%d\r\n",error);
			 
			glyph_index = FT_Get_Char_Index( pFTFace, 0x0041 );
			uart_printf("FT_Get_Char_Index glyph_index=%d\r\n",glyph_index);
			
 			error = FT_Load_Glyph(pFTFace, glyph_index, FT_LOAD_DEFAULT ); 
			uart_printf("FT_Load_Glyph error=%d\r\n",error);
			
 			error = FT_Render_Glyph( pFTFace->glyph, FT_RENDER_MODE_MONO ); 
			uart_printf("FT_Render_Glyph error=%d\r\n",error);

			bitmap  =  pFTFace->glyph->bitmap;
			#endif
		   /* 生成出来的位图像素,每8个像素点便表示 face->glyph->bitmap->buffer[]里的一个字节 */
			 if (!error) {
				uart_printf( "bitmap.rows=%d\r\n" , bitmap.rows);
			    uart_printf( "bitmap.width=%d\r\n" , bitmap.width);
				
				 for ( i= 0 ; i < bitmap.rows;  ++ i) {
					for (j = 0 ; j < bitmap.width;  ++ j) {
						if( bitmap.buffer[i * bitmap.width + j])
						 	uart_printf("*");
						else
						 	uart_printf(" ");
						
					}
					
					 uart_printf( "\r" );
				 }
				 FT_Done_Glyph(glyph);
				 glyph  =  NULL;
			 }

		}
       FT_Done_Face(pFTFace);
        pFTFace  =  NULL;
	}
	FT_Done_FreeType(pFTLib);
     pFTLib  =  NULL;
	 return 0;
	
}
#endif
/*--------------------------------------------*/
#include "ft2build.h"
#include FT_FREETYPE_H


#include "petite_config.h"
#include "font/font.h"
#include "petite.h"
#include "drv_config.h"


static FT_Library g_ft_library=0;
static FT_Face g_ft_face=0;

/* 	打开矢量字库  	*/
void WIN_FontInit (void)
{
	Uprintf("WIN_FontInit!\r\n");
	
	FT_Error ft_error=FT_Init_FreeType (&g_ft_library);
	
	if (ft_error) {
		ft_error=0;
		Uprintf("ft init err!\r\n");
	} else	{
		/*
			chuheisong.ttf
			*/
		ft_error=FT_New_Face (g_ft_library, "mtd0/0:font/chuheisong.ttf", 0, &g_ft_face);
		
		if (ft_error) {
			Uprintf("ft new face err!\r\n");
			ft_error=0;
		}
	}
}

/*
	获取矢量字体
	*/
int WIN_GetWordData(u8 type, unsigned char *buff, int word, int buff_size)
{
  	unsigned long foffset=0; 

	u8 uni[3]={0};
	int  i ,j;
	int  time, time2;
	
	if (g_ft_face)	{
		FT_GlyphSlot slot = g_ft_face->glyph;
		
		Uprintf("FT_Set_Pixel_Sizes!\r\n");
		FT_Set_Pixel_Sizes(g_ft_face, 24, 24);
		
		Uprintf("FT_Load_Char!\r\n");
		/* 输入的字符是unicode编码：韦 0x97e6 */
		time = Stime_get_localtime();
		FT_Load_Char (g_ft_face, 0x97e6, FT_LOAD_RENDER|FT_LOAD_MONOCHROME);
		time2 = Stime_get_localtime();
		/* 测试转换时间 */
		Uprintf("FT_Load_Char time:%d!\r\n", time2-time);
		uart_printf( "bitmap.rows=%d\r\n" , slot->bitmap.rows);
	    uart_printf( "bitmap.width=%d\r\n" , slot->bitmap.width);
		uart_printf( "bitmap.pitch=%d\r\n" , slot->bitmap.pitch);
	    uart_printf( "bitmap_left=%d\r\n" , slot->bitmap_left);
		uart_printf( "bitmap_top=%d\r\n" , slot->bitmap_top);

		/* FT_LOAD_MONOCHROME 格式的显示方法 */
		for ( i= 0 ; i < slot->bitmap.rows;  i++) {
			for (j = 0 ; j < slot->bitmap.width;  j++) {
				if(( slot->bitmap.buffer[i * slot->bitmap.pitch + j/8]<<(j%8))&0X80)
				 	uart_printf("*");
				else
				 	uart_printf("-");
				
			}
			 uart_printf( "\r\n" );
		 }
		
	}	
	
}


int freetype_getdata(uint16_t pixw, int16_t pixh, struct _strBitmapHead* head, uint8_t *dotbuf, uint16_t unicode)
{
	unsigned long foffset=0; 

	u8 uni[3]={0};
	int  i ,j;
	int  time, time2;
	
	if (g_ft_face)	{
		FT_GlyphSlot slot = g_ft_face->glyph;
		
		Uprintf("FT_Set_Pixel_Sizes: %d, %d\r\n", pixw, pixh);
		uart_printf("unicode:%04x\r\n", unicode);
		FT_Set_Pixel_Sizes(g_ft_face, pixw, pixh);
		
		Uprintf("FT_Load_Char!\r\n");
		time = Stime_get_localtime();
		FT_Load_Char (g_ft_face, unicode, FT_LOAD_RENDER|FT_LOAD_MONOCHROME);
		time2 = Stime_get_localtime();
		/* 测试转换时间 */
		Uprintf("FT_Load_Char time:%d!\r\n", time2-time);
		uart_printf( "bitmap.rows=%d\r\n" , slot->bitmap.rows);
		uart_printf( "bitmap.width=%d\r\n" , slot->bitmap.width);
		uart_printf( "bitmap.pitch=%d\r\n" , slot->bitmap.pitch);
		uart_printf( "bitmap_left=%d\r\n" , slot->bitmap_left);
		uart_printf( "bitmap_top=%d\r\n" , slot->bitmap_top);

		head->rows = slot->bitmap.rows;
		head->width = slot->bitmap.width;
		head->pitch = slot->bitmap.pitch;
		head->left = slot->bitmap_left;
		head->top = slot->bitmap_top;

		memcpy(dotbuf, slot->bitmap.buffer, head->rows*head->pitch);

	}	
	
}

/* 
	在指定位置显示字符串
	输入的字符串是utf16格式
	*/
int freetype_showstr_unicode(uint16_t pixw, int16_t pixh, DevLcdNode *lcd, uint16_t x, uint16_t y, uint16_t *unicodestr)
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

	uint16_t baseline = pixh-2;

	while((*pStr) != 0){

		freetype_getdata(pixw, pixh, &BitmapHead, bitmaptmp,*pStr);

		/* 填点 left 和 top 两个参数都会出现负数 */
		lcdx += BitmapHead.left;
		
		if (BitmapHead.top > baseline) yshift = 0;
		else yshift = baseline - BitmapHead.top;
		//uart_printf("yshift:%d\r\n", yshift);
		if (yshift >= pixh) yshift = pixh-1;
		
		
		for(row=0; row<BitmapHead.rows; row++){
			
			for(col=0; col<BitmapHead.width; col++){
				dotdata = bitmaptmp[row * BitmapHead.pitch + col/8];
				
				if((dotdata & (0x80>>(col%8)) )!=0 ){
					uart_printf("*");
					drv_ST7565_drawpoint(lcd, lcdx + col, lcdy + row + yshift, BLACK);
				}else{
					uart_printf("-");
					drv_ST7565_drawpoint(lcd, lcdx + col, lcdy + row + yshift, WHITE);
				}
			}
			uart_printf("\r\n");
		}
		
		lcdx += BitmapHead.width;

		pStr++;
	}

	uart_printf("update\r\n");
	drv_ST7565_update(lcd);
	
	return 0;
}


void FreeTypeTest(DevLcdNode *lcd)
{
	uint16_t cha[4] = {0x97e6, 0x00};
	
	WIN_FontInit();	
	//WIN_GetWordData(1,(unsigned char *)1,1,1);
	
	freetype_showstr_unicode(12, 12, lcd, 1, 1, cha);
	freetype_showstr_unicode(16, 16, lcd, 1+13, 1+8, cha);
	freetype_showstr_unicode(24, 24, lcd, 1+13+16, 1+16, cha);
	freetype_showstr_unicode(32, 32, lcd, 1+13+16+24, 1+24, cha);
	while(1){
		osDelay(1000);		
	}
	
}

