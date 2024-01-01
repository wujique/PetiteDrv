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
#include "stm32f4xx.h"
#include "mcu_fsmc.h"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */

/** @addtogroup STM324xG_EVAL
  * @{
  */

/** @addtogroup STM324xG_EVAL_FSMC_SRAM
  * @brief     This file provides a set of functions needed to drive the 
  *            CY7C1071DV33-12BAXI SRAM memory mounted on STM324xG-EVAL board.
  * @{
  */

/** @defgroup STM324xG_EVAL_FSMC_SRAM_Private_Types
  * @{
  */
/**
  * @}
  */


/** @defgroup STM324xG_EVAL_FSMC_SRAM_Private_Defines
  * @{
  */
/**
  * @brief  FSMC Bank 1 NOR/SRAM2
  */
#if 0
#define Bank1_SRAM2_ADDR  ((uint32_t)0x64000000)  

/**
  * @}
  */


/** @defgroup STM324xG_EVAL_FSMC_SRAM_Private_Macros
  * @{
  */
/**
  * @}
  */


/** @defgroup STM324xG_EVAL_FSMC_SRAM_Private_Variables
  * @{
  */
/**
  * @}
  */


/** @defgroup STM324xG_EVAL_FSMC_SRAM_Private_Function_Prototypes
  * @{
  */
/**
  * @}
  */


/** @defgroup STM324xG_EVAL_FSMC_SRAM_Private_Functions
  * @{
  */

/**
  * @brief  Configures the FSMC and GPIOs to interface with the SRAM memory.
  *         This function must be called before any write/read operation
  *         on the SRAM.
  * @param  None
  * @retval None
  */
void SRAM_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  /* Enable GPIOs clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
                         RCC_AHB1Periph_GPIOG, ENABLE);

  /* Enable FSMC clock */
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 
  
/*-- GPIOs Configuration -----------------------------------------------------*/
/*
 +-------------------+--------------------+------------------+------------------+
 | PD0  <-> FSMC_D2  | PE0  <-> FSMC_NBL0 | PF0 <-> FSMC_A0  | PG0 <-> FSMC_A10 |
 | PD1  <-> FSMC_D3  | PE1  <-> FSMC_NBL1 | PF1 <-> FSMC_A1  | PG1 <-> FSMC_A11 |
 | PD4  <-> FSMC_NOE | PE2  <-> FSMC_A23  | PF2 <-> FSMC_A2  | PG2 <-> FSMC_A12 |
 | PD5  <-> FSMC_NWE | PE3  <-> FSMC_A19  | PF3 <-> FSMC_A3  | PG3 <-> FSMC_A13 |
 | PD8  <-> FSMC_D13 | PE4  <-> FSMC_A20  | PF4 <-> FSMC_A4  | PG4 <-> FSMC_A14 |
 | PD9  <-> FSMC_D14 | PE5  <-> FSMC_A21  | PF5 <-> FSMC_A5  | PG5 <-> FSMC_A15 |
 | PD10 <-> FSMC_D15 | PE6  <-> FSMC_A22  | PF12 <-> FSMC_A6 | PG9 <-> FSMC_NE2 |
 | PD11 <-> FSMC_A16 | PE7  <-> FSMC_D4   | PF13 <-> FSMC_A7 |------------------+
 | PD12 <-> FSMC_A17 | PE8  <-> FSMC_D5   | PF14 <-> FSMC_A8 |
 | PD13 <-> FSMC_A18 | PE9  <-> FSMC_D6   | PF15 <-> FSMC_A9 |
 | PD14 <-> FSMC_D0  | PE10 <-> FSMC_D7   |------------------+
 | PD15 <-> FSMC_D1  | PE11 <-> FSMC_D8   |
 +-------------------| PE12 <-> FSMC_D9   |
                     | PE13 <-> FSMC_D10  |
                     | PE14 <-> FSMC_D11  |
                     | PE15 <-> FSMC_D12  |
                     +--------------------+
*/

  /* GPIOD configuration */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_4  | GPIO_Pin_5  | 
                                GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOD, &GPIO_InitStructure);


  /* GPIOE configuration */
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource2 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource6 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |  
                                GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7 |
                                GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

  GPIO_Init(GPIOE, &GPIO_InitStructure);


  /* GPIOF configuration */
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3  | 
                                GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_12 | GPIO_Pin_13 |
                                GPIO_Pin_14 | GPIO_Pin_15;      

  GPIO_Init(GPIOF, &GPIO_InitStructure);


  /* GPIOG configuration */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 | 
                                GPIO_Pin_4  | GPIO_Pin_5  |GPIO_Pin_9;      

  GPIO_Init(GPIOG, &GPIO_InitStructure);

