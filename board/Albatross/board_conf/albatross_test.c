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
#include "font.h"
#include "emenu.h"


extern u16 PenColor;
extern u16 BackColor;

#define TEST_FONG "songti12"


DevLcdNode * WJQTestLcd;

s32 wjq_wait_key(u8 key)
{

	return 0;
}

s32 wjq_test_showstr(char *s)
{
	wjq_log(LOG_DEBUG, "test:%s", s);
	dev_lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, BackColor);
	
	/*顶行居中显示父菜单*/
	dev_lcd_put_string(WJQTestLcd, TEST_FONG, 1, 32, s, PenColor);
	dev_lcd_update(WJQTestLcd);
	wjq_wait_key(0);
	
	return 0;
}	

s32 wjq_test(void)
{
	wjq_test_showstr((char *)__FUNCTION__);
	return 0;
}


#include "tslib.h"
extern struct tsdev *ts_open_module(void);
s32 test_tp_calibrate(void)
{
	DevLcdNode *lcd;

	dev_lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	/*顶行居中显示父菜单*/
	dev_lcd_put_string(WJQTestLcd, TEST_FONG, 1, 32, (char *)__FUNCTION__, BLACK);
	dev_lcd_update(WJQTestLcd);
	lcd = dev_lcd_open("tftlcd");
	if(lcd == NULL)
	{
		wjq_test_showstr("open lcd err!");	
	}
	else
	{
		dev_lcd_setdir(lcd, H_LCD, L2R_U2D);
		dev_touchscreen_open();
		//ts_calibrate(lcd);
		dev_touchscreen_close();
	}
	
	dev_lcd_color_fill(lcd, 1, 1000, 1, 1000, BLUE);
	dev_lcd_update(lcd);
	dev_lcd_close(lcd);
	
	return 0;
}


