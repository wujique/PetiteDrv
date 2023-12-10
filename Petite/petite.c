

/* Includes ------------------------------------------------------------------*/
#include "mcu.h"
#include "petite_config.h"
#include "petite.h"
#include "petite_def.h"
#include "cmsis_os.h"
#include "board_sysconf.h"
#include "drv_lcd.h"

/** @addtogroup Template_Project
  * @{
  */ 
/*
	创建petite线程，然后调用board init初始化板级设备
	*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

extern s32 board_init(void);
/**
 *@brief:      start_task
 *@details:    
 			   
 *@param[in]   void *pvParameters  
 *@param[out]  
 *@retval:     
 */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "Pet",
  .stack_size = START_TASK_STK_SIZE,
  .priority = (osPriority_t) START_TASK_PRIO,//osPriorityNormal,
};

extern osThreadId_t TestTaskHandle;


void petite_task(void *pvParameters)
{
	//char TaskListBuf[256];
	uint16_t tcnt=0;
	UBaseType_t stackHWM;
	
	LogPetite(LOG_INFO,"petite_task\r\n");

	/* 初始化petite模块状态*/
	//vfs_init();
	
	/* 初始化命令行 */
	fun_cmd_init();
	
	board_init();

	/* 测试内存溢出 rtos检测 ，溢出后HOOK函数会被调用
		vApplicationStackOverflowHook */
	//memset(TaskListBuf, 0, sizeof(TaskListBuf));
	
	for(;;) {
		
		osDelay(5);
		board_low_task(5);
		tcnt++;
		#if 1
		if (tcnt >= 1000) {
			tcnt = 0;
			
			//vTaskList(TaskListBuf);
			//uart_printf("  name         state priority   stack   NUM\r\n");
			//uart_printf("%s", TaskListBuf);
			
			//vTaskGetRunTimeStats(TaskListBuf);
			//uart_printf("  name count per\r\n");
			//uart_printf("%s", TaskListBuf);

			/* 栈的水位线，指栈剩余多少个 U32 */
			//stackHWM = uxTaskGetStackHighWaterMark(defaultTaskHandle);
			//uart_printf("PetiteTask hwm:%d\r\n", stackHWM);
			//stackHWM = uxTaskGetStackHighWaterMark(TestTaskHandle);
			//uart_printf("TestTask hwm:%d\r\n", stackHWM);
		}
		#endif
	}
}

#include <cm_backtrace.h>


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int petite_app(void)
{	
	/*           http://www.network-science.de/ascii/   */
	printf("\r\n*********************************************************\r\n");
	printf("*   ___     _   _ _          ___                        \r\n");       
	printf("*  / _ \\___| |_(_) |_ ___   /   \\_ ____   __             \r\n");
	printf("* / /_)/ _ \\ __| | __/ _ \\ / /\\ / '__\\ \\ / /             \r\n");
	printf("*/ ___/  __/ |_| | ||  __// /_//| |   \\ V /              \r\n");
	printf("*\\/    \\___|\\__|_|\\__\\___/___,' |_|    \\_/www.wujique.com\r\n");
	printf("*********************************************************\r\n\r\n");

	cm_backtrace_init(CM_BACKTRACE_AXF_NAME, HARDWARE_VERSION, SOFTWARE_VERSION);

  /* Infinite loop 
	尽快启动RTOS， 进入start_task再做其他设备初始化 */
	LogPetite(LOG_INFO,"ccreate start task!\r\n");

  	/* Init scheduler */
  	osKernelInitialize();
	defaultTaskHandle = osThreadNew(petite_task, NULL, &defaultTask_attributes);
					
	if(NULL != defaultTaskHandle){
		LogPetite(LOG_INFO,"p] freertos Scheduler\r\n");
		/* Start scheduler */
  		osKernelStart();
	}else{
		LogPetite(LOG_WAR,"xTaskCreate fail\r\n");
	}
	
	LogPetite(LOG_ERR,"while(1) err!\r\n");
	while(1);
  
}



/**
  * @}
  */
/*-------------------------------------
	一些OS HOOK函数定义，暂时放这里
-----------------------------------*/
void vApplicationStackOverflowHook (TaskHandle_t xTask, signed char *pcTaskName)
{
	LogPetite(LOG_ERR, "stack overflow %s\r\n", pcTaskName);
	
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
