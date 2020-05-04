

/* Includes ------------------------------------------------------------------*/
#include "mcu.h"
#include "petite_config.h"

#include "log.h"
#include "FreeRTos.h"
#include "board_sysconf.h"

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

TaskHandle_t  StartTaskHandle;
void start_task(void *pvParameters);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int petite_app(void)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
	/*           http://www.network-science.de/ascii/   */
	wjq_log(LOG_INFO,"\r\n*********************************************************\r\n");
	wjq_log(LOG_INFO,"*   ___     _   _ _          ___                        \r\n");       
	wjq_log(LOG_INFO,"*  / _ \\___| |_(_) |_ ___   /   \\_ ____   __             \r\n");
	wjq_log(LOG_INFO,"* / /_)/ _ \\ __| | __/ _ \\ / /\\ / '__\\ \\ / /             \r\n");
	wjq_log(LOG_INFO,"*/ ___/  __/ |_| | ||  __// /_//| |   \\ V /              \r\n");
	wjq_log(LOG_INFO,"*\\/    \\___|\\__|_|\\__\\___/___,' |_|    \\_/www.wujique.com\r\n");
	wjq_log(LOG_INFO,"*********************************************************\r\n\r\n");

  /* Infinite loop */
	#ifdef SYS_USE_RTOS
	wjq_log(LOG_INFO,"ccreate start task!\r\n");
	xReturn = xTaskCreate(	(TaskFunction_t) start_task,
					(const char *)"StartTask",
					(const configSTACK_DEPTH_TYPE)START_TASK_STK_SIZE,
					(void *) NULL,
					(UBaseType_t) START_TASK_PRIO,
					(TaskHandle_t *) &StartTaskHandle );
	
	if(pdPASS == xReturn){
		wjq_log(LOG_INFO,"[    _app] freertos Scheduler\r\n");
		vTaskStartScheduler();
	}else{
		wjq_log(LOG_INFO,"[    _app] xTaskCreate fail\r\n");
	}
	
	#else
	void *p;
	wjq_log(LOG_INFO,"run start task(no rtos)\r\n");
	start_task(p);
	#endif

	wjq_log(LOG_INFO,"while(1) err!\r\n");
	while(1);
  
}

extern s32 board_init(void);
/**
 *@brief:      start_task
 *@details:    
 			   
 *@param[in]   void *pvParameters  
 *@param[out]  
 *@retval:     
 */
void start_task(void *pvParameters)
{
	wjq_log(LOG_INFO,"[    _app] start task\r\n");

	board_init();
	
	while (1) {
		vTaskDelay(2);
		board_low_task();
	}
}

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