s32 test_tp_test(void)
{
	DevLcdNode *lcd;

	dev_lcd_color_fill(WJQTestLcd, 1, 1000, 1, 1000, WHITE);
	/*顶行居中显示父菜单*/
	dev_lcd_put_string(WJQTestLcd, TEST_FONG, 1, 32, (char *)__FUNCTION__, BLACK);
	dev_lcd_update(WJQTestLcd);
	lcd = dev_lcd_open("tftlcd");
	if(lcd == NULL)
	{
		wjq_test_showstr("open lcd err!");	
	}
	else
	{
		dev_lcd_setdir(lcd, H_LCD, L2R_U2D);
		dev_touchscreen_open();	
	
		struct tsdev *ts;
		//ts = ts_open_module();

		struct ts_sample samp[10];
		int ret;
		u8 i =0;	
		while(1)
		{
			//ret = ts_read(ts, samp, 10);
			if(ret != 0)
			{
				//uart_printf("pre:%d, x:%d, y:%d\r\n", samp[0].pressure, samp[0].x, samp[0].y);
						
				i = 0;
				
				while(1)
				{
					if(i>= ret)
						break;
					
					if(samp[i].pressure != 0 )
					{
						//uart_printf("pre:%d, x:%d, y:%d\r\n", samp.pressure, samp.x, samp.y);
						dev_lcd_drawpoint(lcd, samp[i].x, samp[i].y, RED); 
					}
					i++;
				}
			}

			u8 keyvalue;
			s32 res;
			
			//res = dev_keypad_read(&keyvalue, 1);
			if (res == 1) {
				if (keyvalue == 8) {
					dev_lcd_color_fill(lcd, 1, 1000, 1, 1000, BLUE);
					dev_lcd_update(lcd);
				} else if(keyvalue == 12) {
					break;
				}
			}
		}

		dev_touchscreen_close();
	}
	return 0;
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
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			/*
			MENU_L_2,//菜单等级
			"SPI COG",//中文
			"SPI COG",	//英文
			MENU_TYPE_FUN,//菜单类型
			test_spi_cog_display,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			*/
			MENU_L_2,//菜单等级
			"SPI tft",//中文
			"SPI tft",	//英文
			MENU_TYPE_FUN,//菜单类型
			//test_lcd_spi_128128,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			wjq_test,
			
			MENU_L_2,//菜单等级
			"tft",//中文
			"tft",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
	
			MENU_L_2,//菜单等级
			"图片测试",//中文
			"test BMP",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
			MENU_L_2,//菜单等级
			"字库测试",//中文
			"test Font",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行

		MENU_L_1,//菜单等级
		"声音",//中文
		"sound",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"蜂鸣器",//中文
			"buzzer",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"DAC音乐",//中文
			"DAC music",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"收音",//中文
			"FM",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"I2S音乐",//中文
			"I2S Music",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			MENU_L_2,//菜单等级
			"录音",//中文
			"rec",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行

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
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
			/*触摸按键*/
			MENU_L_2,//菜单等级
			"触摸按键",//中文
			"touch key",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行

			/*可以不测，本来测试程序就要使用矩阵按键*/
			MENU_L_2,//菜单等级
			"矩阵按键",//中文
			"keypad",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
		
		MENU_L_1,//菜单等级
		"摄像",//中文
		"camera",	//英文
		MENU_TYPE_FUN,//菜单类型
		wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
		
		MENU_L_1,//菜单等级
		"SPI FLASH",//中文
		"SPI FLASH",	//英文
		MENU_TYPE_LIST,//菜单类型
		NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			MENU_L_2,//菜单等级
			"核心板FLASH",//中文
			"core FLASH",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
			MENU_L_2,//菜单等级
			"底板 FLASH",//中文
			"board FLASH",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
		MENU_L_1,//菜单等级
		"通信",//中文
		"con",	//英文
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
				wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
				MENU_L_3,//菜单等级
				"485 发送",//中文
				"485 snd",	//英文
				MENU_TYPE_FUN,//菜单类型
				wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
			MENU_L_2,//菜单等级
			"CAN",//中文
			"CAN",	//英文
			MENU_TYPE_LIST,//菜单类型
			NULL,//菜单函数，功能菜单才会执行，有子菜单的不会执行
				MENU_L_3,//菜单等级
				"CAN 接收",//中文
				"CAN rec",	//英文
				MENU_TYPE_FUN,//菜单类型
				wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
				
				MENU_L_3,//菜单等级
				"CAN 发送",//中文
				"CAN snd",	//英文
				MENU_TYPE_FUN,//菜单类型
				wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
			
		
		
			MENU_L_2,//菜单等级
			"串口",//中文
			"uart",	//英文
			MENU_TYPE_FUN,//菜单类型
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行

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
			wjq_test,//菜单函数，功能菜单才会执行，有子菜单的不会执行
		

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


void albatross_750_test(void)
{
	wjq_log(LOG_DEBUG,"run app\r\n");

	//WJQTestLcd = dev_lcd_open("tftlcd");
	//WJQTestLcd = dev_lcd_open("spicoglcd");
	WJQTestLcd = dev_lcd_open("i2coledlcd");
	if (WJQTestLcd == NULL) {
		wjq_log(LOG_DEBUG, "emenu open lcd err\r\n");
	}

	//dev_keypad_open();
	//emenu_run(WJQTestLcd, (MENU *)&WJQTestList[0], sizeof(WJQTestList)/sizeof(MENU), TEST_FONG, 1, MENU_LANG_ENG);	

	/*打开背光*/
	dev_lcd_backlight(WJQTestLcd, 1);

	//dev_lcd_put_string(WJQTestLcd, "songti12", 10,1, "ABC-abc", BLACK);
	//dev_lcd_put_string(WJQTestLcd, "siyuan16", 1,13, "这是LcdOledI2C", BLACK);
	//dev_lcd_put_string(WJQTestLcd, "songti12", 10,30, "www.wujique.com", BLACK);
	//dev_lcd_put_string(WJQTestLcd, "siyuan16", 1,47, "屋脊雀工作室", BLACK);
	
	dev_lcd_put_string(WJQTestLcd, "songti12", 10,1, "ABC-abc", BLACK);
	//dev_lcd_put_string(WJQTestLcd, "siyuan16", 1,13, "LcdOledI2C", BLACK);
	dev_lcd_put_string(WJQTestLcd, "songti12", 10,30, "www.wujique.com", BLACK);
	//dev_lcd_put_string(WJQTestLcd, "siyuan16", 1,47, "Albatross", BLACK);
	dev_lcd_update(WJQTestLcd);
	
	while(1) {
		wjq_log(LOG_DEBUG, "Albatross_750_test run \r\n");
		Delay(1000);
	}
}

#include "FreeRtos.h"

TaskHandle_t  CanaryTaskHandle;

s32 albatross_750test_init(void)
{
	#if 0
	xTaskCreate(	(TaskFunction_t) albatross_750_test,
					(const char *)"canary stm103 test task",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
					(const configSTACK_DEPTH_TYPE) ALBATROSS_TASK_STK_SIZE,
					(void *) NULL,
					(UBaseType_t) ALBATROSS_TASK_PRIO,
					(TaskHandle_t *) &CanaryTaskHandle );	
	#else
	albatross_750_test();
	#endif				
	return 0;
}



