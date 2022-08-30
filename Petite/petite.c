

/* Includes ------------------------------------------------------------------*/
#include "mcu.h"
#include "petite_config.h"

#include "petite.h"
#include "cmsis_os.h"

#include "board_sysconf.h"


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
  .name = "StartTask",
  .stack_size = START_TASK_STK_SIZE,
  .priority = (osPriority_t) START_TASK_PRIO,//osPriorityNormal,
};

void start_task(void *pvParameters)
{
	char TaskListBuf[256];
	char tcnt=0;
	
	wjq_log(LOG_INFO,"[    _app] start task\r\n");

	/* 初始化petite模块状态*/
	vfs_init();
	font_init();
	
	board_init();
	
	for(;;) {
		
		osDelay(5);
		board_low_task(5);
		tcnt++;
		if (tcnt >= 200) {
			tcnt = 0;
			vTaskList(TaskListBuf);
			uart_printf("  name         state priority   stack   NUM\r\n");
			uart_printf("%s", TaskListBuf);
			vTaskGetRunTimeStats(TaskListBuf);
			uart_printf("  name count per\r\n");
			uart_printf("%s", TaskListBuf);
		}
	}
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int petite_app(void)
{	
	/*           http://www.network-science.de/ascii/   */
	wjq_log(LOG_INFO,"\r\n*********************************************************\r\n");
	wjq_log(LOG_INFO,"*   ___     _   _ _          ___                        \r\n");       
	wjq_log(LOG_INFO,"*  / _ \\___| |_(_) |_ ___   /   \\_ ____   __             \r\n");
	wjq_log(LOG_INFO,"* / /_)/ _ \\ __| | __/ _ \\ / /\\ / '__\\ \\ / /             \r\n");
	wjq_log(LOG_INFO,"*/ ___/  __/ |_| | ||  __// /_//| |   \\ V /              \r\n");
	wjq_log(LOG_INFO,"*\\/    \\___|\\__|_|\\__\\___/___,' |_|    \\_/www.wujique.com\r\n");
	wjq_log(LOG_INFO,"*********************************************************\r\n\r\n");

  /* Infinite loop 
	尽快启动RTOS， 进入start_task再做其他设备初始化 */
	wjq_log(LOG_INFO,"ccreate start task!\r\n");
  	/* Init scheduler */
  	osKernelInitialize();
	defaultTaskHandle = osThreadNew(start_task, NULL, &defaultTask_attributes);
					
	if(NULL != defaultTaskHandle){
		wjq_log(LOG_INFO,"[    _app] freertos Scheduler\r\n");
		/* Start scheduler */
  		osKernelStart();
	}else{
		wjq_log(LOG_INFO,"[    _app] xTaskCreate fail\r\n");
	}
	
	wjq_log(LOG_INFO,"while(1) err!\r\n");
	while(1);
  
}



/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
