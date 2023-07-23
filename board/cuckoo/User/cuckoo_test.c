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
#include "petite.h"

#include "lvgl.h"
#include "lvgl_porting/lv_port_disp.h"
#include "demos/widgets/lv_demo_widgets.h"

#include "drv_config.h"
#include "display.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"


extern ADC_HandleTypeDef hadc2;

__align(8) double adcx;/*双精度浮点数*/
__align(8) double cpu_temp_sensor = 32.35;

int temp_a;
char testbuf[128];
__align(8) union{
	double x;
	char y[8];
}ut;

void cpu_temp(void)
{
	uint16_t TS_CAL1;
	uint16_t TS_CAL2;
	uint32_t   temp_adc;

	TS_CAL1 = *(__IO uint16_t *)(0x08FFF814);
	TS_CAL2 = *(__IO uint16_t *)(0x08FFF818);
	
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 100);
	temp_adc = HAL_ADC_GetValue(&hadc2);  /* 读取数值 */
	uart_printf("\r\nadc_v:%d, %d, %d\r\n", temp_adc, TS_CAL1, TS_CAL2);
	
	cpu_temp_sensor = (130.0 - 30.0)* (temp_adc - TS_CAL1) / (TS_CAL2 - TS_CAL1)  + 30;   /* 转换 */

	//uart_printf("double size:%d\r\n", sizeof(double));
	//uart_printf("stm32 temp:%f\r\n", cpu_temp_sensor);
	
	sprintf(testbuf, "%f", cpu_temp_sensor);
	uart_printf("sprintf:%s\r\n", testbuf);
	ut.x = cpu_temp_sensor;
	uart_printf("%02x %02x %02x %02x %02x %02x %02x %02x\r\n", ut.y[0],ut.y[1],ut.y[2],ut.y[3],ut.y[4],ut.y[5],ut.y[6],ut.y[7]);
	/*
	ut.x = 32.35;
	uart_printf("32.35:%02x %02x %02x %02x %02x %02x %02x %02x\r\n", ut.y[0],ut.y[1],ut.y[2],ut.y[3],ut.y[4],ut.y[5],ut.y[6],ut.y[7]);
	*/
	temp_a = (int)cpu_temp_sensor;
	uart_printf("(int):%d\r\n", temp_a);

}

extern RTC_HandleTypeDef hrtc;
void cpu_rtc(void)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	uart_printf("stime:%d-%d-%d\r\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

DevLcdNode *LcdOledI2C = NULL;

petite_font_t *wjqwqyst1212;
petite_font_t *wjqunicode1212;

void cuckoo_test_lcd(void)
{
	
	#if 0//test
	wjq_log(LOG_FUN, "dev_i2coledlcd_test\r\n");
	LcdOledI2C = dev_lcd_open("i2coledlcd");
	if (LcdOledI2C==NULL)
		wjq_log(LOG_FUN, "open oled i2c lcd err\r\n");
	else
		wjq_log(LOG_FUN, "open oled i2c lcd suc\r\n");
	/*打开背光*/
	dev_lcd_backlight(LcdOledI2C, 1);
	dev_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 10,1, "oled test", BLACK);
	dev_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 10,30, "www.wujique.com", BLACK);
	dev_lcd_update(LcdOledI2C);

	#endif

	#if 1//test
	DevLcdNode *LcdCogSpi = NULL;
	wjq_log(LOG_INFO, "cog lcd test\r\n");
	LcdCogSpi = lcd_open("spicoglcd");
	if (LcdCogSpi==NULL)
		wjq_log(LOG_INFO, "open spi cog-lcd err\r\n");
	else
		wjq_log(LOG_INFO, "open spi cog-lcd suc\r\n");
	/*打开背光*/
	lcd_backlight(LcdCogSpi, 1);
	/*登录,登錄,Login,Login,로그인,Anmeldung,S'identifier,Accesso,Iniciar sesión,Đăng nhập,Логин,Entrar,Masuk,Oturum aç,เข้าสู่ระบบ*/
	display_lcd_put_string(LcdCogSpi, wjqwqyst1212, 10,1, "中国www.wujique.com", BLACK);
	display_lcd_put_string(LcdCogSpi, wjqunicode1212, 10,14, "登录登錄로그인Accesso", BLACK);
	display_lcd_put_string(LcdCogSpi, wjqunicode1212, 10,27, "Iniciar sesión Đăng nhập", BLACK);
	display_lcd_put_string(LcdCogSpi, wjqunicode1212, 10,40, "Логин Entrar เข้าสู่ระบบ", BLACK);
	display_lcd_put_string(LcdCogSpi, &FontVga6X12, 10,52, "The font g test!", BLACK);
	lcd_update(LcdCogSpi);
	#endif
	
	#if 0//电子纸
	DevLcdNode *LcdEpaper = NULL;
	wjq_log(LOG_FUN, "e paper test\r\n");
	LcdEpaper = dev_lcd_open("spiE-Paper");
	if (LcdEpaper==NULL)
		wjq_log(LOG_FUN, "open e paper test err\r\n");
	else
		wjq_log(LOG_FUN, "open e paper test suc\r\n");
	/*打开背光*/
	dev_lcd_backlight(LcdEpaper, 1);
	dev_lcd_put_string(LcdEpaper, "WQY_ST_16_H", 10,1, "e paper test", BLACK);
	dev_lcd_put_string(LcdEpaper, "WQY_ST_16_H", 10,30, "www.wujique.com", BLACK);
	dev_lcd_update(LcdEpaper);
	#endif
}

