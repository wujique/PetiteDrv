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
	//sys_timer_test();
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
void esp32_c3_spi_at_test(void);
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

	//esp8266_uart_test();
	//esp32_c3_spi_at_test();

	#if 0
	mcu_i2s_init(1);//初始化I2S接口
	dev_wm8978_init();
	petite_add_loop("sound", fun_sound_task, 30);
	#endif
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

/*--------------------------



----------------------------*/

/*
	8266 串口WIFI模块简单测试

 */
/*
	pochard stm32f407vgt 单板
	PA1 IO2
	PA0 IO0
	PC10 EN
	PA15 RST

	外扩串口接ESP8266串口模块，IO与I2S复用，需要屏蔽调I2S功能
	*/

void esp8266_io_init(void)
{
	/* 全部配置为输入，就不会影像模块工作了 */
	mcu_io_config_in(MCU_PORT_A, MCU_IO_0);	
	mcu_io_config_in(MCU_PORT_A, MCU_IO_1);	
	mcu_io_config_in(MCU_PORT_A, MCU_IO_15);	
	mcu_io_config_in(MCU_PORT_C, MCU_IO_10);	
}

char ESP8266_buf[10];

const static BusUartPra Uart7Pra = {
	.BaudRate = 115200, //波特率			 

	.bufsize = 512,//接收缓冲长度
};

void esp8266_uart_test(void)
{
	int rlen;
	void *uartnode;
	uint8_t buf[16];

	esp8266_io_init();

	uartnode = bus_uart_open("uart2", &Uart7Pra);

	while(1) {
		//uart_printf("s ");
		bus_uart_write(uartnode, "AT\r\n", 4);
		HAL_Delay(1000);
		memset(buf, 0, sizeof(buf));
		rlen = bus_uart_read(uartnode, buf, 10);
		if (rlen != 0) {
			uart_printf("%s", buf);
		}
	}
}

/**
 * 贴在板子上的esp32-c3模块，SPI接口
 * 
 * https://github.com/espressif/esp-at/tree/9bd111dd3233caead0fdf40dc99092cc7e9b0930/examples/at_spi_master/spi/esp32_c_series
 * 
 * https://docs.espressif.com/projects/esp-at/zh_CN/latest/esp32c3/Compile_and_Develop/How_to_implement_SPI_AT.html
 * 
 * 
 * esp32 c3固件默认管脚
 * SCLK 		GPIO6
 * MISO 		GPIO2
 * MOSI 		GPIO7
 * CS   		GPIO10
 * HANDSHAKE 	GIIO3
 * 
 * 
 * 
 * stm32对应管脚如下
#define ESP32_C3_SPI2_MISO  PB14
#define ESP32_C3_SPI2_CLK	PB13
#define ESP32_C3_SPI2_MOSI  PB15
#define ESP32_C3_SPI2_NSS	PB12

#define ESP32_C3_HANDSHAKE	PD13
#define ESP32_C3_DATA_READY	PD12  DATA_READY脚是 esp host方式使用
#define ESP32_C3_RESET		PA9

使用握手线的具体方法为：
Master 向 slave 发送 AT 数据时，使用握手线的方法为：
master 向 slave 发送请求传输数据的请求，然后等待 slave 向握手线发出的允许发送数据的信号。
master 检测到握手线上的 slave 发出的允许发送的信号后，开始发送数据。
master 发送数据后，通知 slave 数据发送结束。
Master 接收 slave 发送的 AT 数据时，使用握手线的方法为：
slave 通过握手线通知 master 开始接收数据。
master 接收数据，并在接收所有数据后，通知 slave 数据已经全部接收。

 */

DevSpiChNode *EspAtSpiChNode;
uint8_t EspAtSpiSN = 0;


struct _sSlaveSta {
	uint8_t NA[3];
	uint8_t dre;//0x01, 可读， 0x02, 可写
	uint8_t sn; // slave sn
	uint8_t len_l;
	uint8_t len_h;

	/* len */
	uint16_t len;
};

struct _sSlaveSta SlaveSta;

#define ESP_SPI_DMA_MAX_LEN   4092

#define CMD_HD_WRBUF_REG      0x01
#define CMD_HD_RDBUF_REG      0x02
#define CMD_HD_WRDMA_REG      0x03
#define CMD_HD_RDDMA_REG      0x04
#define CMD_HD_WR_END_REG     0x07
#define CMD_HD_INT0_REG       0x08

#define WRBUF_START_ADDR      0x0
#define RDBUF_START_ADDR      0x4
#define STREAM_BUFFER_SIZE    1024 * 8

#define CMD_HD_DUMMY	0x00

#define CMD_DEFAULT_SN		0x00
#define CMD_WRITE_DATALEN	0x02
#define CMD_MAGIC			0xfe
/*-------------------------------------------------------------------*/
// 主机向从机发送数据
int at_spi_master_send_data(uint8_t *data, uint16_t len)
{
	char tmp[3] = {CMD_HD_WRDMA_REG, WRBUF_START_ADDR, CMD_HD_DUMMY};
	bus_spich_cs(EspAtSpiChNode, 0);
	bus_spich_transfer(EspAtSpiChNode, tmp, NULL, 3);
	bus_spich_transfer(EspAtSpiChNode, data, NULL, len);
	bus_spich_cs(EspAtSpiChNode, 1);

	return 0;
}