/*-- FSMC Configuration ------------------------------------------------------*/
  p.FSMC_AddressSetupTime = 1;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 2;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM2;
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
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /*!< Enable FSMC Bank1_SRAM2 Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE); 

}

/**
  * @brief  Writes a Half-word buffer to the FSMC SRAM memory.
  * @param  pBuffer : pointer to buffer.
  * @param  WriteAddr : SRAM memory internal address from which the data will be
  *         written.
  * @param  NumHalfwordToWrite : number of half-words to write.
  * @retval None
  */
void SRAM_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
  for (; NumHalfwordToWrite != 0; NumHalfwordToWrite--) /* while there is data to write */
  {
    /* Transfer data to the memory */
    *(uint16_t *) (Bank1_SRAM2_ADDR + WriteAddr) = *pBuffer++;

    /* Increment the address*/
    WriteAddr += 2;
  }
}

/**
  * @brief  Reads a block of data from the FSMC SRAM memory.
  * @param  pBuffer : pointer to the buffer that receives the data read from the
  *         SRAM memory.
  * @param  ReadAddr : SRAM memory internal address to read from.
  * @param  NumHalfwordToRead : number of half-words to read.
  * @retval None
  */
void SRAM_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead)
{
  for (; NumHalfwordToRead != 0; NumHalfwordToRead--) /* while there is data to read */
  {
    /* Read a half-word from the memory */
    *pBuffer++ = *(__IO uint16_t*) (Bank1_SRAM2_ADDR + ReadAddr);

    /* Increment the address*/
    ReadAddr += 2;
  }
}
#endif

typedef struct{
  GPIO_TypeDef* port;
  uint16_t pinsrc;
  uint32_t pin;

}Stm32F407IoCnf;


typedef struct{
  uint32_t reg_addr;
  uint32_t data_addr;
  uint32_t ne;// 区域，通过NE选择，1,2,3,4
  uint16_t ionum; // 要配置的IO数
  Stm32F407IoCnf ioconf[20];
}Stm32FsmcLcdDef;


