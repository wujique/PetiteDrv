/**
 * @file                wujique_stm407.c
 * @brief           屋脊雀STM32F407开发板硬件测试程序
 * @author          wujique
 * @date            2018年1月29日 星期一
 * @version         初稿
 * @par             版权所有 (C), 2013-2023
 * @par History:
 * 1.日    期:        2018年1月29日 星期一
 *   作    者:         wujique
 *   修改内容:   创建文件
*/
#include "mcu.h"
#include "board_sysconf.h"

#include "log.h"
#include "petite.h"

#include "emenu.h"
#include "tslib.h"
#include "touch.h"

#include "wm8978.h"
#include "drv_lcd.h"
#include "soundplay.h"
#include "drv_touchkey.h"
#include "drv_keypad.h"
#include "drv_key.h"

extern petite_font_t *font_wjq_create_from_file(const char * path, uint16_t line_height);
extern petite_font_t FontVga6X12;
petite_font_t *WjqTestFont;
petite_font_t *WjqTestFont12;
petite_font_t *WjqTestFont16;
petite_font_t *WjqTestFont24;
petite_font_t *wjqunicode1212;

extern u16 PenColor;
extern u16 BackColor;


DevLcdNode * WJQTestLcd;

s32 wjq_wait_key(u8 key)
{
	while(1) {
		u8 keyvalue;
		s32 res;
		
		res = dev_keypad_read(&keyvalue, 1);
		if(res == 1) {
			if(key == 0) break;
			else if(keyvalue == key) break;	
		}
	}	
	return 0;
}

s32 wjq_test_showstr(char *s)
{
	wjq_log(LOG_DEBUG, "test:%s", s);
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, BackColor);
	
	/*顶行居中显示父菜单*/
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, s, PenColor);
	lcd_update(WJQTestLcd);
	wjq_wait_key(0);
	
	return 0;
}	
/**
 *@brief:      test_tft_display
 *@details:    测试TFT LCD
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 test_tft_display(void)
{
	DevLcdNode *lcd;
	u8 step = 0;
	u8 dis = 1;
	
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	
	/*顶行居中显示父菜单*/
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	
	lcd = lcd_open("tftlcd");
	if (lcd == NULL)	{
		wjq_test_showstr("open lcd err!");	
	} else {
		while(1) {
			if (dis == 1) {
				dis = 0;
				switch(step)
				{
					case 0:
						lcd_color_fill(lcd, 1, 1000, 1, 1000, YELLOW);
						lcd_update(lcd);
						break;
					case 1:
						lcd_color_fill(lcd, 1, 1000, 1, 1000, RED);
						lcd_update(lcd);
						break;
					case 2:
						lcd_color_fill(lcd, 1, 1000, 1, 1000, BLUE);
						display_lcd_put_string(lcd, WjqTestFont, 1, 120, "abc屋脊雀ADC123工作室12345678901234屋脊雀工作室", RED);
						lcd_update(lcd);

						break;
					default:
						break;
				}
				step++;
				if(step >= 3)
					step = 0;
			}
			u8 keyvalue;
			s32 res;
			
			res = dev_keypad_read(&keyvalue, 1);
			if (res == 1) {
				if(keyvalue == 16) {
					dis = 1;
				} else if(keyvalue == 12) {
					break;
				}
			}
		}
	
	}
		return 0;
}

s32 test_cogoled_lcd_display(char *name)
{
	DevLcdNode *lcd;
	u8 step = 0;
	u8 dis = 1;
	
	lcd = lcd_open(name);
	if (lcd == NULL) {
		wjq_test_showstr("open cog lcd err!");	
	} else {
		while(1) {
			if(dis == 1) {
				dis = 0;
				switch(step)
				{
					case 0:
						lcd_color_fill(lcd, 1, 1000, 1, 1000, BLACK);
						lcd_update(lcd);
						break;
					case 1:
						lcd_color_fill(lcd, 1, 1000, 1, 1000, WHITE);
						lcd_update(lcd);
						break;
					case 2:
						display_lcd_put_string(lcd, WjqTestFont, 1, 56, "abc屋脊雀ADC123工作室", BLACK);
						lcd_update(lcd);
						break;
						
					default:
						break;
				}
				step++;
				if(step >= 3)
					step = 0;
			}
			u8 keyvalue;
			s32 res;
			
			res = dev_keypad_read(&keyvalue, 1);
			if (res == 1) {
				if(keyvalue == 16) {
					dis = 1;
				} else if(keyvalue == 12) {
					break;
				}
			}
		}

	}
	
	return 0;
}
s32 test_i2c_oled_display(void)
{
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	/*顶行居中显示父菜单*/
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	return 	test_cogoled_lcd_display("i2coledlcd");
}
s32 test_vspi_oled_display(void)
{
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	/*顶行居中显示父菜单*/
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	return 	test_cogoled_lcd_display("vspioledlcd");
}

