/**
 * @file            mcu_uart.c
 * @brief           串口驱动
 * @author          wujique
 * @date            2017年10月24日 星期二
 * @version         初稿
 * @par             
 * @par History:
 * 1.日    期:      2017年10月24日 星期二
 *   作    者:     
 *   修改内容:        创建文件
		版权说明：
		1 源码归屋脊雀工作室所有。
		2 可以用于的其他商业用途（配套开发板销售除外），不须授权。
		3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
		4 可随意修改源码并分发，但不可直接销售本代码获利，并且请保留WUJIQUE版权说明。
		5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
		6 使用本源码则相当于认同本版权说明。
		7 如侵犯你的权利，请联系：code@wujique.com
		8 一切解释权归屋脊雀工作室所有。
*/
#include "mcu.h"
#include "mcu_uart.h"
#include "log.h"
#include "alloc.h"
#include "p_kfifo.h"


static BusUartNode *McuUart[MCU_UART_MAX];

UART_HandleTypeDef McuHuart4;
UART_HandleTypeDef McuHuart2;

/*------------------------------------------------------*/
/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
/*
	要注意，硬件初始化函数在
	void HAL_UART_MspInit(UART_HandleTypeDef* huart)；
	这是由cube生成的。
*/
static void mcu_MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  McuHuart4.Instance = UART4;
  McuHuart4.Init.BaudRate = 115200;
  McuHuart4.Init.WordLength = UART_WORDLENGTH_8B;
  McuHuart4.Init.StopBits = UART_STOPBITS_1;
  McuHuart4.Init.Parity = UART_PARITY_NONE;
  McuHuart4.Init.Mode = UART_MODE_TX_RX;
  McuHuart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  McuHuart4.Init.OverSampling = UART_OVERSAMPLING_16;
  McuHuart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  McuHuart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  McuHuart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&McuHuart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&McuHuart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&McuHuart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&McuHuart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void mcu_MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  McuHuart2.Instance = USART2;
  McuHuart2.Init.BaudRate = 115200;
  McuHuart2.Init.WordLength = UART_WORDLENGTH_8B;
  McuHuart2.Init.StopBits = UART_STOPBITS_1;
  McuHuart2.Init.Parity = UART_PARITY_NONE;
  McuHuart2.Init.Mode = UART_MODE_TX_RX;
  McuHuart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  McuHuart2.Init.OverSampling = UART_OVERSAMPLING_16;
  McuHuart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  McuHuart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  McuHuart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&McuHuart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&McuHuart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&McuHuart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&McuHuart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
 *@brief:      
 *@details:    打开串口，实际就是初始化串口
 *@param[in]   s32 comport  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_uart_init(McuUartNum comport, BusUartNode *BusNode, const BusUartPra *Pra)
{
	if(comport == MCU_UART_2){
		__HAL_RCC_USART2_CLK_ENABLE();
		mcu_MX_USART2_UART_Init();
		McuUart[MCU_UART_2] = BusNode;
	} else if(comport == MCU_UART_4) {
		__HAL_RCC_UART4_CLK_ENABLE();
		mcu_MX_UART4_Init();
		__HAL_UART_ENABLE_IT(&McuHuart4, UART_IT_RXNE);
		McuUart[MCU_UART_4] = BusNode;
	} else	{
		/* 串口号不支持*/
		return -1;
	}
	return (0);
}
/**
 *@brief:      mcu_uart_close
 *@details:    关闭串口
 *@param[in]   s32 comport  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_uart_deinit (McuUartNum comport)
{
	if(comport == MCU_UART_2) {
		
	} else if(comport == MCU_UART_4) {

	} else return -1;

	return(0);
}

/**
 *@brief:      mcu_dev_uart_set_baud
 *@details:    设置串口波特率
 *@param[in]   s32 comport   
               s32 baud      
               s32 databits  
               s32 parity    
               s32 stopbits  
               s32 flowctl   
 *@param[out]  无
 *@retval:     
 */
s32 mcu_uart_set_baud (McuUartNum comport, s32 baud)
{

	if(comport >= MCU_UART_MAX)
		return -1;
	
	return 0;
}

/**
 *@brief:      mcu_dev_uart_write
 *@details:    写串口数据(发送数据)
 *@param[in]   s32 comport  
               u8 *buf      
               s32 len      
 *@param[out]  无
 *@retval:     
 */
s32 mcu_uart_send (McuUartNum comport, u8 *buf, s32 len)
{
	u32 t;
	u16 ch;
  
	if (len <= 0) return(-1);
		
	if(buf == NULL) return(-1);
	
	if(comport == MCU_UART_2) {
		HAL_UART_Transmit(&McuHuart2, buf, len, 100);	
	}	else if(comport == MCU_UART_4)	{
		HAL_UART_Transmit(&McuHuart4, buf, len, 100);
	} else	return -1;
	
	return(0);
}

/*
	串口中断服务函数        */
void mcu_uart4_irq(void)
{
	char ch;
	struct _pkfifo *pfifo;

	pfifo = &(McuUart[MCU_UART_4]->Kfifo);
	/*  中断中仅仅处理了 UART_IT_RXNE，
	更稳健的做法，是对所有中断进行处理 */
	if((__HAL_UART_GET_IT(&McuHuart4, UART_IT_RXNE) != RESET) 
		&& (__HAL_UART_GET_IT_SOURCE(&McuHuart4, UART_IT_RXNE) != RESET)) {
		
		ch = (uint8_t)(McuHuart4.Instance->RDR);	

		PKFIFO_IN_1U8(pfifo, ch);
		
        __HAL_UART_CLEAR_IT(&McuHuart4, UART_IT_RXNE);
    }	
}



