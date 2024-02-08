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
#include "mcu.h"
#include "log.h"
#include "stimer.h"
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

RCC_ClocksTypeDef RCC_Clocks;

/*
	cpu初始化
*/
s32 board_mcu_preinit(void)
{
	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
	/*
		中断优先级分组
	*/	
	/* 2 bit for pre-emption priority, 2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  
	RCC_GetClocksFreq(&RCC_Clocks);
		
	/*IO跟串口是调试信息的依赖，所以最早初始化*/
	mcu_io_init();
	mcu_uart_init();
	mcu_uart_open(PC_PORT);
	log_init();
	wjq_log(LOG_DEBUG, "****************************************\r\n");
	wjq_log(LOG_DEBUG, "        Board canary stm32f103\r\n");
	wjq_log(LOG_DEBUG, "****************************************\r\n\r\n");
	wjq_log(LOG_DEBUG, "***SYSCLK_Frequency:%d Hz\r\n", RCC_Clocks.SYSCLK_Frequency);
	wjq_log(LOG_DEBUG, "***HCLK_Frequency:  %d Hz\r\n", RCC_Clocks.HCLK_Frequency);
	wjq_log(LOG_DEBUG, "***PCLK1_Frequency: %d Hz\r\n", RCC_Clocks.PCLK1_Frequency);
	wjq_log(LOG_DEBUG, "***PCLK2_Frequency: %d Hz\r\n", RCC_Clocks.PCLK2_Frequency);
	wjq_log(LOG_DEBUG, "***ADCCLK_Frequency:%d Hz\r\n", RCC_Clocks.ADCCLK_Frequency);
	
	#ifndef SYS_USE_RTOS
	/* SysTick end of count event */
	wjq_log(LOG_DEBUG, "init systick no rtos\r\n");
	SysTick_Config(RCC_Clocks.HCLK_Frequency / (1000/SYSTEMTICK_PERIOD_MS));
	#endif

	//mcu_rtc_init();
	return 0;
}

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
 
	/*跳转到petite app*/
	petite_app();

	while(1);
  
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