s32 test_spi_cog_display(void)
{
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	/*顶行居中显示父菜单*/
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	return 	test_cogoled_lcd_display("spicoglcd");
}
/*
	240*240 彩色TFT lcd 0x7735控制器
*/
s32 test_lcd_spi_128128(void)
{
	DevLcdNode *lcd;
	u8 step = 0;
	u8 dis = 1;
	
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	/*顶行居中显示父菜单*/
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	
	lcd = lcd_open("spitftlcd");
	if (lcd == NULL)	{
		wjq_test_showstr("open lcd err!");	
	} else	{
		lcd_setdir(lcd, W_LCD, L2R_D2U);
		
		while(1) {
			if (dis == 1) {
				dis = 0;
				switch(step)
				{
					case 0:
						lcd_color_fill(lcd, 1, 1000, 1, 1000, YELLOW);
						lcd_update(lcd);
						break;
					case 1:
						lcd_color_fill(lcd, 1, 1000, 1, 1000, RED);
						lcd_update(lcd);
						break;
					case 2:
						lcd_color_fill(lcd, 1, 1000, 1, 1000, BLUE);
						display_lcd_put_string(lcd, WjqTestFont, 1, 120, "abc屋脊雀ADC123工作室12345678901234屋脊雀工作室", RED);
						lcd_update(lcd);
						break;
					case 3:
						dev_lcd_show_bmp(lcd, 1, 1, 240, 240, "/1:/pic/pic128.bmp");
						break;
					default:
						break;
				}
				step++;
				if(step >= 4)
					step = 0;
			}
			u8 keyvalue;
			s32 res;
			
			res = dev_keypad_read(&keyvalue, 1);
			if (res == 1) {
				if (keyvalue == 16) {
					dis = 1;
				} else if(keyvalue == 12) {
					break;
				}
			}
		}
	
	}
	return 0;
}

s32 test_lcd_pic(void)
{
	DevLcdNode *lcd;
	u8 step = 0;
	u8 dis = 1;
	
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	/*顶行居中显示父菜单*/
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	lcd = lcd_open("tftlcd");
	if (lcd == NULL)	{
		wjq_test_showstr("open lcd err!");	
		return -1;
	}
	
	lcd_setdir(lcd, W_LCD, L2R_D2U);
	
	wjq_test_showstr((char *)__FUNCTION__);
	dev_lcd_show_bmp(lcd, 1, 1, 320, 240, "/1:/pic/girl_black.bmp");
	wjq_wait_key(16);
	dev_lcd_show_bmp(lcd, 1, 1, 320, 240, "/1:/pic/girl16.bmp");//调色板
	wjq_wait_key(16);
	dev_lcd_show_bmp(lcd, 1, 1, 320, 240, "/1:/pic/girl256.bmp");//调色板
	wjq_wait_key(16);
	dev_lcd_show_bmp(lcd, 1, 1, 320, 240, "/1:/pic/girl24.bmp");//真彩色
	wjq_wait_key(16);
	/*
	dev_lcd_show_bmp(lcd, 1, 1, 128, 128, "/1:/pic/pic128.bmp");
	wjq_wait_key(16);
	dev_lcd_show_bmp(lcd, 1, 1, 128, 64, "/1:/pic/PIC12864.bmp");
	wjq_wait_key(16);
	dev_lcd_show_bmp(lcd, 1, 1, 240, 240, "/1:/pic/pic240240.bmp");
	wjq_wait_key(16);
	*/
	lcd_setdir(lcd, W_LCD, L2R_U2D);
	
	return 0;
}

