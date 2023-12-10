/*

	board.c定义有什么
	cuckoo.c就是怎么用这些东西。

	*/
#include "mcu.h"
#include "bus_uart.h"
#include "log.h"
#include "board_sysconf.h"

#include "touch.h"
#include "partition.h"

extern const TouchDev CtpGt9147;
extern const TouchDev CtpGt1151;
extern const TouchDev RtpXpt2046;

extern void DCMI_PWDN_RESET_Init(void);

extern s32 petite_dev_register(void *DevTable[]);

extern void *PetiteDevTable[];
extern PartitionDef PetitePartitonTable[];

/*
	板级初始化
*/
s32 board_init(void)
{
	int res= -1;
	
	wjq_log(LOG_DEBUG, "[   board] cuckoo other dev init!\r\n");

	/* 初始化设备*/
	petite_dev_register(PetiteDevTable);

	/* 初始化存储空间与分区*/
	petite_partition_init(PetitePartitonTable);
	
    lcd_reset();
    LCD_Config();

	//dev_wm8978_init();
	//dev_wm8960_init();
	/*  触摸屏初始化 */
	res= -1;
	#if 1
	res = tp_init(&CtpGt9147);
	/**/
	if (res == -1) {
		res = tp_init(&CtpGt1151);	
	}
	
	/**/
	if (res == -1) {
		/*  初始化电阻屏 */
		//tp_init(&RtpXpt2046);
	}
	#endif
	/* 摄像头初始化 */
	DCMI_PWDN_RESET_Init();
	camera_init();

	cuckoo_7b0_test_init();
	
	return 0;
}


const BusUartPra Uart4Pra = {
	.BaudRate = 115200, //波特率			 

	.bufsize = 128,//接收缓冲长度, 如不使用交互，可设置为8
};

BusUartNode *LogUartNode;
#if 0
extern const unsigned char elfsimage[];
extern const unsigned long elfsimage_length;
#else
const unsigned char *elfsimage = (unsigned char *)QSPI_FLASH_LITTLEFS_ADDR;
const unsigned long elfsimage_length = QSPI_FLASH_LITTLEFS_SIZE;
#endif
void board_main(void)
{
	bus_uart_init();
	LogUartNode = bus_uart_open("uart4", &Uart4Pra);
	wjq_log(LOG_INFO, "------Cuckoo (stm32h7b0vb) run! 20220809------\r\n");

	/*  尽早初始化 Flash，并使其进入map模式，因为后续有代码是保存在 QSPI Flash上 
		在执行到这里之前已经开了systick中断，中断中调用的函数都需要放到内部Flash        		
		@note 如果使用不同的flash，如何兼容呢？*/
   	W25Qxx_init();
	w25qxx_test_wr();
  	w25qxx_map();

	#if 1
	u8 cnt;
	cnt = test_qpi_run_fun(2);
	uart_printf("run qpi:%d\r\n", cnt);
	if (cnt != 3) {
		uart_printf("run qpi err!\r\n");
		while(1);
	}

	#endif
	#if 1
	/* 测试 将一个littlefs镜像链接到外部QSPI FLASH */
	uart_printf("elfs addr:0x%08x, len:0x%x", elfsimage, elfsimage_length);
	DUMP_HEX_16BYTE((u8 *)elfsimage, 128);
	#endif

	
  	petite_app();
}
/*
	低优先级轮训任务
	这是系统整个系统的底层轮询
	在这里的函数有以下要求：
	1 不能执行太长时间，更不要进行等待和delay
	2 临时变量不要使用太多，否则会任务栈溢出。
*/
extern void fun_sound_task(void);
extern void (*tp_task)(void);

uint16_t touch_task_cnt = 0;
void board_low_task(uint8_t tick)
{
	
	//wjq_log(LOG_DEBUG, " low task ");
	touch_task_cnt += tick;
	if (touch_task_cnt >= 30) {
		touch_task_cnt = 0;
		if (tp_task != NULL) {
			//wjq_log(LOG_DEBUG, "1");
			tp_task();
		}
	}

	//fun_sound_task();
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
	GPIO_InitStructure.Pull = GPIO_NOPULL;
   	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
   	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
   	HAL_GPIO_Init(DCMI_RESET_PORT, &GPIO_InitStructure);

	/* PWDN hight ov5640 power off */
	GPIO_InitStructure.Pin = DCMI_PWDN_PIN;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
   	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
   	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
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
	//OV5640_RGB565_Mode();	
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


void board_pre_sleep(uint32_t xModifiableIdleTime)
{
	//uart_printf("1");
}

void board_post_sleep(uint32_t xExpectedIdleTime)
{
	//uart_printf("2");
}


s32 dev_keypad_read(u8 *key, u8 len)
{
	return 0;
}


