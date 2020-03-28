

#include "mcu.h"
#include "log.h"
#include "board_sysconf.h"
#include "vfs.h"

RCC_ClocksTypeDef RCC_Clocks;


const VFSDIR SdFatFs=
{
	VFS_SD_DIR,
	FS_TYPE_FATFS,
	SYS_FS_FATFS_SD
};

const VFSDIR USBFatFs=
{
	VFS_USB_DIR,
	FS_TYPE_FATFS,
	SYS_FS_FATFS_USB
};

/*
	cpu初始化
*/
s32 board_mcu_preinit(void)
{
	/* Set the Vector Table base address at 0x08000000 */
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
		/*
			中断优先级分组
		*/	
		/* 2 bit for pre-emption priority, 2 bits for subpriority */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		  
		RCC_GetClocksFreq(&RCC_Clocks);
	#ifndef SYS_USE_RTOS
		/* SysTick end of count event */	
		SysTick_Config(RCC_Clocks.HCLK_Frequency / (1000/SYSTEMTICK_PERIOD_MS));
	#endif
		
		/*IO跟串口是调试信息的依赖，所以最早初始化*/
		mcu_io_init();
		mcu_uart_init();
		mcu_uart_open(PC_PORT);
		
		wjq_log(LOG_DEBUG, "***SYSCLK_Frequency:%d Hz\r\n", RCC_Clocks.SYSCLK_Frequency);
		wjq_log(LOG_DEBUG, "***HCLK_Frequency:%d Hz\r\n", RCC_Clocks.HCLK_Frequency);
		wjq_log(LOG_DEBUG, "***PCLK1_Frequency:%d Hz\r\n", RCC_Clocks.PCLK1_Frequency);
		wjq_log(LOG_DEBUG, "***PCLK2_Frequency:%d Hz\r\n", RCC_Clocks.PCLK2_Frequency);
}

s32 board_mcu_init(void)
{
	wjq_log(LOG_DEBUG, "***mcu: stm32f407 dev init!***\r\n");
	mcu_rtc_init();
}

s32 board_main_init(void)
{
	wjq_log(LOG_DEBUG, "***board: wujique other dev init!***\r\n");

	sys_dev_register();
	
	dev_key_init();
	
	#ifdef SYS_USE_KEYPAD
	dev_keypad_init();
	#endif
	
	dev_buzzer_init();
	dev_tea5767_init();
	dev_dacsound_init();

	dev_wm8978_init();
	dev_rs485_init();
	dev_touchscreen_init();
	dev_touchkey_init();
	dev_camera_init();
	dev_8266_init();

	mcu_adc_temprate_init();
	dev_htu21d_init();
	dev_ptHCHO_init();

	//dev_srf05_test();

	vfs_init();
	vfs_mount(&SdFatFs);

	font_hzfont_init();
	//sys_spiffs_mount_coreflash();
	//sys_lfs_mount();
	//lfs_test();

	usb_task_create();
	vfs_mount(&USBFatFs);
	wujique_407test_init();
	

	//eth_app_init();

	//fun_cmd_init();
	//mcu_timer_test();	
}


void board_low_task(void)
{
	//wjq_log(LOG_DEBUG, "low TASK ");
	
	dev_key_scan();
		
	#ifdef SYS_USE_KEYPAD
	dev_keypad_scan();
	#endif
	eth_loop_task();
	fun_sound_task();
	fun_rec_task();
	dev_touchkey_task();
	
}

