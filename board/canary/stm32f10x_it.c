/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "log.h"
#include "board_sysconf.h"



/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );
extern void vPortSVCHandler( void );
extern void Time_Update(void);
/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	wjq_log(LOG_ERR, "NMI_Handler\r\n");
	while (1)
	{
	}

}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	wjq_log(LOG_ERR, "HardFault_Handler\r\n");
	while (1)
	{
	}

}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	wjq_log(LOG_ERR, "MemManage_Handler\r\n");
	while (1)
	{
	}

}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	wjq_log(LOG_ERR, "BusFault_Handler\r\n");
	while (1)
	{
	}

}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	wjq_log(LOG_ERR, "UsageFault_Handler\r\n");
	while (1)
	{
	}

}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
    //wjq_log(LOG_ERR, "SVC_Handler\r\n");
#ifdef SYS_USE_RTOS
	vPortSVCHandler();
#endif

}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
	wjq_log(LOG_ERR, "DebugMon_Handler\r\n");

}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
    //wjq_log(LOG_ERR, "PendSV_Handler\r\n");
#ifdef SYS_USE_RTOS
	xPortPendSVHandler();
#endif

}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	Time_Update();
	/*
	  freertos的接口放在这里
	*/
#ifdef SYS_USE_RTOS
	xPortSysTickHandler();
#endif

}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 
/*--------------串口中断---------------*/
extern void mcu_uart1_IRQhandler(void);
extern void mcu_uart2_IRQhandler(void);
extern void mcu_uart3_IRQhandler(void);

void USART3_IRQHandler(void)
{
	mcu_uart3_IRQhandler();    
}

void USART2_IRQHandler(void)
{
	mcu_uart2_IRQhandler();    
}
void USART1_IRQHandler(void)
{
	mcu_uart1_IRQhandler();    
}
void TIM5_IRQHandler(void)
{
    mcu_tim5_IRQhandler();
}

void TIM3_IRQHandler(void)
{
    mcu_tim3_IRQhandler();
}

void TIM7_IRQHandler(void)
{
    mcu_tim7_IRQhandler();
}

/*--------------------------------------*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
