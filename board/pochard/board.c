#include "mcu.h"

#include "board_sysconf.h"
#include "log.h"
#include "bus/bus_uart.h"
#include "vfs.h"
#include "drv_keypad.h"
#include "drv_buzzer.h"
#include "dacsound.h"
#include "touch.h"
#include "drv_lcd.h"
#include "camera_api.h"

#include "cmsis_os.h"

#include "lvgl.h"
#include "lvgl_porting/lv_port_disp.h"
#include "demos/benchmark/lv_demo_benchmark.h"

#include "panel/soundplay.h"

#include "components/softtimer/softtimer.h"

#include "gb2312.h"
#include "gb18030.h"

void *BoardSlstLoop = NULL;

int board_add_loop(char *name, void *cb, uint32_t periodic)
{
	void *looptimer = slst_create(name, cb);
	slst_start(BoardSlstLoop, looptimer, periodic, looptimer, SOFTTIMER_TYPE_PERIODIC);
}

DevLcdNode *lvgllcd;

osThreadId_t TestTaskHandle;
const osThreadAttr_t TestTask_attributes = {
  .name = "TestTask",
  .stack_size = Wujique407_TASK_STK_SIZE,
  .priority = (osPriority_t) Wujique407_TASK_PRIO,//osPriorityNormal,
};

extern void esp_host_task_Init(void);

void loop_lv_task_handler(void *userdata)
{
	lv_task_handler();
}
void board_lv_tick_inc(void *userdata)
{
	lv_tick_inc(5);
}
/**/
void board_app_task(void)
{
	LogBoard(LOG_DEBUG, "run app task! 2023.12.10\r\n");

	BoardSlstLoop = slst_create_loop();

	/* 初始化文件系统 */
	sd_fatfs_init();
	
	//flashdb_demo();

	/* 测试littlefs */
	#if 0
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
	
	//esp_host_task_Init();

	//fun_sound_play("/1:/sound/stereo_16bit_32k.wav", "wm8978");

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
	board_add_loop("lvgl", loop_lv_task_handler, 10);
	board_add_loop("lvgl tick", board_lv_tick_inc, 2);
	
	#endif
	
	int cnt = 0;

	while(1){
		slst_task(BoardSlstLoop);

		osDelay(5);//不会执行
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
void board_test_camera(void);
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
	petite_add_loop("tp loop", tp_task_loop, 30);
	
	//SCCB_GPIO_Config();
	BUS_DCMI_HW_Init();
	bus_dcmi_init();
	DCMI_PWDN_RESET_Init();
	
	camera_init();
	//board_test_camera();

	mcu_i2s_init(1);//初始化I2S接口
	dev_wm8978_init();
	petite_add_loop("sound", fun_sound_task, 30);

	/*---------------------------------------*/
	/* 创建目标板应用程序 */
	board_app_init();

	return 0;
}

/*----------------------这是个垃圾桶 没有归属的，板相关的都先放这里 ------------------------------------*/
#if 1

#include "ov5640.h"
#include "drv_ov2640.h"

ImageFormat_TypeDef ImageFormat;


u16 *camera_buf0;
u16 *camera_buf1;

#define LCD_BUF_SIZE 320*2*2//一次传输2行数据，一个像素2个字节


#define CAMERA_USE_RAM2LCD	0
/*
	启动摄像头DMA传输，图像显示到制定的LCD上
*/
#if 1
s32 board_camera_show(DevLcdNode *lcd)
{

	volatile u8 sta;
	
	uart_printf("board_camera_show\r\n");

	/* LCD Display window */
	lcd_setdir(lcd, W_LCD, L2R_U2D);
	/** 设置显示窗口 */
	lcd_prepare_display(lcd, 0, 480, 0, 320);

	u8 dcmimode = 0;
	/** 第一个参数是模式，连续 or 快照 */
	BUS_DCMI_Config(dcmimode, DCMI_PCKPolarity_Rising, DCMI_VSPolarity_Low, DCMI_HSPolarity_Low);

	Camera_TypeDef cameratype;
	cameratype = camera_get_type();
	if (cameratype == OV5640_CAMERA){
		//OV5640_RGB565_Mode();	
		OV5640_OutSize_Set(4, 0, 480 , 320); 
		OV5640_SET_SCPLL(2);
	} else if(cameratype == OV2640_CAMERA) {
		/* 选择图片格式 */
		OV2640_UXGAConfig();
		OV2640_RGB565_Mode();
		OV2640_OutSize_Set(480, 320);

	} else {
		LogBoard(LOG_INFO, "test camera type err!\r\n");
		return 0;
	}
	
	/*  配置DCMI传输参数，第三个参数是目标地址空间长度，
		8080 接口LCD，地址不变，因此设置为1，此时，传输模式必须是0
		如果是传输到buf，则是buf的长度*/
	mcu_dcmi_captruce(0, FSMC_LCD_ADDRESS, 1);
	mcu_dcmi_start();

	while(1) {
		if (dcmimode == 1) {

			mcu_dcmi_get_sta(&sta);

			/*一定要先检测数据再检测帧完成，最后一次两个中断差不多同时来*/
			if(DCMI_FLAG_FRAME == (sta&DCMI_FLAG_FRAME)) {
				osDelay(1000);
				mcu_dcmi_start();
			}
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

void board_test_camera(void)
{
	DevLcdNode *lcd;

	lcd = lcd_open("tftlcd");
	if (lcd == NULL) {
		wjq_log(LOG_DEBUG, "open lcd err!");

	} 

	if (-1 == camera_open()) {
			wjq_log(LOG_DEBUG, "open camera err\r\n");
			return;
	}

	board_camera_show(lcd);
}

#endif

void board_pre_sleep(uint32_t xModifiableIdleTime)
{
	//LogBoard("1");
}

void board_post_sleep(uint32_t xExpectedIdleTime)
{
	//LogBoard("2");
}