s32 test_lcd_font(void)
{
	wjq_test_showstr((char *)__FUNCTION__);
	return 0;
}


s32 test_sound_buzzer(void)
{
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	/*顶行居中显示父菜单*/
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	while(1) {
		u8 keyvalue;
		s32 res;
		
		res = dev_keypad_read(&keyvalue, 1);
		if (res == 1) {
			if(keyvalue == 16) {
				dev_buzzer_open();	
			} else if(keyvalue == (0x80+16)) {
				dev_buzzer_close();
			} else if(keyvalue == 12) {
				break;
			}
		}
			
	}
	return 0;
}

s32 test_sound_fm(void)
{
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	dev_wm8978_inout(WM8978_INPUT_DAC|WM8978_INPUT_AUX|WM8978_INPUT_ADC,
					WM8978_OUTPUT_PHONE|WM8978_OUTPUT_SPK);
	
	dev_tea5767_open();
	dev_tea5767_setfre(105700);
	wjq_wait_key(12);
	//dev_tea5767_close();
	return 0;
}

s32 test_sound_wm8978(void)
{
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	fun_sound_play("/1:/stereo_16bit_32k.wav", "wm8978");
	wjq_wait_key(16);
	fun_sound_stop();
	wjq_log(LOG_DEBUG,"wm8978 test out\r\n");
	return 0;
}

s32 test_sound_dac(void)
{
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	fun_sound_play("/1:/mono_16bit_8k.wav", "dacsound");
	wjq_wait_key(16);
	fun_sound_stop();
	wjq_log(LOG_DEBUG,"dac test out\r\n");
	return 0;
}
s32 test_sound_rec(void)
{
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	//fun_sound_rec("mtd0/1:rec8.wav");
	wjq_wait_key(16);
	//fun_rec_stop();
	fun_sound_play("/1:/rec8.wav", "wm8978");	
	while(1) {
		if(SOUND_IDLE == fun_sound_get_sta())
			break;

		u8 keyvalue;
		s32 res;
		
		res = dev_keypad_read(&keyvalue, 1);
		if (res == 1) {
			if(keyvalue == 12) {
				break;
			}
		}
	}
	wjq_log(LOG_DEBUG, "test_sound_rec OUT!\r\n");
	return 0;
}

s32 test_tp_calibrate(void)
{
	DevLcdNode *lcd;

	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	/*顶行居中显示父菜单*/
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	lcd = lcd_open("tftlcd");
	if(lcd == NULL) {
		wjq_test_showstr("open lcd err!");	
	} else	{
		lcd_setdir(lcd, H_LCD, L2R_U2D);
		tp_open();
		ts_calibrate(lcd);
		tp_close();
	}
	
	lcd_color_fill(lcd, 1, 1000, 1, 1000, BLUE);
	lcd_update(lcd);
	lcd_close(lcd);
	
	return 0;
}


s32 test_tp_test(void)
{
	DevLcdNode *lcd;

	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	/*顶行居中显示父菜单*/
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	lcd = lcd_open("tftlcd");
	if (lcd == NULL)	{
		wjq_test_showstr("open lcd err!");	
	} else {
		lcd_setdir(lcd, H_LCD, L2R_U2D);
		tp_open();	
	
		TouchPoint samp[10];
		int ret;
		u8 i =0;	
		while(1) {
			ret = rtp_get_point(samp, 10);
			if (ret != 0) {
				//uart_printf("pre:%d, x:%d, y:%d\r\n", samp[0].pressure, samp[0].x, samp[0].y);
						
				i = 0;
				
				while(1) {
					if (i>= ret) break;
					
					if (samp[i].pressure != 0 ) {
						//uart_printf("pre:%d, x:%d, y:%d\r\n", samp.pressure, samp.x, samp.y);
						lcd_drawpoint(lcd, samp[i].x, samp[i].y, RED); 
					}
					i++;
				}
			}

			u8 keyvalue;
			s32 res;
			
			res = dev_keypad_read(&keyvalue, 1);
			if (res == 1) {
				if (keyvalue == 8) {
					lcd_color_fill(lcd, 1, 1000, 1, 1000, BLUE);
					lcd_update(lcd);
				} else if(keyvalue == 12) {
					break;
				}
			}
		}

		tp_close();
	}
	return 0;
}