void cuckoo_test_lcdtask(uint8_t b)
{
#if 0
	switch (b) {
		case 1:init_rgb565_buf(0xF800F800);//R
			break;
		case 2:init_rgb565_buf(0x001F001F);//B
			break;
		case 3:init_rgb565_buf(0x07E007E0);//G
			break;
		case 4:
			tickstart = HAL_GetTick();
			fill_rgb_buf_800_480_girl();
			tmp = HAL_GetTick();
			uart_printf("pass:%d ms\r\n", tmp-tickstart);
		default:
			b = 0;
			break;
	}
	//fill_rgb565buf(RGB565_480x272_PIC);
	//HAL_Delay(1000);

	//fill_rgb565buf(RGB565_480x272_PIC,0,0,480, 272);
	//HAL_Delay(1000);
#endif
#if 1
	switch (b%4) {
		case 0:
			display_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 1,1, "111111111111111111111111", BLACK);
			display_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 1,30, "www.wujique.com", BLACK);
			lcd_update(LcdOledI2C);
			break;
		case 1:
			display_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 1,1, "2222222222222222222222222222222", BLACK);
			display_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 1,30, "www.wujique.com", BLACK);
			lcd_update(LcdOledI2C);
			break;
		case 2:
			display_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 1,1, "33333333333333333333333333333333", BLACK);
			display_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 1,30, "www.wujique.com", BLACK);
			lcd_update(LcdOledI2C);
			break;
		case 3:
			display_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 1,1, "4444444444444444444444444444444", BLACK);
			display_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 1,30, "www.wujique.com", BLACK);
			lcd_update(LcdOledI2C);
		default:

			break;
	}
#endif

}


#include "src/font/lv_font.h"
#include "lv_tiny_ttf.h"

void tiny_tty_test(void)
{
#if 1
	lv_font_t *font_t;
	lv_font_glyph_dsc_t glyph_dsc;
	const uint8_t *bitmap;
	u16 unicode_ch[5] = {0x6211,0x662f, 0x4e2d, 0x56fd, 0x4eba};
	uint8_t i=0;
	int cnt = 0;
	int row=0;
	int col=0;
	bool getres;
	/* 使用字体文件创建一个lvgl定义的font_t，
		后续就用这个字体指针获取字符点阵。*/
	font_t = lv_tiny_ttf_create_file("/mtd0/chuheisong.ttf", 24);
	i = 0;
	while(1) {
	
		lcd_backlight(LcdOledI2C, 1);
		font_t->get_glyph_dsc(font_t, &glyph_dsc, unicode_ch[i], NULL);
		bitmap = font_t->get_glyph_bitmap(font_t, unicode_ch[i]);
		lcd_backlight(LcdOledI2C, 0);
		
		wjq_log(LOG_INFO, "adv_w:%d\r\n", glyph_dsc.adv_w);
		wjq_log(LOG_INFO, "box_w:%d\r\n", glyph_dsc.box_w);
		wjq_log(LOG_INFO, "box_h:%d\r\n", glyph_dsc.box_h);
		wjq_log(LOG_INFO, "ofs_x:%d\r\n", glyph_dsc.ofs_x);
		wjq_log(LOG_INFO, "ofs_y:%d\r\n", glyph_dsc.ofs_y);
		wjq_log(LOG_INFO, "bpp:%d\r\n", glyph_dsc.bpp);
		PrintFormat((u8 *)bitmap, 64);
		/* tinyttf 也就是stb_truetype库，生成的bitmap是8bit灰度值。
			AWTK GUI 二值化参数
			uint32_t threshold = font_size > 24? (font_size > 48 ? (font_size > 71 ? (font_size > 95 ? 195 : 175) : 160) : 118) : 95;
			*/
		cnt = 0;
		for(row=0; row< glyph_dsc.box_h; row++){
			for(col=0; col< glyph_dsc.box_w; col++){
				if(*(bitmap+cnt) > 118){
					uart_printf("*");
				} else {
					uart_printf(" ");
				}
				cnt++;
			}
			uart_printf("\r\n");
		}
		i++;
		if(i>=5) break;
		
		//osDelay(1000);
		
	}
#endif
}

