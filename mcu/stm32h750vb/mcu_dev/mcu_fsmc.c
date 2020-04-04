/**
  ******************************************************************************
  * @file    stm324xg_eval_fsmc_sram.c
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    19-September-2013
  * @brief   This file provides a set of functions needed to drive the
  *          IS61WV102416BLL SRAM memory mounted on STM324xG-EVAL evaluation
  *          board(MB786) RevB.    
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
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
#include "mcu_fsmc.h"

/*
	写寄存器要两步
	*LcdReg = LCD_Reg; //写入要写的寄存器序号
	*LcdData = LCD_RegValue; //写入数据 

	FSMC bank1地址0x6000 0000
	分四个区域,每个区域64M，偏移地址分别是
		0x000 0000
		0X400 0000
		0X800 0000
		0XC00 0000

	无论是8位还是16位位宽，A0接设备A0。FSMC内部控制器自动对地址做右移一位处理。
	用来区分命令还是数据的管脚接在地址线上，如果是16位宽，内部FSMC会对地址进行右移一位。
	因此，如果接在A15上，在地址上就应该选A16位。
*/
volatile u16 *LcdReg =  (u16*)0x60000000;
volatile u16 *LcdData = (u16*)0x60100000;

SRAM_HandleTypeDef McuHsram1;
static void mcu_MX_FMC_Init(void);

/**
 *@brief:      mcu_fsmc_lcd_Init
 *@details:    LCD使用的FSMC初始化
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_fsmc_lcd_Init(void)
{
	wjq_log(LOG_DEBUG, ">--------mcu_fsmc_lcd_Init\r\n");
	mcu_MX_FMC_Init();  
}

/**
  * @}
  */
/* FMC initialization function */
static void mcu_MX_FMC_Init(void)
{
#if 0
  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_NORSRAM_TimingTypeDef WTiming = {0};
	FMC_NORSRAM_TimingTypeDef RTiming = {0};
  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SRAM1 memory initialization sequence
  */
  McuHsram1.Instance = FMC_NORSRAM_DEVICE;
  McuHsram1.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  McuHsram1.Init.NSBank = FMC_NORSRAM_BANK1;
  McuHsram1.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
  McuHsram1.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
  McuHsram1.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  McuHsram1.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
  McuHsram1.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;

  McuHsram1.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
  McuHsram1.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  McuHsram1.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
  McuHsram1.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
  McuHsram1.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  McuHsram1.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
  McuHsram1.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  McuHsram1.Init.WriteFifo = FMC_WRITE_FIFO_DISABLE;
  McuHsram1.Init.PageSize = FMC_PAGE_SIZE_NONE;

  /* Timing */
  RTiming.AddressSetupTime = 15;
  RTiming.AddressHoldTime = 10;
  RTiming.DataSetupTime = 200;
  RTiming.BusTurnAroundDuration = 15;
  RTiming.CLKDivision = 16;
  RTiming.DataLatency = 17;
  RTiming.AccessMode = FMC_ACCESS_MODE_A;
  
  WTiming.AddressSetupTime = 21;
  WTiming.AddressHoldTime = 10;
  WTiming.DataSetupTime = 100;
  WTiming.BusTurnAroundDuration = 15;
  WTiming.CLKDivision = 16;
  WTiming.DataLatency = 17;
  WTiming.AccessMode = FMC_ACCESS_MODE_A;
  /* ExtTiming */

  if (HAL_SRAM_Init(&McuHsram1, &RTiming, &WTiming) != HAL_OK)
  {
	Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */
#endif
  
	  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_NORSRAM_TimingTypeDef Timing = {0};
  FMC_NORSRAM_TimingTypeDef ExtTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SRAM1 memory initialization sequence
  */
  McuHsram1.Instance = FMC_NORSRAM_DEVICE;
  McuHsram1.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  McuHsram1.Init.NSBank = FMC_NORSRAM_BANK1;
  McuHsram1.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
  McuHsram1.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;
  McuHsram1.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  McuHsram1.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
  McuHsram1.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  McuHsram1.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
  McuHsram1.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
  McuHsram1.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
  McuHsram1.Init.ExtendedMode = FMC_EXTENDED_MODE_ENABLE;
  McuHsram1.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  McuHsram1.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
  McuHsram1.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  McuHsram1.Init.WriteFifo = FMC_WRITE_FIFO_DISABLE;
  McuHsram1.Init.PageSize = FMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 0x11;
  Timing.AddressHoldTime = 0;
  Timing.DataSetupTime = 0x55;
  Timing.BusTurnAroundDuration = 2;
  Timing.CLKDivision = 16;
  Timing.DataLatency = 17;
  Timing.AccessMode = FMC_ACCESS_MODE_A;
  /* ExtTiming */
  ExtTiming.AddressSetupTime = 0x15;
  ExtTiming.AddressHoldTime = 0;
  ExtTiming.DataSetupTime = 0x15;
  ExtTiming.BusTurnAroundDuration = 2;
  ExtTiming.CLKDivision = 16;
  ExtTiming.DataLatency = 17;
  ExtTiming.AccessMode = FMC_ACCESS_MODE_A;

  if (HAL_SRAM_Init(&McuHsram1, &Timing, &ExtTiming) != HAL_OK)
  {
    Error_Handler( );
  }
  /* USER CODE END FMC_Init 2 */
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