s32 test_key(void)
{
	u8 tmp;
	s32 res;
	u8 keyvalue;
		
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	dev_key_open();

	while(1) {
		res = dev_key_read(&tmp, 1);
		if(1 == res) {
			if (tmp == DEV_KEY_PRESS) {
				display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 10, "	   press	", BLACK);
				lcd_update(WJQTestLcd);
			} else if(tmp == DEV_KEY_REL) {
				display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 10, "	 release	  ", BLACK);
				lcd_update(WJQTestLcd);
			}
		}

		res = dev_keypad_read(&keyvalue, 1);
		if (res == 1) {
			if (keyvalue == 12) {
				break;
			}
		}
	}


	dev_key_close();
	return 0;
}


s32 test_camera(void)
{
	DevLcdNode *lcd;

	//dev_lcd_color_fill(emenulcd, 1, 1000, 1, 1000, WHITE);
	/*顶行居中显示父菜单*/
	//dev_lcd_put_string(emenulcd, FONT_SONGTI_1212, 1, 32, __FUNCTION__, BLACK);
	//dev_lcd_update(emenulcd);
	lcd = lcd_open("tftlcd");
	if (lcd == NULL) {
		wjq_test_showstr("open lcd err!");
		wjq_wait_key(16);
	} else {
		if (-1 == camera_open()) {
			wjq_log(LOG_DEBUG, "open camera err\r\n");
			return -1;
		}
		
		board_camera_show(lcd);
		
		wjq_wait_key(16);
		camera_close();
			
	}
	
	lcd_color_fill(lcd, 1, 1000, 1, 1000, BLUE);
	lcd_update(lcd);
	return 0;
}

s32 test_rs485_rec(void)
{
	u8 keyvalue;
	u8 buf[20];
	u8 len;
	s32 res;
	
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	//dev_rs485_open();

	while(1) {
		res = dev_keypad_read(&keyvalue, 1);
		if(res == 1) {
			if(keyvalue == 12){
				break;
			}
		}

		//len = dev_rs485_read(buf, sizeof(buf));
		if(len > 0)
		{
			buf[len] = 0;
			wjq_log(LOG_DEBUG,"%s", buf);
			memset(buf, 0, sizeof(buf));
		}

	}

	return 0;
}

s32 test_rs485_snd(void)
{
	u8 keyvalue;
	s32 res;
	
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	//dev_rs485_open();

	while(1)
	{
		res = dev_keypad_read(&keyvalue, 1);
		if(res == 1)
		{
			if(keyvalue == 12)
			{
				break;
			}
			else if(keyvalue == 16)
			{
				//res = dev_rs485_write("rs485 test\r\n", 14);
				wjq_log(LOG_DEBUG, "dev rs485 write:%d\r\n", res);
			}
		}
	}
	return 0;
}


s32 test_can_rec(void)
{
	wjq_test_showstr((char *)__FUNCTION__);
	return 0;
}
s32 test_can_snd(void)
{
	wjq_test_showstr((char *)__FUNCTION__);
	return 0;
}
s32 test_uart(void)
{
	u8 keyvalue;
	u8 buf[12];
    s32 len;
    s32 res;
	
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	while(1) {
		
	    //len =  mcu_uart_read (MCU_UART_3, buf, 10);
		if(len > 0) {
	    	//res = mcu_uart_write(MCU_UART_3, buf, len);
		}

		res = dev_keypad_read(&keyvalue, 1);
		if(res == 1) {
			if(keyvalue == 12)
			{
				break;
			}
		}
	}
	return 0;
}
/*

	简单测试，串口发送AT命令，收到OK即可。

*/
s32 test_esp8266(void)
{
	return 0;
}



s32 wjq_test(void)
{
	wjq_test_showstr((char *)__FUNCTION__);
	return 0;
}

s32 test_spiflash_board(void)
{
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	
	dev_spiflash_test_chipcheck("board_spiflash");
	wjq_wait_key(12);
	return 0;
}

s32 test_spiflash_core(void)
{
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	
	dev_spiflash_test_chiperase("core_spiflash");
	wjq_wait_key(12);
	return 0;
}

