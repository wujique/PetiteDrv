#include "mcu.h"

#include "board_sysconf.h"
#include "log.h"
#include "bus/bus_uart.h"
#include "vfs.h"
#include "drv_keypad.h"
#include "drv_buzzer.h"
#include "dacsound.h"
#include "touch.h"

#include "mcu_dcmi.h"
#include "drv_lcd.h"
#include "camera_api.h"

#include "gb18030.h"
#include "gb2312.h"


/* 硬件相关的都在board.c定义
	调用对应的驱动进行初始化。
	不能在驱动中包含硬件相关信息。
*/
extern const TouchDev RtpXpt2046;
extern const TouchDev RtpAdc;
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

/*	8266 底层驱动，负责跟串口等硬件通信

	模块使用安信可8266模块ESP-01/01S
	VCC/GND
	TXD/TXD
	EN  使能，高电平有效 -----------PF5
	RST 复位，低电平有效      ----------PF4
	IO0 启动时拉低，进入下载模式---------PA11
	IO2 请拉高				------------PA12 */	
#define DEV_8266_UART "uart1"

const static BusUartPra Esp8266PortPra={
	.BaudRate = 115200,
	.bufsize = 512,
	};
	
BusUartNode *Esp8266UartNode;

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
	
	Esp8266UartNode = bus_uart_open(DEV_8266_UART, &Esp8266PortPra);

	return 0;
}

s32 bsp_exuart_wifi_read(u8 *buf, s32 len)
{
	s32 res;
	
	res = bus_uart_read(Esp8266UartNode, buf, len);	
	
	return res;
}

s32 bsp_exuart_wifi_write(u8 *buf, s32 len)
{
	s32 res;
	
	res = bus_uart_write(Esp8266UartNode, buf, len);

	return res;
}

/**/
#include "cmsis_os.h"

osThreadId_t TestTaskHandle;
const osThreadAttr_t TestTask_attributes = {
  .name = "TestTask",
  .stack_size = Wujique407_TASK_STK_SIZE,
  .priority = (osPriority_t) Wujique407_TASK_PRIO,//osPriorityNormal,
};

extern void wujique_stm407_test(void);

/**/
void board_app_task(void)
{
	wjq_log(LOG_DEBUG, "[   board] run app task! 2020.10.15 \r\n");

	//spi_example();
	//bus_uart_test();

	/* 初始化文件系统 */
	sd_fatfs_init();
	
	flashdb_demo();

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
	
	wujique_stm407_test();
	while(1){}
}
/**/
s32 board_app_init(void)
{
	TestTaskHandle = osThreadNew(board_app_task, NULL, &TestTask_attributes);
					
	if(NULL != TestTaskHandle){
		wjq_log(LOG_INFO,"[    _app] freertos Scheduler\r\n");
	}else{
		wjq_log(LOG_INFO,"[    _app] xTaskCreate fail\r\n");
	}
	
	return 0;
}

extern void *PetiteDevTable[];
extern PartitionDef PetitePartitonTable[];

