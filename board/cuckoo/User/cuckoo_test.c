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
#include "porting/lv_port_disp.h"
#include "demos/widgets/lv_demo_widgets.h"

#include "drv_config.h"

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


const VFSDIR SdFatFs=
{
	VFS_SD_DIR,
	FS_TYPE_FATFS,
	SYS_FS_FATFS_SD
};

DevLcdNode *LcdOledI2C = NULL;
void cuckoo_7b0_test(void)
{
	
	uint32_t tickstart,tmp;
  	uint32_t a,b;
	uint8_t lampsta = 0;
	wjq_log(LOG_DEBUG,"run app\r\n");

	#if 0//test
	esp8266_io_init();
	esp8266_uart_test();
	dev_ptHCHO_test();
	#endif
	
	sd_fatfs_init();
	/* 将SD卡文件系统挂载到VFS上 */
	vfs_add_node(&SdFatFs);

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

	#if 0//test
	DevLcdNode *LcdCogSpi = NULL;
	wjq_log(LOG_FUN, "cog lcd test\r\n");
	LcdCogSpi = dev_lcd_open("spicoglcd");
	if (LcdCogSpi==NULL)
		wjq_log(LOG_FUN, "open spi cog-lcd err\r\n");
	else
		wjq_log(LOG_FUN, "open spi cog-lcd suc\r\n");
	/*打开背光*/
	dev_lcd_backlight(LcdCogSpi, 1);
	dev_lcd_put_string(LcdCogSpi, "WQY_ST_12_H", 10,1, "cog lcd", BLACK);
	dev_lcd_put_string(LcdCogSpi, "WQY_ST_12_H", 10,30, "www.wujique.com", BLACK);
	dev_lcd_update(LcdCogSpi);
	#endif
	#if 0
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
	
	/* 初始化lvgl 
	注意，初始化LVGL的过程，会用到不少栈，
	如果在rtos的任务中进行初始化，注意任务栈的大小，
	防止溢出造成hardfault */
	#if 0
	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();
	lv_demo_widgets();
	#endif

	#if 0//测试WM8978
	fun_sound_play("mtd0/0:stereo_16bit_32k.wav", "wm8978");
	#endif
	camera_test();

	//tp_open();

	while(1) {

		osDelay(5);
		
		/* lvgl */
		lv_task_handler();
		/*测试LCD RGB565 */

		a++;
	
		#if 0
		if (a % 200 == 0) {	
			cpu_temp();
			cpu_rtc();

		}
		#endif
		
		if (a % 100 == 0) {
			b++;

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
		#if 0
			switch (b) {
				case 1:
					dev_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 10,1, "111111111111111111111111", BLACK);
					dev_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 10,30, "www.wujique.com", BLACK);
					dev_lcd_update(LcdOledI2C);
					break;
				case 2:
					dev_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 10,1, "2222222222222222222222222222222", BLACK);
					dev_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 10,30, "www.wujique.com", BLACK);
					dev_lcd_update(LcdOledI2C);
					break;
				case 3:
					dev_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 10,1, "33333333333333333333333333333333", BLACK);
					dev_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 10,30, "www.wujique.com", BLACK);
					dev_lcd_update(LcdOledI2C);
					break;
				case 4:
					dev_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 10,1, "4444444444444444444444444444444", BLACK);
					dev_lcd_put_string(LcdOledI2C, "WQY_ST_12_H", 10,30, "www.wujique.com", BLACK);
					dev_lcd_update(LcdOledI2C);
				default:
					b = 0;
					break;
			}
		#endif
		}

	}
}

#include "cmsis_os.h"


osThreadId_t TestTaskHandle;
const osThreadAttr_t TestTask_attributes = {
  .name = "TestTask",
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



