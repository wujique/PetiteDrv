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

#include "cmsis_os.h"

#include "lvgl.h"
#include "lvgl_porting/lv_port_disp.h"
#include "demos/benchmark/lv_demo_benchmark.h"

DevLcdNode *lvgllcd;

osThreadId_t TestTaskHandle;
const osThreadAttr_t TestTask_attributes = {
  .name = "TestTask",
  .stack_size = Wujique407_TASK_STK_SIZE,
  .priority = (osPriority_t) Wujique407_TASK_PRIO,//osPriorityNormal,
};


/**/
void board_app_task(void)
{
	LogBoard(LOG_DEBUG, "run app task! 2023.12.10\r\n");

	/* 初始化文件系统 */
	//sd_fatfs_init();
	
	flashdb_demo();

	/* 测试littlefs */
	#if 1
	int filefd;
	filefd = vfs_open("/mtd0/bootcnt", O_CREAT);
	if(filefd > 0) {
		uint32_t bootcnt = 0;
		
		vfs_read(filefd, &bootcnt, sizeof(bootcnt));
		LogBoard(LOG_INFO, "boot cnt:%d\r\n", bootcnt);
		
		bootcnt++;
		
		vfs_lseek(filefd, 0, SEEK_SET);
		vfs_write(filefd, &bootcnt, sizeof(bootcnt));
		vfs_close(filefd);

	}
	#endif
	
		/* 初始化lvgl 
	注意，初始化LVGL的过程，会用到不少栈，
	如果在rtos的任务中进行初始化，注意任务栈的大小，
	防止溢出造成hardfault */
	#if 1
	
    lvgllcd = lcd_open("tftlcd");
	lcd_setdir(lvgllcd, H_LCD, R2L_U2D);
	tp_open("board_ctp_g1158");

	#if 0
	lcd_setdir(lvgllcd, W_LCD, R2L_U2D);
	tp_rotate(270);
	#endif

	lv_init();
	lv_port_disp_init();
	lv_port_indev_init();
	//lv_demo_benchmark();
	lv_demo_widgets();
	#endif
	
	while(1){
		osDelay(2);
		
		/* lvgl */
		lv_task_handler();

	}
}
/**/
s32 board_app_init(void)
{
	TestTaskHandle = osThreadNew(board_app_task, NULL, &TestTask_attributes);
					
	if(NULL != TestTaskHandle){
		LogBoard(LOG_INFO, "freertos Scheduler\r\n");
	}else{
		LogBoard(LOG_INFO, "xTaskCreate fail\r\n");
	}
	
	return 0;
}

extern void *PetiteDevTable[];
extern PartitionDef PetitePartitonTable[];
extern const DevTouch BoardDevTp;

/*
	板级初始化
*/
s32 board_init(void)
{
	LogBoard(LOG_DEBUG, "pochar other dev init!***\r\n");

	/* 注册总线的驱动 ：I2C/SPI/BUS LCD...*/
	mcu_fsmc_lcd_Init(2);
	petite_dev_register(PetiteDevTable);

	/* 初始化存储空间与分区*/
	petite_partition_init(PetitePartitonTable);


	tp_init(&BoardDevTp);

	/*---------------------------------------*/
	/* 创建目标板应用程序 */
	board_app_init();

	return 0;
}

/*
	低优先级轮训任务
*/


void board_low_task(uint8_t tick)
{
	
	//wjq_log(LOG_DEBUG, " low task ");
	tp_task_loop(tick);

	//fun_sound_task();
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
			LogBoard(LOG_DEBUG, "-f-%d- ", DmaCnt);
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
	//LogBoard("1");
}

void board_post_sleep(uint32_t xExpectedIdleTime)
{
	//LogBoard("2");
}



