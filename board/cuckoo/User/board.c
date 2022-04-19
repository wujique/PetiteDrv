/*

	board.c定义有什么
	cuckoo.c就是怎么用这些东西。

	*/
#include "mcu.h"
#include "log.h"
#include "board_sysconf.h"

#include "touch.h"


extern const TouchDev CtpGt9147;
extern const TouchDev CtpGt1151;

extern void DCMI_PWDN_RESET_Init(void);
/*
	板级初始化
*/
s32 board_init(void)
{
	int res;
	
	wjq_log(LOG_DEBUG, "[   board] cuckoo other dev init!\r\n");

	petite_dev_register();

    lcd_reset();
    LCD_Config();

	//dev_wm8978_init();
	/*  触摸屏初始化 */
	res = tp_init(&CtpGt9147);
	/**/
	if (res == -1) {
		res = tp_init(&CtpGt1151);	
	}
	/**/
	if (res == -1) {
		/*  初始化电阻屏 */
	}
	/* 摄像头初始化 */
	DCMI_PWDN_RESET_Init();
	camera_init();

#if 0
	camera_open();
	OV2640_UXGAConfig();
	OV2640_RGB565_Mode();
	OV2640_OutSize_Set(480,272);
	//OV2640_OutSize_Set(800,480);
	HAL_Delay(200);
	camera_test();
#endif

	cuckoo_7b0_test_init();
	
	return 0;
}

/*
	低优先级轮训任务
	这是系统整个系统的底层轮询
	在这里的函数有以下要求：
	1 不能执行太长时间，更不要进行等待和delay
	2 临时变量不要使用太多，否则会任务栈溢出。
*/
extern void fun_sound_task(void);
extern void (*cap_touch_task)(void);

uint16_t touch_task_cnt = 0;
void board_low_task(uint8_t tick)
{
	
	//wjq_log(LOG_DEBUG, " low task ");
	touch_task_cnt += tick;
	if (touch_task_cnt >= 15) {
		touch_task_cnt = 0;
		if (cap_touch_task != NULL) {
			//wjq_log(LOG_DEBUG, "1");
			cap_touch_task();
		}
	}

	fun_sound_task();
}


/*--------------------这就是个垃圾桶 所有板相关的都可以先放这里----------------------*/
/*
	一个摄像头的DVP总线通常包含
	I2C
	PWDN,RESET

	VS,HS,PCLK,MCLK,D[0~11]，其中这些接口在STM32中叫做DCMI
	
	通常使用8bit的数据，比如OV2640；MT9V034则支持10bit，用高8bit也是可以的。
	
	stm32使用最新的HAL库，已经封装好很多接口，和原来STM32F407使用的老库不一样了。
	*/
#define DCMI_RESET_PORT GPIOC
#define DCMI_RESET_PIN GPIO_PIN_13

#define DCMI_PWDN_PORT GPIOE
#define DCMI_PWDN_PIN GPIO_PIN_3

/*
	复位摄像头
	*/
void DCMI_PWDN_RESET_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	HAL_GPIO_WritePin(DCMI_PWDN_PORT, DCMI_PWDN_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(DCMI_RESET_PORT, DCMI_RESET_PIN, GPIO_PIN_RESET);

   	GPIO_InitStructure.Pin = DCMI_RESET_PIN;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
   	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
   	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
   	HAL_GPIO_Init(DCMI_RESET_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = DCMI_PWDN_PIN;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
   	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
   	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
   	HAL_GPIO_Init(DCMI_PWDN_PORT, &GPIO_InitStructure);

	HAL_Delay(10);
	
	/*PWDN*/
	HAL_GPIO_WritePin(DCMI_PWDN_PORT, DCMI_PWDN_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	/* reset */
	//HAL_GPIO_WritePin(DCMI_RESET_PORT, DCMI_RESET_PIN, GPIO_PIN_RESET);
	//HAL_Delay(100);
	HAL_GPIO_WritePin(DCMI_RESET_PORT, DCMI_RESET_PIN, GPIO_PIN_SET);
	HAL_Delay(10);

}

/*
	摄像头补光灯，对焦*/



/*
	启动DCMI DMA传输，将图像数据传输到指定地址
	*/
extern uint32_t RGB565_480x272[LCD_WIDTH*LCD_HEIGHT/2];

void camera_test(void)
{
#if 1
	OV5640_RGB565_Mode();	
	OV5640_OutSize_Set(4, 0, LCD_WIDTH , LCD_HEIGHT); 
	OV5640_SET_SCPLL(2);
#else
	camera_open();
	OV2640_UXGAConfig();
	OV2640_RGB565_Mode();
	OV2640_OutSize_Set(LCD_WIDTH, LCD_HEIGHT);
#endif
	/* *2, 将长度转换为byte长度 */
	mcu_dcmi_captruce(0, (u32)RGB565_480x272, LCD_WIDTH*LCD_HEIGHT*2);

	return;
}


