/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "log.h"
#include "FreeRTos.h"

extern s32 board_mcu_preinit(void);
extern s32 board_mcu_init(void);
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
int main(void)
{
 /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files before to branch to application main.
       To reconfigure the default setting of SystemInit() function, 
       refer to system_stm32f4xx.c file */

	board_mcu_preinit();
	
	wjq_log(LOG_INFO,"\r\n---hello world!--20190401 10:41---\r\n");
		
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

	board_mcu_init();	
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