// 主机读取从机发送的数据
void at_spi_master_recv_data(uint8_t* data, uint32_t len)
{
	
	char tmp[3] = {CMD_HD_RDDMA_REG, RDBUF_START_ADDR, CMD_HD_DUMMY};
	bus_spich_cs(EspAtSpiChNode, 0);
	bus_spich_transfer(EspAtSpiChNode, tmp, NULL, 3);
	bus_spich_transfer(EspAtSpiChNode, NULL, data, len);
	bus_spich_cs(EspAtSpiChNode, 1);
}
// send a single to slave to tell slave that master has read DMA done
// 主机通知从机，主机读取数据已经结束
static void at_spi_rddma_done(void)
{
	char tmp[3] = {CMD_HD_INT0_REG, 0, 0};
	bus_spich_cs(EspAtSpiChNode, 0);
	bus_spich_transfer(EspAtSpiChNode, tmp, NULL, 3);
	bus_spich_cs(EspAtSpiChNode, 1);
}

// send a single to slave to tell slave that master has write DMA done
static void at_spi_wrdma_done(void)
{
	char tmp[3] = {CMD_HD_WR_END_REG, 0, 0};
	bus_spich_cs(EspAtSpiChNode, 0);
	bus_spich_transfer(EspAtSpiChNode, tmp, NULL, 3);
	bus_spich_cs(EspAtSpiChNode, 1);
}

// when spi slave ready to send/recv data from the spi master, the spi slave will a trigger GPIO interrupt,
// then spi master should query whether the slave will perform read or write operation.
// 读状态



static int query_slave_data_trans_info(void)
{

	char tmp[7] = {CMD_HD_RDBUF_REG, RDBUF_START_ADDR, 0, 0xff, 0xff, 0xff, 0xff};
	bus_spich_cs(EspAtSpiChNode, 0);
	bus_spich_transfer(EspAtSpiChNode, tmp, &SlaveSta, 7);
	//bus_spich_transfer(EspAtSpiChNode, NULL, sta, 4);
	bus_spich_cs(EspAtSpiChNode, 1);

	SlaveSta.len = SlaveSta.len_h*256 + SlaveSta.len_l;
	printf("slave sta(1=r, 2=w):%d, sn:%d, len:%d\r\n", SlaveSta.dre, SlaveSta.sn, SlaveSta.len);

    return 0;
}

char SpiAtReqWri[7]={CMD_HD_WRBUF_REG, WRBUF_START_ADDR, CMD_HD_DUMMY, 
					CMD_WRITE_DATALEN&0xff, (CMD_WRITE_DATALEN>>8)&0xff,
					CMD_DEFAULT_SN, CMD_MAGIC};

/* 请求写数据，len是希望写数据长度 */
int esp_spi_at_request_to_write(uint16_t len)
{
	SpiAtReqWri[3] = len & 0xff;
	SpiAtReqWri[4] = (len>>8) & 0xff;
	SpiAtReqWri[5] = EspAtSpiSN;

	/* 拉低 */
	bus_spich_cs(EspAtSpiChNode, 0);
	bus_spich_transfer(EspAtSpiChNode, SpiAtReqWri, NULL, 7);
	bus_spich_cs(EspAtSpiChNode, 1);

	return 0;
}
/*-------------------------------------------------------------------*/

// before spi master write to slave, the master should write WRBUF_REG register to notify slave,
// and then wait for handshark line trigger gpio interrupt to start the data transmission.
// 握手管脚状态
int esp_spi_at_handshake_sta(void)
{
	return mcu_io_input_readbit(MCU_PORT_D, MCU_IO_13);
}


void esp32_c3_io_init(void)
{
	/* spi 接口已经 初始化 */
	mcu_io_config_in(MCU_PORT_D, MCU_IO_12);//no use

	mcu_io_config_in(MCU_PORT_D, MCU_IO_13);//handshake

	mcu_io_config_out(MCU_PORT_A, MCU_IO_9);//reset
	mcu_io_output_resetbit(MCU_PORT_A, MCU_IO_9);
	osDelay(50);
	mcu_io_output_setbit(MCU_PORT_A, MCU_IO_9);
}

uint8_t databuf[256];

void esp32_c3_spi_at_test(void)
{
	int handshake_sta;

	EspAtSpiChNode = bus_spich_open("SPI2_CH1", SPI_MODE_0, 2000, 0xffffffff);

	if (EspAtSpiChNode == NULL) {
		printf("open spich err!\r\n");
	}

	esp32_c3_io_init();

	while(1) {
		osDelay(1000);
		handshake_sta = esp_spi_at_handshake_sta();
		printf("handshake sta:%d\r\n", handshake_sta);
		if (handshake_sta == 1) {
			query_slave_data_trans_info();

			if(SlaveSta.dre == 1 && SlaveSta.len != 0) {
				at_spi_master_recv_data(databuf, SlaveSta.len);
				dump_hex(databuf, SlaveSta.len, 16);
				at_spi_rddma_done();
			} else if (SlaveSta.dre == 2) {
				at_spi_master_send_data("AT\r\n", 4);
				at_spi_wrdma_done();
			}
		} else {
			esp_spi_at_request_to_write(4);
		}
	
	}

}

/*------------------------end----------------------*/