s32 test_touchkey(void)
{
	u8 tmp;
	s32 res;
	u8 keyvalue;
		
	lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 32, (char *)__FUNCTION__, BLACK);
	lcd_update(WJQTestLcd);
	
	dev_touchkey_open();

	while(1)
	{
		res = dev_touchkey_read(&tmp, 1);
		if(1 == res)
		{
			if(tmp == DEV_TOUCHKEY_TOUCH)
			{
				wjq_log(LOG_DEBUG, "touch key test get a touch event!\r\n");
				display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 10, "      touch    ", BLACK);
				lcd_update(WJQTestLcd);
			}
			else if(tmp == DEV_TOUCHKEY_RELEASE)
			{
				wjq_log(LOG_DEBUG, "touch key test get a release event!\r\n");
				display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 10, "      release    ", BLACK);
				lcd_update(WJQTestLcd);
			}
		}

		res = dev_keypad_read(&keyvalue, 1);
		if(res == 1)
		{
			if(keyvalue == 12)
			{
				break;
			}
		}
	}


	dev_touchkey_close();
	return 0;
}


s32 test_keypad(void)
{
	u8 dis_flag = 1;
	u8 keyvalue;
	s32 res;
	s32 esc_flag = 1;
	
	char testkeypad1[]="1 2 3 F1 ";
	char testkeypad2[]="4 5 6 DEL";
	char testkeypad3[]="7 8 9 ESC";
	char testkeypad4[]="* 0 # ENT";
	
	dev_keypad_open();

	while(1)
	{
		if(dis_flag == 1)
		{
			lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
			display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 1, (char *)__FUNCTION__, BLACK);
			display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 13, testkeypad1, BLACK);
			display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 26, testkeypad2, BLACK);
			display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 39, testkeypad3, BLACK);
			display_lcd_put_string(WJQTestLcd, WjqTestFont, 1, 52, testkeypad4, BLACK);
			lcd_update(WJQTestLcd);
			dis_flag = 0;
		}
		
		res = dev_keypad_read(&keyvalue, 1);
		if(res == 1 && ((keyvalue & KEYPAD_PR_MASK) == 0))
		{
			dis_flag = 1;
			switch(keyvalue)
			{
				case 1:
					testkeypad1[0] = ' ';
					break;
				case 2:
					testkeypad1[2] = ' ';
					break;
				case 3:
					testkeypad1[4] = ' ';
					break;
				case 4:
					testkeypad1[6] = ' ';
					testkeypad1[7] = ' ';
					testkeypad1[8] = ' ';
					break;
				case 5:
					testkeypad2[0] = ' ';
					break;
				case 6:
					testkeypad2[2] = ' ';
					break;
				case 7:
					testkeypad2[4] = ' ';
					break;
				case 8:
					testkeypad2[6] = ' ';
					testkeypad2[7] = ' ';
					testkeypad2[8] = ' ';
					break;
				case 9:
					testkeypad3[0] = ' ';
					break;
				case 10:
					testkeypad3[2] = ' ';
					break;
				case 11:
					testkeypad3[4] = ' ';
					break;
				case 12:
					if(esc_flag == 1)
					{
						esc_flag = 0;
						testkeypad3[6] = ' ';
						testkeypad3[7] = ' ';
						testkeypad3[8] = ' ';
					}
					else
					{
						return 0;
					}
					break;

				case 13:
					testkeypad4[0] = ' ';
					break;
				case 14:
					testkeypad4[2] = ' ';
					break;
				case 15:
					testkeypad4[4] = ' ';
					break;
				case 16:
					testkeypad4[6] = ' ';
					testkeypad4[7] = ' ';
					testkeypad4[8] = ' ';
					break;
			} 
		}
	}

}