/*
	写寄存器要两步
	*LcdReg = LCD_Reg; //写入要写的寄存器序号
	*LcdData = LCD_RegValue; //写入数据 

	FSMC bank1地址0x6000 0000, 通过NE管脚选择
	分四个区域,每个区域64M，偏移地址分别是
		0x000 0000
		0X400 0000
		0X800 0000
		0XC00 0000

	无论是8位还是16位位宽，A0接设备A0。FSMC内部控制器自动对地址做右移一位处理。
	用来区分命令还是数据的管脚接在地址线上，如果是16位宽，内部FSMC会对地址进行右移一位。
	因此，如果接在A15上，在地址上就应该选A16位。
*/
/** 配置1，在STM32F407ZGT，wujique板*/
Stm32FsmcLcdDef Stm32FsmcLcd_1 = {
  .reg_addr =  0x6C000000,
  .data_addr = 0x6C010000,//A15
  .ne = FSMC_Bank1_NORSRAM4,
  .ionum = 20,
  .ioconf = {
      {GPIOD, GPIO_PinSource0, GPIO_Pin_0}, //PD0	<-> FSMC_D2 
      {GPIOD, GPIO_PinSource1, GPIO_Pin_1}, //PD1	<-> FSMC_D3  
      {GPIOD, GPIO_PinSource4, GPIO_Pin_4}, //PD4	<-> FSMC_NOE 
      {GPIOD, GPIO_PinSource5, GPIO_Pin_5}, //PD5	<-> FSMC_NWE 
      {GPIOD, GPIO_PinSource8, GPIO_Pin_8}, //PD8	<-> FSMC_D13 
      {GPIOD, GPIO_PinSource9, GPIO_Pin_9}, //PD9	<-> FSMC_D14 
      {GPIOD, GPIO_PinSource10, GPIO_Pin_10}, //PD10 <-> FSMC_D15 
      {GPIOD, GPIO_PinSource14, GPIO_Pin_14}, //PD14 <-> FSMC_D0  
      {GPIOD, GPIO_PinSource15, GPIO_Pin_15}, //PD15 <-> FSMC_D1   
      {GPIOE, GPIO_PinSource7 , GPIO_Pin_7}, //PE7	<-> FSMC_D4  
      {GPIOE, GPIO_PinSource8 , GPIO_Pin_8}, //PE8	<-> FSMC_D5  
      {GPIOE, GPIO_PinSource9 , GPIO_Pin_9}, //PE9	<-> FSMC_D6  
      {GPIOE, GPIO_PinSource10 , GPIO_Pin_10}, //PE10 <-> FSMC_D7 
      {GPIOE, GPIO_PinSource11 , GPIO_Pin_11}, //PE11 <-> FSMC_D8   
      {GPIOE, GPIO_PinSource12 , GPIO_Pin_12}, //PE12 <-> FSMC_D9 
      {GPIOE, GPIO_PinSource13 , GPIO_Pin_13}, //PE13 <-> FSMC_D10  
      {GPIOE, GPIO_PinSource14 , GPIO_Pin_14}, //PE14 <-> FSMC_D11  
      {GPIOE, GPIO_PinSource15 , GPIO_Pin_15}, //PE15 <-> FSMC_D12
      {GPIOG, GPIO_PinSource5 , GPIO_Pin_5}, //PG5 <-> FSMC_A15 |
      {GPIOG, GPIO_PinSource12 , GPIO_Pin_12}, //PG12 <-> FSMC_NE4 |
  },
};
/** 配置2，在stm32f407vgt pochard板 */
Stm32FsmcLcdDef Stm32FsmcLcd_2 = {
  .reg_addr =  0x60000000,
  .data_addr = 0x60100000,//A19
  .ne = FSMC_Bank1_NORSRAM1,
  .ionum = 20,
  .ioconf = {
      {GPIOE, GPIO_PinSource3 , GPIO_Pin_3}, //  PE3   ------> FSMC_A19
      {GPIOE, GPIO_PinSource7 , GPIO_Pin_7}, //  PE7   ------> FSMC_D4
      {GPIOE, GPIO_PinSource8 , GPIO_Pin_8}, //  PE8   ------> FSMC_D5
      {GPIOE, GPIO_PinSource9 , GPIO_Pin_9}, //  PE9   ------> FSMC_D6
      {GPIOE, GPIO_PinSource10 , GPIO_Pin_10}, //  PE10   ------> FSMC_D7
      {GPIOE, GPIO_PinSource11 , GPIO_Pin_11}, //  PE11   ------> FSMC_D8
      {GPIOE, GPIO_PinSource12 , GPIO_Pin_12}, //  PE12   ------> FSMC_D9
      {GPIOE, GPIO_PinSource13 , GPIO_Pin_13}, //  PE13   ------> FSMC_D10
      {GPIOE, GPIO_PinSource14 , GPIO_Pin_14}, //  PE14   ------> FSMC_D11
      {GPIOE, GPIO_PinSource15 , GPIO_Pin_15}, //  PE15   ------> FSMC_D12
      {GPIOD, GPIO_PinSource0, GPIO_Pin_0}, //  PD0   ------> FSMC_D2
      {GPIOD, GPIO_PinSource1, GPIO_Pin_1}, //   PD1   ------> FSMC_D3
      {GPIOD, GPIO_PinSource4, GPIO_Pin_4}, //   PD4   ------> FSMC_NOE
      {GPIOD, GPIO_PinSource5, GPIO_Pin_5}, //   PD5   ------> FSMC_NWE
      {GPIOD, GPIO_PinSource7, GPIO_Pin_7}, //   PD7   ------> FSMC_NE1
      {GPIOD, GPIO_PinSource8, GPIO_Pin_8}, //   PD8   ------> FSMC_D13
      {GPIOD, GPIO_PinSource9, GPIO_Pin_9},  //  PD9   ------> FSMC_D14
      {GPIOD, GPIO_PinSource10, GPIO_Pin_10}, //  PD10   ------> FSMC_D15
      {GPIOD, GPIO_PinSource14, GPIO_Pin_14}, //  PD14   ------> FSMC_D0
      {GPIOD, GPIO_PinSource15, GPIO_Pin_15}, //  PD15   ------> FSMC_D1
  },
};

