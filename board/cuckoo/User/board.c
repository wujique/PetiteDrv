
#include "mcu.h"
#include "log.h"
#include "board_sysconf.h"



/*
	板级初始化
*/
s32 board_init(void)
{
	wjq_log(LOG_DEBUG, "[   board] cuckoo other dev init!\r\n");

	petite_dev_register();

    lcd_reset();
    LCD_Config();

	//dev_camera_init();
	//dev_camera_show();
	//camera_test();

	//dev_wm8978_init();
	
	cap_touch_init();

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

		cap_touch_task();
	}


	fun_sound_task();
}