const MENU WJQTestList[]=
{
	MENU_L_0,//菜单等级
	"测试程序",//中文
	"test",	//英文
	MENU_TYPE_KEY_2COL,//菜单类型
	NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"LCD",//中文
		"LCD",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			/*
			MENU_L_2,//菜单等级
			"VSPI OLED",//中文
			"VSPI OLED",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_vspi_oled_display,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			*/
			MENU_L_2,//菜单等级
			"I2C OLED",//中文
			"I2C OLED",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_i2c_oled_display,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			/*
			MENU_L_2,//菜单等级
			"SPI COG",//中文
			"SPI COG",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_spi_cog_display,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			*/
			MENU_L_2,//菜单等级
			"SPI oled/cog",//中文
			"SPI oled/cog",	//英文
			MENU_TYPE_FUN,//菜单类型
			//test_lcd_spi_128128,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			test_spi_cog_display,
			
			MENU_L_2,//菜单等级
			"tft",//中文
			"tft",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_tft_display,//菜单函数，功能菜单才会执行，有子菜单的不会执行
	
			MENU_L_2,//菜单等级
			"图片测试",//中文
			"test BMP",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_lcd_pic,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
			MENU_L_2,//菜单等级
			"字库测试",//中文
			"test Font",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_lcd_font,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"声音",//中文
		"sound",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"蜂鸣器",//中文
			"buzzer",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_sound_buzzer,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"DAC音乐",//中文
			"DAC music",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_sound_dac,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"收音",//中文
			"FM",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_sound_fm,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"I2S音乐",//中文
			"I2S Music",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_sound_wm8978,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"录音",//中文
			"rec",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_sound_rec,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"触摸屏",//中文
		"tp",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
			MENU_L_2,//菜单等级
			"校准",//中文
			"calibrate",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_tp_calibrate,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"测试",//中文
			"test",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_tp_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
		MENU_L_1,//菜单等级
		"按键",//中文
		"KEY",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"核心板按键",//中文
			"core KEY",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_key,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
			/*触摸按键*/
			MENU_L_2,//菜单等级
			"触摸按键",//中文
			"touch key",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_touchkey,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			/*可以不测，本来测试程序就要使用矩阵按键*/
			MENU_L_2,//菜单等级
			"矩阵按键",//中文
			"keypad",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_keypad,//菜单函数，功能菜单才会执行，有子菜单的不会执行
		
		MENU_L_1,//菜单等级
		"摄像",//中文
		"camera",	//英文
		MENU_TYPE_FUN,//菜单类型
		test_camera,//菜单函数，功能菜单才会执行，有子菜单的不会执行
		
		MENU_L_1,//菜单等级
		"SPI FLASH",//中文
		"SPI FLASH",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"核心板FLASH",//中文
			"core FLASH",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_spiflash_core,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
			MENU_L_2,//菜单等级
			"底板 FLASH",//中文
			"board FLASH",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_spiflash_board,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
		MENU_L_1,//菜单等级
		"通信",//中文
		"com",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"485",//中文
			"485",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
				MENU_L_3,//菜单等级
				"485 接收",//中文
				"485 rec",	//英文
				MENU_TYPE_FUN,//菜单类型
				test_rs485_rec,//菜单函数，功能菜单才会执行，有子菜单的不会执行
				MENU_L_3,//菜单等级
				"485 发送",//中文
				"485 snd",	//英文
				MENU_TYPE_FUN,//菜单类型
				test_rs485_snd,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
			MENU_L_2,//菜单等级
			"CAN",//中文
			"CAN",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
				MENU_L_3,//菜单等级
				"CAN 接收",//中文
				"CAN rec",	//英文
				MENU_TYPE_FUN,//菜单类型
				test_can_rec,//菜单函数，功能菜单才会执行，有子菜单的不会执行
				
				MENU_L_3,//菜单等级
				"CAN 发送",//中文
				"CAN snd",	//英文
				MENU_TYPE_FUN,//菜单类型
				test_can_snd,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
		
		
			MENU_L_2,//菜单等级
			"串口",//中文
			"uart",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_uart,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"网络",//中文
			"eth",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"OTG",//中文
			"OTG",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
				MENU_L_3,//菜单等级
				"HOST",//中文
				"HOST",	//英文
				MENU_TYPE_LIST,//菜单类型
				NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
				MENU_L_3,//菜单等级
				"Device",//中文
				"Device",	//英文
				MENU_TYPE_LIST,//菜单类型
				NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
		MENU_L_1,//菜单等级
		"模块",//中文
		"mod",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
		
			/*外扩接口测试*/
			MENU_L_2,//菜单等级
			"RF24L01",//中文
			"RF24L01",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"MPU6050",//中文
			"MPU6050",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			/*用串口外扩8266模块*/
			MENU_L_2,//菜单等级
			"wifi",//中文
			"wifi",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_esp8266,//菜单函数，功能菜单才会执行，有子菜单的不会执行
		

		MENU_L_1,//菜单等级
		"test",//中文
		"test",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
		
		MENU_L_1,//菜单等级
		"test1",//中文
		"test1",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"test2",//中文
		"test2",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"test3",//中文
		"test3",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"test4",//中文
		"test4",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"test5",//中文
		"test5",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"test6",//中文
		"test6",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"test7",//中文
		"test7",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"test8",//中文
		"test8",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"test9",//中文
		"test9",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"test10",//中文
		"test10",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
		
			MENU_L_2,//菜单等级
			"t10-1",//中文
			"t10-1",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"t10-2",//中文
			"t10-2",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"t10-3",//中文
			"t10-3",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"t10-4",//中文
			"t10-4",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"t10-5",//中文
			"t10-5",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"t10-6",//中文
			"t10-6",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"t10-7",//中文
			"t10-7",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"t10-8",//中文
			"t10-8",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"t10-9",//中文
			"t10-9",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"t10-10",//中文
			"t10-10",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"t10-11",//中文
			"t10-11",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"t10-12",//中文
			"t10-12",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
	
	/*最后的菜单是结束菜单，无意义*/			
	MENU_L_0,//菜单等级
	"END",//中文
	"END",	//英文
	MENU_TYPE_NULL,//菜单类型
	NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
};

