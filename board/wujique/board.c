#include "mcu.h"

#include "board_sysconf.h"
#include "log.h"

#include "vfs.h"
#include "drv_keypad.h"
#include "drv_buzzer.h"
#include "drv_dacsound.h"


/* 硬件相关的都在board.c定义
	调用对应的驱动进行初始化。
	不能在驱动中包含硬件相关信息。
*/
/*
	矩阵按键硬件定义
	row输出，放前面
*/
KeyPadIO KeyPadIOList[KEY_PAD_ROW_NUM+KEY_PAD_COL_NUM]=
		{
			/*ROW*/
			{MCU_PORT_F, MCU_IO_12},
			{MCU_PORT_F, MCU_IO_13},
			{MCU_PORT_F, MCU_IO_14},
			{MCU_PORT_F, MCU_IO_15},
			/*COL*/
			{MCU_PORT_F, MCU_IO_11},
			{MCU_PORT_F, MCU_IO_10},
			{MCU_PORT_F, MCU_IO_9},
			{MCU_PORT_F, MCU_IO_8},
		};
/*
	定义文件系统节点
			*/
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
/**/
DevBuzzer BoardBuzzer={
		MCU_PORT_D,
		MCU_IO_13, 
		MCU_TIMER_4,
		MCU_TIMER_CH2,
};
		
DevDacSound BoardDacSound={
		MCU_PORT_A,
		MCU_IO_5, 
		MCU_TIMER_3,
		NULL,
};

/* TEA5767 挂在vi2c1上*/
#define DEV_TEA5767_I2CBUS "VI2C1"



/*

	8266 底层驱动，负责跟串口等硬件通信

	模块使用安信可8266模块ESP-01/01S
	VCC/GND
	TXD/TXD
	EN  使能，高电平有效 -----------PF5
	RST 复位，低电平有效      ----------PF4
	IO0 启动时拉低，进入下载模式---------PA11
	IO2 请拉高				------------PA12
*/	
#define DEV_8266_UART MCU_UART_1

s32 bsp_exuart_wifi_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5; //GPIOG8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //速度 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure); //初始化 PG8	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12; //GPIOG8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //速度 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化 PG8
	
	GPIO_SetBits(GPIOA, GPIO_Pin_11|GPIO_Pin_12);
	GPIO_SetBits(GPIOF, GPIO_Pin_4);
	GPIO_ResetBits(GPIOF, GPIO_Pin_5);
	Delay(100);
	GPIO_ResetBits(GPIOF, GPIO_Pin_4);
	Delay(100);
	GPIO_SetBits(GPIOF, GPIO_Pin_4);
	GPIO_SetBits(GPIOF, GPIO_Pin_5);
	
	mcu_uart_open(DEV_8266_UART);
	mcu_uart_set_baud(DEV_8266_UART, 115200);

	
	return 0;
}

s32 bsp_exuart_wifi_read(u8 *buf, s32 len)
{
	s32 res;
	
	res = mcu_uart_read(DEV_8266_UART, buf, len);	
	
	return res;
}

s32 bsp_exuart_wifi_write(u8 *buf, s32 len)
{
	s32 res;
	
	res = mcu_uart_write(DEV_8266_UART, buf, len);

	return res;
}
/**/
#include "FreeRtos.h"
#include "frtos_task.h"

TaskHandle_t  Wujique407TaskHandle;

extern void wujique_stm407_test(void);

/**/
void board_app_task(void)
{
	wjq_log(LOG_DEBUG, "[   board] run app task! 2020.10.15 \r\n");

	wujique_stm407_test();
	
	while(1){}
}
/**/
s32 board_app_init(void)
{
	BaseType_t xReturn = pdPASS;
	
	xReturn = xTaskCreate(	(TaskFunction_t) board_app_task,
					(const char *)"wujique 407 test task",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
					(const configSTACK_DEPTH_TYPE) Wujique407_TASK_STK_SIZE,
					(void *) NULL,
					(UBaseType_t) Wujique407_TASK_PRIO,
					(TaskHandle_t *) &Wujique407TaskHandle );	
	
	if(xReturn != pdPASS)	
		wjq_log(LOG_DEBUG, "xTaskCreate wujique_407test err!\r\n");	
	
	return 0;
}

/*
	板级初始化
*/
s32 board_init(void)
{
	wjq_log(LOG_DEBUG, "[   board] wujique other dev init!***\r\n");

	/* 通过注册总线的驱动 */
	petite_dev_register();

	/* 以下是为归纳到总线系统的去驱动 */
	#if (SYS_USE_KEYPAD == 1)
	dev_keypad_init();
	#endif
	
	#if (SYS_USE_EXUART == 1)
	bsp_exuart_wifi_init();

	#endif
	
	dev_buzzer_init(&BoardBuzzer);
	dev_tea5767_init(DEV_TEA5767_I2CBUS);
	dev_dacsound_init(&BoardDacSound);

	/* need fix*/
	dev_key_init();
	dev_wm8978_init();/* 要实现一个声卡框架 */
	dev_rs485_init();
	dev_touchscreen_init();
	dev_touchkey_init();
	dev_camera_init();

	mcu_adc_temprate_init();
	//dev_htu21d_init();
	//dev_ptHCHO_init();
	//dev_srf05_test();

	vfs_init();
	vfs_mount(&SdFatFs);

	//sys_spiffs_mount_coreflash();
	//sys_lfs_mount();
	//lfs_test();
	
	/* USB 任务有长延时，后续要处理 need fix*/
	//usb_task_create();
	//vfs_mount(&USBFatFs);

	/* 创建目标板应用程序 */
	board_app_init();
	//eth_app_init();
	//fun_cmd_init();
	//mcu_timer_test();	
	
	return 0;
}

/*
	低优先级轮训任务
*/
void board_low_task(void)
{
	//wjq_log(LOG_DEBUG, "low TASK ");
	
	dev_key_scan();
		
	#if (SYS_USE_KEYPAD == 1)
	dev_keypad_scan();
	#endif
	
	eth_loop_task();
	fun_sound_task();
	fun_rec_task();
	dev_touchkey_task();

}

