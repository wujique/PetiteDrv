#include "mcu.h"
#include "log.h"
#include "board_sysconf.h"
#include "vfs.h"


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
	板级初始化
*/
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

/*
	低优先级轮训任务
*/
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