/*
	测试程序，相当于一个APP线程
	
*/
void wujique_stm407_test(void)
{
	wjq_log(LOG_DEBUG,"\r\n\r\n\r\n---run board test\r\n\r\n\r\n\r\n");

	/* 这四个字库是 WJQ GBK 排布 */
	WjqTestFont = font_wjq_create_from_file("1:/font/wqy12h18030.bin", 12);
	WjqTestFont16 = font_wjq_create_from_file("1:/font/wqy16h18030.bin", 16);
	WjqTestFont24 = font_wjq_create_from_file("1:/font/syst_m_2424_18030.bin", 24);//syst_m_2424_big5

	wjqunicode1212 = bitmapfont_create_from_file("1:/font/font_file.bin", 12);
	
	WJQTestLcd = lcd_open("tftlcd");
	//WJQTestLcd = lcd_open("spicoglcd");
	//WJQTestLcd = lcd_open("i2coledlcd");
	//WJQTestLcd = lcd_open("spitftlcd");
	//WJQTestLcd = lcd_open("spiE-Paper");
	
	if (WJQTestLcd == NULL) {
		wjq_log(LOG_DEBUG, "emenu open lcd err\r\n");
	}
	#if 1
	lcd_backlight(WJQTestLcd, 1);
	/*登录,登錄,Login,Login,로그인,Anmeldung,S'identifier,Accesso,Iniciar sesión,Đăng nhập,Логин,Entrar,Masuk,Oturum aç,เข้าสู่ระบบ*/
	display_lcd_put_string(WJQTestLcd, WjqTestFont, 10,1, "中国www.wujique.com", BLACK);
	display_lcd_put_string(WJQTestLcd, wjqunicode1212, 10,14, "登录登錄로그인Accesso", BLACK);
	display_lcd_put_string(WJQTestLcd, wjqunicode1212, 10,27, "Iniciar sesión Đăng nhập", BLACK);
	display_lcd_put_string(WJQTestLcd, wjqunicode1212, 10,40, "Логин Entrar เข้าสู่ระบบ", BLACK);
	display_lcd_put_string(WJQTestLcd, &FontVga6X12, 10,52, "The font g test!", BLACK);
	
	lcd_update(WJQTestLcd);
	while(1);
	#endif
	
	dev_key_open();
	dev_keypad_open();

	//dev_wm8978_test();

	emenu_run(WJQTestLcd, (MENU *)&WJQTestList[0], sizeof(WJQTestList)/sizeof(MENU), WjqTestFont24, 1, MENU_LANG_CHA);	
	while(1) { }
}


