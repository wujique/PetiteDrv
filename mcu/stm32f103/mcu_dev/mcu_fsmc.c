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
volatile u16 *LcdData = (u16*)0x60020000;

/**
 *@brief:      mcu_fsmc_lcd_Init
 *@details:    LCD使用的FSMC初始化
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_fsmc_lcd_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  w,r;
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  /* Enable FSMC clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
/*-- GPIOs Configuration -----------------------------------------------------*/
/*
 +-------------------+--------------------+------------------+------------------+
PD0	<-> FSMC_D2  
PD1	<-> FSMC_D3  
PD4	<-> FSMC_NOE 
PD5	<-> FSMC_NWE 
PD8	<-> FSMC_D13  
PD9	<-> FSMC_D14 
PD10 <-> FSMC_D15 
PD14 <-> FSMC_D0   
PD15 <-> FSMC_D1   

PE7	<-> FSMC_D4    
PE8	<-> FSMC_D5   
PE9	<-> FSMC_D6  
PE10 <-> FSMC_D7   
PE11 <-> FSMC_D8   
PE12 <-> FSMC_D9   
PE13 <-> FSMC_D10  
PE14 <-> FSMC_D11  
PE15 <-> FSMC_D12  


PD11 <-> FSMC_A16 |	
PD7 <-> FSMC_NE1 |
*/


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0	| GPIO_Pin_1  | GPIO_Pin_4	| GPIO_Pin_5  | 
								GPIO_Pin_8	| GPIO_Pin_9  | GPIO_Pin_10 |GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* GPIOE configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 |
								GPIO_Pin_8	| GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
								GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* GPIOG configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11  |GPIO_Pin_7;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

/*-- FSMC Configuration ------------------------------------------------------*/
  w.FSMC_AddressSetupTime = 0x0;
  w.FSMC_AddressHoldTime = 0x0;
  w.FSMC_DataSetupTime = 0x1;
  w.FSMC_BusTurnAroundDuration = 0x00;
  w.FSMC_CLKDivision = 0x01;
  w.FSMC_DataLatency = 0;
  w.FSMC_AccessMode = FSMC_AccessMode_A;

  r.FSMC_AddressSetupTime = 1;
  r.FSMC_AddressHoldTime = 0;
  r.FSMC_DataSetupTime = 0xF;
  r.FSMC_BusTurnAroundDuration = 0;
  r.FSMC_CLKDivision = 0x01;
  r.FSMC_DataLatency = 0;
  r.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &r;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &w;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /*!< Enable FSMC Bank1_SRAM1 Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); 

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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
