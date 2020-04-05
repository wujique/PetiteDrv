

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "log.h"
#include "FreeRTos.h"

extern s32 board_main_init(void);

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
#define START_TASK_STK_SIZE 256
#define START_TASK_PRIO	3//
TaskHandle_t  StartTaskHandle;
void start_task(void *pvParameters);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int petite_app(void)
{

	/*           http://www.network-science.de/ascii/   */
wjq_log(LOG_INFO,"*********************************************************\r\n");
wjq_log(LOG_INFO,"*   ___     _   _ _          ___                        \r\n");       
wjq_log(LOG_INFO,"*  / _ \\___| |_(_) |_ ___   /   \\_ ____   __             \r\n");
wjq_log(LOG_INFO,"* / /_)/ _ \\ __| | __/ _ \\ / /\\ / '__\\ \\ / /             \r\n");
wjq_log(LOG_INFO,"*/ ___/  __/ |_| | ||  __// /_//| |   \\ V /              \r\n");
wjq_log(LOG_INFO,"*\\/    \\___|\\__|_|\\__\\___/___,' |_|    \\_/www.wujique.com\r\n");
wjq_log(LOG_INFO,"*********************************************************\r\n");

  /* Infinite loop */
	#ifdef SYS_USE_RTOS
	wjq_log(LOG_INFO,"create start task!\r\n");
	xTaskCreate(	(TaskFunction_t) start_task,
					(const char *)"StartTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
					(const configSTACK_DEPTH_TYPE)START_TASK_STK_SIZE,
					(void *) NULL,
					(UBaseType_t) START_TASK_PRIO,
					(TaskHandle_t *) &StartTaskHandle );
	vTaskStartScheduler();
	#else
	void *p;
	wjq_log(LOG_INFO,"run start task(no rtos)\r\n");
	start_task(p);
	#endif
	while(1);
  
}

/**
 *@brief:      start_task
 *@details:    
 			   
 *@param[in]   void *pvParameters  
 *@param[out]  
 *@retval:     
 */
void start_task(void *pvParameters)
{

	wjq_log(LOG_INFO,"start task---\r\n");

	board_main_init();
	
	while (1)
	{
		vTaskDelay(2);
		board_low_task();
	}
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