/*

生产测试

*/
#if 0
s32 test_tft_lcd(void)
{
	DevLcdNode *lcd;
	u8 step = 0;
	u8 dis = 1;
	
	lcd = dev_lcd_open("tftlcd");
	if(lcd == NULL) {
		wjq_test_showstr("open tft lcd err!");	
	} else {
		while(1)
		{
			if(dis == 1)
			{
				dis = 0;
				switch(step)
				{
					case 0:
						dev_lcd_color_fill(lcd, 1, 1000, 1, 1000, YELLOW);
						dev_lcd_update(lcd);
						break;
					
					case 1:
						dev_lcd_color_fill(lcd, 1, 1000, 1, 1000, RED);
						dev_lcd_update(lcd);
						break;
					
					case 2:
						dev_lcd_color_fill(lcd, 1, 1000, 1, 1000, BLUE);
						dev_lcd_put_string(lcd, TEST_FONG, 20, 20, "abc屋脊雀工作室ADC", RED);
						dev_lcd_update(lcd);
						break;

					case 3:
						dev_lcd_show_bmp(lcd, 1, 1, 320, 240, "1:pic/女人单色.bmp");
						break;
					
					case 4:
						dev_lcd_show_bmp(lcd, 1, 1, 320, 240, "1:pic/女人24位.bmp");//真彩色
						break;
					case 5:
						dev_lcd_backlight(lcd, 0);
						break;
					case 6:
						dev_lcd_backlight(lcd, 1);
						break;		
					default:
						break;
				}
				step++;
				if(step >= 7)
					step = 0;
			}
			u8 keyvalue;
			s32 res;
			
			res = dev_keypad_read(&keyvalue, 1);
			if(res == 1)
			{
				if(keyvalue == 16)
				{
					dis = 1;
				}
				else if(keyvalue == 12)
				{
					break;
				}
			}
		}

	}
	dev_lcd_close(lcd);
	return 0;
}
#endif

#if 0
s32 test_cog_lcd(void)
{
	DevLcdNode *lcd;
	u8 step = 0;
	u8 dis = 1;
	
	lcd = dev_lcd_open("spicoglcd");
	if (lcd == NULL) {
		wjq_test_showstr("open cog lcd err!");	
		while(1);
	}

	while(1) {
		dev_lcd_color_fill(lcd, 1, 1000, 1, 1000, BLACK);
		dev_lcd_update(lcd);
		wjq_wait_key(16);
		dev_lcd_color_fill(lcd, 1, 1000, 1, 1000, WHITE);
		dev_lcd_update(lcd);
		wjq_wait_key(16);
		dev_lcd_put_string(lcd, TEST_FONG, 1, 32, "cog LCD测试程序", BLACK);
		dev_lcd_update(lcd);
		wjq_wait_key(16);
		dev_lcd_backlight(lcd, 0);
		wjq_wait_key(16);
		dev_lcd_backlight(lcd, 1);
		wjq_wait_key(16);
	}
	
	return 0;
}
#endif

#if 0
s32 test_tft_tp(void)
{
	DevLcdNode *lcd;

	lcd = dev_lcd_open("tftlcd");
	if(lcd == NULL) {
		wjq_test_showstr("open lcd err!");	
	} else {
		dev_lcd_backlight(lcd, 1);
		
		dev_lcd_color_fill(lcd, 1, 1000, 1, 1000, BLUE);
		dev_lcd_update(lcd);
		dev_lcd_setdir(lcd, H_LCD, L2R_U2D);
		tp_open();
		ts_calibrate(lcd);
		tp_close();
	}
	
	dev_lcd_color_fill(lcd, 1, 1000, 1, 1000, BLUE);
	dev_lcd_update(lcd);
	

	tp_open();

	TouchPoint samp[10];
	int ret;
	u8 i =0;	
	while(1) {
		ret = rtp_get_point(samp, 10);
		if (ret != 0) {
			//uart_printf("pre:%d, x:%d, y:%d\r\n", samp[0].pressure, samp[0].x, samp[0].y);
					
			i = 0;
			
			while(1) {
				if (i>= ret) break;
				
				if (samp[i].pressure != 0 ) {
					//uart_printf("pre:%d, x:%d, y:%d\r\n", samp.pressure, samp.x, samp.y);
					dev_lcd_drawpoint(lcd, samp[i].x, samp[i].y, 0xF800); 
				}
				i++;
			}
		}

	}

	tp_close();

	return 0;
}
#endif