#if 1
const char lua_test[] = {

    "print(\"Hello,I am lua!\\n--this is newline printf\")\n"\
    "function foo()\n"\
    "  local i = 0\n"\
    "  local sum = 1\n"\
    "  while i <= 10 do\n"\
    "    sum = sum * 2\n"\
    "    i = i + 1\n"\
    "  end\n"\
    "return sum\n"\
    "end\n"\
    "print(\"sum =\", foo())\n"\
    "print(\"and sum = 2^11 =\", 2 ^ 11)\n"\
    "print(\"exp(200) =\", math.exp(200))\n"\
    "print(\"lua test end!\")\n"
};

/* 运行Lua */
int do_lua_file_script(void)
{
	int res;
    lua_State *L;
    L = luaL_newstate(); /* 建立Lua运行环境 */
    luaL_openlibs(L);
    luaopen_base(L);
    res = luaL_loadstring(L, lua_test); /* 运行Lua脚本 */
	//res = luaL_loadfile(L, "/spiffs/lua_test.lua"); /* 运行Lua脚本 */
	if (res != 0) {
		wjq_log(LOG_DEBUG, "\r\n%s\r\n", lua_tostring(L, -1));
	}
	wjq_log(LOG_DEBUG, "load file=%d", res);
	res = lua_pcall(L, 0, LUA_MULTRET, 0);
	wjq_log(LOG_DEBUG, "pcall = %d", res);
    lua_close(L);
    return 0;
}
#endif

extern int flashdb_demo(void);
extern void petite_partition_test(void);

void cuckoo_7b0_test(void)
{
	
	uint32_t tickstart,tmp;
  	uint32_t a,b;
	uint8_t lampsta = 0;
	wjq_log(LOG_DEBUG,"run app\r\n");

	sd_fatfs_init();
	
	#if 0//test
	esp8266_io_init();
	esp8266_uart_test();
	dev_ptHCHO_test();
	petite_partition_test();
	flashdb_demo();
	#endif

	/* 测试littlefs */
	#if 1
	int filefd;
	filefd = vfs_open("/mtd0/bootcnt", O_CREAT);
	if(filefd > 0) {
		uint32_t bootcnt = 0;
		
		vfs_read(filefd, &bootcnt, sizeof(bootcnt));
		wjq_log(LOG_INFO, "boot cnt:%d\r\n", bootcnt);
		
		bootcnt++;
		
		vfs_lseek(filefd, 0, SEEK_SET);
		vfs_write(filefd, &bootcnt, sizeof(bootcnt));
		vfs_close(filefd);

	}
	#endif

	do_lua_file_script();
	while(1);
	
	/*把sd卡的矢量字库文件拷贝到spi flash*/
	//vfs_file_copy("/0:/font/chuheisong.ttf", "/mtd0/chuheisong.ttf");

	wjqwqyst1212 = font_wjq_create_from_file("0:/font/wqy12h18030.bin", 12);
	wjqunicode1212 = bitmapfont_create_from_file("0:/font/font_file.bin", 12);

	cuckoo_test_lcd();
	while(1);

	LcdOledI2C = lcd_open("spicoglcd");
	//LcdOledI2C = lcd_open("i2coledlcd");
	//font_test_utf16(LcdOledI2C);
	//font_unicode_bitmap_test(LcdOledI2C);//把整个bitmap字库在 COG lcd上顺序显示 
	emenu_test(LcdOledI2C, wjqwqyst1212);

	/* 测试tiny ttf 适量字库引擎 */
	//tiny_tty_test();

	/* 初始化lvgl 
	注意，初始化LVGL的过程，会用到不少栈，
	如果在rtos的任务中进行初始化，注意任务栈的大小，
	防止溢出造成hardfault */
	#if 1
	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();
	lv_demo_widgets();
	#endif

	#if 0//测试WM8978
	fun_sound_play("mtd0/0:sound/stereo_16bit_32k.wav", "wm8960");
	//fun_sound_play("mtd0/0:sound/stereo_16bit_32k.wav", "wm8978");
	#endif
	//camera_test();

	tp_open();

	while(1) {

		osDelay(2);
		
		/* lvgl */
		lv_task_handler();

		a++;
	
		#if 0
		if (a % 200 == 0) {	
			cpu_temp();
			cpu_rtc();
		}
		#endif
		
		if (a % 500 == 0) {
			b++;
			//cuckoo_test_lcdtask(b);
			
		}

	}
}

#include "cmsis_os.h"


osThreadId_t TestTaskHandle;
const osThreadAttr_t TestTask_attributes = {
  .name = "Te",
  .stack_size = CUCKOO_TASK_STK_SIZE,
  .priority = (osPriority_t) CUCKOO_TASK_PRIO,//osPriorityNormal,
};


s32 cuckoo_7b0_test_init(void)
{
	TestTaskHandle = osThreadNew(cuckoo_7b0_test, NULL, &TestTask_attributes);
					
	if(NULL != TestTaskHandle){
		wjq_log(LOG_INFO,"[    _app] freertos Scheduler\r\n");
	}else{
		wjq_log(LOG_INFO,"[    _app] xTaskCreate fail\r\n");
	}
	
	return 0;
}