/*
	板级初始化
*/
s32 board_init(void)
{
	wjq_log(LOG_DEBUG, "[   board] wujique other dev init!***\r\n");

	/* 注册总线的驱动 ：I2C/SPI/BUS LCD...*/
	mcu_fsmc_lcd_Init();
	petite_dev_register(PetiteDevTable);

	/* 初始化存储空间与分区*/
	petite_partition_init(PetitePartitonTable);

	/* 以下是未归纳到总线系统的去驱动 
		要将这些驱动改为字符驱动架构 
		*/
	#if (SYS_USE_KEYPAD == 1)
	dev_keypad_init();
	#endif
	#if (SYS_USE_EXUART == 1)
	bsp_exuart_wifi_init();
	#endif
	dev_buzzer_init(&BoardBuzzer);
	dev_key_init();
	//dev_rs485_init();
	//mcu_adc_test();
	#if (SYS_USE_TS_IC_CASE == 1)
	tp_init(&RtpXpt2046);
	#endif
	#if (SYS_USE_TS_ADC_CASE == 1)
	tp_init(&RtpAdc);
	#endif
	dev_touchkey_init();
	#if (SYS_USE_TS_ADC_CASE != 1)
	mcu_adc_temprate_init();
	#endif
	//dev_htu21d_init();
	//dev_ptHCHO_init();
	//dev_srf05_test();

	/* 摄像头不归入字符设备 */
	/* camera xclk use the MCO1 */
	
	//SCCB_GPIO_Config();
	BUS_DCMI_HW_Init();
	bus_dcmi_init();
	DCMI_PWDN_RESET_Init();
	
	camera_init();
	//test_camera();
	/* 要实现一个声卡框架 
		兼容多种声音播放方式 */
	dev_tea5767_init(DEV_TEA5767_I2CBUS);
	dev_dacsound_init(&BoardDacSound);

	mcu_i2s_init();//初始化I2S接口
	dev_wm8978_init();

	//sys_spiffs_mount_coreflash();
	//sys_lfs_mount();
	//lfs_test();
	/* USB 任务有长延时，后续要处理 need fix*/
	//usb_task_create();
	//vfs_add_node(&USBFatFs);
	/*---------------------------------------*/
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
extern void (*tp_task)(void);

void board_low_task(int tick)
{
	//wjq_log(LOG_DEBUG, "low TASK ");
	
	dev_key_scan();
	
	if (tp_task != NULL) {
			//wjq_log(LOG_DEBUG, "1");
			tp_task();
		}
	
	#if (SYS_USE_KEYPAD == 1)
	dev_keypad_scan();
	#endif
	
	//eth_loop_task();
	fun_sound_task();
	//fun_rec_task();
	
	dev_touchkey_task();

	
}



/*----------------------这是个垃圾桶 没有归属的，板相关的都先放这里 ------------------------------------*/
#if 1

ImageFormat_TypeDef ImageFormat;


u16 *camera_buf0;
u16 *camera_buf1;

#define LCD_BUF_SIZE 320*2*2//一次传输2行数据，一个像素2个字节
u16 DmaCnt;

#define CAMERA_USE_RAM2LCD	0
/*
	启动摄像头DMA传输，图像显示到制定的LCD上
*/
#if 1
s32 board_camera_show(DevLcdNode *lcd)
{

	volatile u8 sta;
	
	uart_printf("board_camera_show\r\n");

	DmaCnt = 0;
	/* 选择图片格式 */
	ImageFormat = (ImageFormat_TypeDef)BMP_QVGA;

	/* LCD Display window */
	lcd_setdir(lcd, W_LCD, L2R_U2D);
	lcd_prepare_display(lcd, 1, 320, 1, 240);

	BUS_DCMI_Config(DCMI_PCKPolarity_Rising, DCMI_VSPolarity_Low, DCMI_HSPolarity_Low);

	#if 0
	/* 没有调试通 */
	//OV5640_RGB565_Mode();	
	//OV5640_OutSize_Set(4, 0, 320 , 240); 
	//OV5640_SET_SCPLL(3);
	#else
	OV2640_Config(ImageFormat);
	#endif
	/* 4个byte  也就是一个word , DMA会除4，也就是进行一次传输 */
	mcu_dcmi_captruce(0, FSMC_LCD_ADDRESS, 4);

	while(1) {
		mcu_dcmi_get_sta(&sta);

		/*一定要先检测数据再检测帧完成，最后一次两个中断差不多同时来*/
		if(DCMI_FLAG_FRAME == (sta&DCMI_FLAG_FRAME)) {
			wjq_log(LOG_DEBUG, "-f-%d- ", DmaCnt);
			DmaCnt = 0;
			mcu_dcmi_start();
		}
	}
	return 0;
}
#else
s32 board_camera_show(DevLcdNode *lcd)
{
	uint8_t abuffer[40];
  	s32 ret;
	volatile u8 sta;

	uart_printf("board_camera_show\r\n");

	DmaCnt = 0;
	uint8_t bufindex = 0;
	
	/* 选择图片格式 */
	ImageFormat = (ImageFormat_TypeDef)BMP_QVGA;

	/* LCD Display window */
	dev_lcd_setdir(lcd, W_LCD, L2R_U2D);
	dev_lcd_prepare_display(lcd, 1, 320, 1, 240);

	BUS_DCMI_Config(DCMI_PCKPolarity_Rising, DCMI_VSPolarity_Low, DCMI_HSPolarity_Low);

	OV2640_Config(ImageFormat);
	
	camera_buf0 = (u16 *)wjq_malloc(LCD_BUF_SIZE);
	camera_buf1 = (u16 *)wjq_malloc(LCD_BUF_SIZE);
	mcu_dcmi_captruce(1, (uint32_t)camera_buf0, LCD_BUF_SIZE);
	
	while(1) {
		
		mcu_dcmi_get_sta(&sta);

		if (DCMI_FLAG_BUF0 == (sta&DCMI_FLAG_BUF0)) {
			if (bufindex == 0) {
				mcu_dcmi_captruce(1, (uint32_t)camera_buf1, LCD_BUF_SIZE);
				bufindex = 1;
				dev_lcd_flush(lcd, camera_buf0, LCD_BUF_SIZE/2);
				
			} else {
				mcu_dcmi_captruce(1, (uint32_t)camera_buf0, LCD_BUF_SIZE);
				bufindex = 0;
				dev_lcd_flush(lcd, camera_buf1, LCD_BUF_SIZE/2);
			}
			DmaCnt++;
		}

		/*一定要先检测数据再检测帧完成，最后一次两个中断差不多同时来*/
		if(DCMI_FLAG_FRAME == (sta&DCMI_FLAG_FRAME)) {
			//wjq_log(LOG_DEBUG, "-f-%d- ", DmaCnt);
			DmaCnt = 0;
			//camera_capture();
		}
	}
	return 0;
}
#endif
#endif

void board_pre_sleep(uint32_t xModifiableIdleTime)
{
	//uart_printf("1");
}

void board_post_sleep(uint32_t xExpectedIdleTime)
{
	//uart_printf("2");
}



