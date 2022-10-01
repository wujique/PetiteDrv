
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
int WIN_GetWordData(u8 type,unsigned char *buff, int word, int buff_size)
{
  	unsigned long foffset=0; 
	u8 qh=word>>8;u8 ql=word&0xff;
	u8 gbk[3]={0};
	gbk[0]=word>>8;
	gbk[1]=word&0xff;
	u8 uni[3]={0};
	int  i ,j;
	int  time, time2;
	
	if (g_ft_face)	{
		FT_GlyphSlot slot = g_ft_face->glyph;
		
		int w_byte=slot->bitmap.pitch;
		u8 *buf=slot->bitmap.buffer;
		
		Uprintf("FT_Set_Pixel_Sizes!\r\n");
		FT_Set_Pixel_Sizes(g_ft_face, 24, 24);
		
		Uprintf("FT_Load_Char!\r\n");
		/* 输入的字符是unicode编码：韦 */
		time = Stime_get_localtime();
		FT_Load_Char (g_ft_face, 0x97e6, FT_LOAD_RENDER|FT_LOAD_MONOCHROME);
		time2 = Stime_get_localtime();
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


void FreeTypeTest(void)
{
	WIN_FontInit();	
	WIN_GetWordData(1,(unsigned char *)1,1,1);
}