volatile u16 *LcdReg = NULL;
volatile u16 *LcdData = NULL;

/**
 * @brief   保存fsmc参数，以便获取到LCD型号后重新配置
 * 
 * 
 */
static FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
/**
 * @brief   重新配置FSMC时序
 * 
 * @param   AddressSetupTime 
 * @param   DataSetupTime 
 * 
 */
void mcu_fsmc_reconfig_write_time(uint32_t AddressSetupTime, uint32_t DataSetupTime)
{
  
  FSMC_NORSRAMCmd(FSMC_NORSRAMInitStructure.FSMC_Bank, DISABLE); 
  
/*-- FSMC Configuration ------------------------------------------------------*/
  FSMC_NORSRAMTimingInitTypeDef  w,r;
  w.FSMC_AddressSetupTime = AddressSetupTime;
  w.FSMC_AddressHoldTime = 0;
  w.FSMC_DataSetupTime = DataSetupTime;
  w.FSMC_BusTurnAroundDuration = 0;
  w.FSMC_CLKDivision = 0;
  w.FSMC_DataLatency = 0;
  w.FSMC_AccessMode = FSMC_AccessMode_A;

  r.FSMC_AddressSetupTime = 16;
  r.FSMC_AddressHoldTime = 0;
  r.FSMC_DataSetupTime = 24;
  r.FSMC_BusTurnAroundDuration = 0;
  r.FSMC_CLKDivision = 0;
  r.FSMC_DataLatency = 0;
  r.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &r;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &w;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /*!< Enable FSMC Bank1_SRAM4 Bank */
  FSMC_NORSRAMCmd(FSMC_NORSRAMInitStructure.FSMC_Bank, ENABLE); 

}

/**
 *@brief:      mcu_fsmc_lcd_Init
 *@details:    LCD使用的FSMC初始化
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_fsmc_lcd_Init(uint8_t conf_num)
{

  
  GPIO_InitTypeDef GPIO_InitStructure; 
  Stm32FsmcLcdDef *fsmc_conf;

  if(conf_num == 1) {
    fsmc_conf = &Stm32FsmcLcd_1;
  } else if(conf_num == 2) {
    fsmc_conf = &Stm32FsmcLcd_2;
  } 

  LcdReg = (uint16_t *)fsmc_conf->reg_addr;
  LcdData = (uint16_t *)fsmc_conf->data_addr;
  /* Enable GPIOs clock */
  //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE |
		//				 RCC_AHB1Periph_GPIOG, ENABLE);

  /* Enable FSMC clock */
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 
  

  /* GPIOD configuration */
  uint16_t index = 0;
  Stm32F407IoCnf *p_io_conf = &fsmc_conf->ioconf[0];

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;

  while(1) {
    if (index >= fsmc_conf->ionum)
      break;

    GPIO_PinAFConfig(p_io_conf->port, p_io_conf->pinsrc, GPIO_AF_FSMC);  
    GPIO_InitStructure.GPIO_Pin = p_io_conf->pin;
    GPIO_Init(p_io_conf->port, &GPIO_InitStructure);
    p_io_conf++;
    index++;
  }

/*-- FSMC Configuration ------------------------------------------------------*/
  FSMC_NORSRAMTimingInitTypeDef  w,r;
  w.FSMC_AddressSetupTime = 10;
  w.FSMC_AddressHoldTime = 0;
  w.FSMC_DataSetupTime = 10;
  w.FSMC_BusTurnAroundDuration = 0;
  w.FSMC_CLKDivision = 0;
  w.FSMC_DataLatency = 0;
  w.FSMC_AccessMode = FSMC_AccessMode_A;

  r.FSMC_AddressSetupTime = 16;
  r.FSMC_AddressHoldTime = 0;
  r.FSMC_DataSetupTime = 24;
  r.FSMC_BusTurnAroundDuration = 0;
  r.FSMC_CLKDivision = 0;
  r.FSMC_DataLatency = 0;
  r.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = fsmc_conf->ne;
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

  /*!< Enable FSMC Bank1_SRAM4 Bank */
  FSMC_NORSRAMCmd(fsmc_conf->ne, ENABLE); 

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
