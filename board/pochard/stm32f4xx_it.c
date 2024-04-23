/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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

//#include "usb_bsp.h"
//#include "usb_hcd_int.h"
//#include "usb_dcd_int.h"
//#include "usbh_core.h"
//#include "dual_func_demo.h"

#include "stm32f4xx_it.h"
#include "log.h"
#include "board_sysconf.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
//extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
//extern USBH_HOST                    USB_Host;
//extern void USB_OTG_BSP_TimerIRQ (void);

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
//extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
//extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif
/* Private functions ---------------------------------------------------------*/
extern void mcu_uart1_IRQhandler(void);
extern void mcu_uart2_IRQhandler(void);
extern void mcu_uart3_IRQhandler(void);
extern void mcu_tim5_IRQhandler(void);
extern void mcu_tim3_IRQhandler(void);

#include "stm324xg_eval_sdio_sd.h"
extern SD_Error SD_ProcessIRQSrc(void);
extern void SD_ProcessDMAIRQ(void);

extern void mcu_i2s_dma_process(void);
extern void mcu_tim7_IRQhandler(void);
extern void mcu_adc_IRQhandler(void);
extern void mcu_can1_rx0_IRQ(void);
extern void mcu_i2sext_dma_process(void);

extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );
extern void vPortSVCHandler( void );

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	wjq_log(LOG_ERR, "NMI_Handler\r\n");
	while (1)	{
	}

}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
#if 0
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  wjq_log(LOG_ERR, "HardFault_Handler\r\n");
  while (1)  {
  }
}
#endif
/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  wjq_log(LOG_ERR, "MemManage_Handler\r\n");
  while (1)   {
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
  while (1)  {
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
  while (1)  {
  }
}
#if 0
/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
    //uart_printf("SVC_Handler\r\n");
	//vPortSVCHandler();
}
#endif
/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
    wjq_log(LOG_ERR, "DebugMon_Handler\r\n");
}
#if 0
/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
    //uart_printf("PendSV_Handler\r\n");
	//xPortPendSVHandler();

}
#endif
/**
  * @brief This function handles System tick timer.
  */
#include "FreeRTOS.h"
#include "task.h"

/* cmsis os v2 */
void osSystickHandler(void)
{

#if (INCLUDE_xTaskGetSchedulerState  == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif  /* INCLUDE_xTaskGetSchedulerState */  
    xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
  }
#endif  /* INCLUDE_xTaskGetSchedulerState */  
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  //wjq_log(LOG_DEBUG, "DebugMon_Handler\r\n");
  Time_Update();
  /* rtos的接口放在这里     */
   osSystickHandler();

}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
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

/**
  * @brief  This function handles SDIO global interrupt request.
  * @param  None
  * @retval None
  */
void SDIO_IRQHandler(void)
{
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
}

/**
  * @brief  This function handles DMA2 Stream3 or DMA2 Stream6 global interrupts
  *         requests.
  * @param  None
  * @retval None
  */
//void SD_SDIO_DMA_IRQHANDLER(void)
void DMA2_Stream3_IRQHandler(void)
{
  /* Process DMA2 Stream3 or DMA2 Stream6 Interrupt Sources */
  SD_ProcessDMAIRQ();
}

void DMA1_Stream5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5)==SET)		// 判断是否完成传输
	{ 
		DMA_ClearITPendingBit(DMA1_Stream5,DMA_IT_TCIF5);	//	清除相应的标志位
		mcu_i2s_dma_process();		
	} 

}
void DMA1_Stream0_IRQHandler(void)
{  
	if(DMA_GetITStatus(DMA1_Stream0,DMA_IT_TCIF0)==SET) //DMA1_Stream3,传输完成标志
	{ 
		DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);	//清除传输完成中断
		mcu_i2sext_dma_process();	//执行回调函数,读取数据等操作在这里面处理  
	}
}

void ADC_IRQHandler(void)
{
	mcu_adc_IRQhandler();
}

#if 0
/**
  * @brief  EXTI2_IRQHandler
  *         This function handles External line 1 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{

  if(EXTI_GetITStatus(EXTI_Line2) != RESET)
  {
      USB_Host.usr_cb->OverCurrentDetected();
      EXTI_ClearITPendingBit(EXTI_Line2);
  }
}
/**
  * @brief  TIM2_IRQHandler
  *         This function handles Timer2 Handler.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
  USB_OTG_BSP_TimerIRQ();
}


/**
  * @brief  OTG_FS_IRQHandler
  *          This function handles USB-On-The-Go FS global interrupt request.
  *          requests.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_FS  
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
	/* ensure that we are in device mode */
  if (USB_OTG_IsHostMode(&USB_OTG_Core)) 
  {
    USBH_OTG_ISR_Handler(&USB_OTG_Core);
  }
  else
  {
    USBD_OTG_ISR_Handler(&USB_OTG_Core);
  }
  
}


#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
  USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_Core);
}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_OUT_IRQHandler(void)
{
  USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_Core);
}
#endif


#include "stm32f4x7_eth_bsp.h"
/**
  * @brief  This function handles External line 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetITStatus(ETH_LINK_EXTI_LINE) != RESET)
  {
    Eth_Link_ITHandler(ETH_PHY_ADRESS);
    /* Clear interrupt pending bit */
    EXTI_ClearITPendingBit(ETH_LINK_EXTI_LINE);
  }

    if(EXTI_GetITStatus(EXTI_Line10) != RESET)
  {
    /* Clear interrupt pending bit */
    EXTI_ClearITPendingBit(EXTI_Line10);
  }
}
#endif
/**
  * @brief  This function handles CAN1 RX0 request.
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{

	mcu_can1_rx0_IRQ();
}
/*
	摄像头帧中断
*/
void DCMI_IRQHandler(void)
{
	if(DCMI_GetITStatus(DCMI_IT_FRAME)==SET)
	{
		DCMI_ClearITPendingBit(DCMI_IT_FRAME);
		mcu_dcmi_frame_process();
	}
} 

void DMA2_Stream1_IRQHandler(void)
{        
	if(DMA_GetFlagStatus(DMA2_Stream1,DMA_FLAG_TCIF1)==SET)
	{  
		 DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1);
		 mcu_dcmi_dma_process();
	}    											 
}  


/**
  * @brief  EXTI15_10_IRQHandler
  *         This function handles External line  interrupt request.
  * @param  None
  * @retval None
  */
extern void ESP_HOST_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

void EXTI15_10_IRQHandler(void)
{

  if(EXTI_GetITStatus(EXTI_Line12) != RESET)
  {
      ESP_HOST_GPIO_EXTI_Callback(GPIO_DATA_READY_Pin);
      EXTI_ClearITPendingBit(EXTI_Line12);
  }

  if(EXTI_GetITStatus(EXTI_Line13) != RESET)
  {
      ESP_HOST_GPIO_EXTI_Callback(GPIO_HANDSHAKE_Pin);
      EXTI_ClearITPendingBit(EXTI_Line13);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
