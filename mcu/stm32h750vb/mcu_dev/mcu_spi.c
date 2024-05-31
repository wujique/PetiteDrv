/**
 * @file            mcu_spi.c
 * @brief           片上SPI控制器驱动
 * @author          test
 * @date            2017年10月26日 星期四
 * @version         初稿
 * @par             
 * @par History:
 * 1.日    期:      2017年10月26日 星期四
 *   作    者:      test
 *   修改内容:      创建文件
		版权说明：
		1 源码归屋脊雀工作室所有。
		2 可以用于的其他商业用途（配套开发板销售除外），不须授权。
		3 屋脊雀工作室不对代码功能做任何保证，请使用者自行测试，后果自负。
		4 可随意修改源码并分发，但不可直接销售本代码获利，并且保留版权说明。
		5 如发现BUG或有优化，欢迎发布更新。请联系：code@wujique.com
		6 使用本源码则相当于认同本版权说明。
		7 如侵犯你的权利，请联系：code@wujique.com
		8 一切解释权归屋脊雀工作室所有。
*/
#include "mcu.h"
#include "log.h"
#include "board_sysconf.h"

#define MCU_SPI_DEBUG

#ifdef MCU_SPI_DEBUG
#define SPI_DEBUG	wjq_log 
#else
#define SPI_DEBUG(a, ...)
#endif


#define MCU_SPI_WAIT_TIMEOUT 0x40000

/*
	相位配置，一共四种模式
*/
typedef struct
{
	/* 注意H750 是U32*/
	u32 CPOL;
	u32 CPHA;	
}_strSpiModeSet;

const _strSpiModeSet SpiModeSet[SPI_MODE_MAX]=
	{
		{SPI_POLARITY_LOW, SPI_PHASE_1EDGE},
		{SPI_POLARITY_LOW, SPI_PHASE_2EDGE},
		{SPI_POLARITY_HIGH, SPI_PHASE_1EDGE},
		{SPI_POLARITY_HIGH, SPI_PHASE_2EDGE}
	};

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
SPI_HandleTypeDef McuHspi2;
static void mcu_MX_SPI2_Init(void)
{
  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  McuHspi2.Instance = SPI2;
  McuHspi2.Init.Mode = SPI_MODE_MASTER;
  McuHspi2.Init.Direction = SPI_DIRECTION_2LINES;
  McuHspi2.Init.DataSize = SPI_DATASIZE_8BIT;  
  McuHspi2.Init.NSS = SPI_NSS_SOFT;
  McuHspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  McuHspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  McuHspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  McuHspi2.Init.CRCPolynomial = 0x7;
  McuHspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  McuHspi2.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  McuHspi2.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  McuHspi2.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  McuHspi2.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  McuHspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_02CYCLE;
  McuHspi2.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_02CYCLE;
  McuHspi2.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  McuHspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  McuHspi2.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  
  //if (HAL_SPI_Init(&McuHspi2) != HAL_OK)
  //{
	//Error_Handler();
  //}
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
 *@brief:      mcu_spi_init
 *@details:    初始化SPI控制器，暂时支持SPI3
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_hspi_init(const char *name, const SpiIoDef *io)
{
	SPI_DEBUG(LOG_DEBUG, ">----mcu_hspi_init\r\n");
	mcu_MX_SPI2_Init();
    return 0;
}

/**
 *@brief:      mcu_spi_open
 *@details:       打开SPI
 *@param[in]   SPI_DEV dev  ：SPI号
               u8 mode      模式
               u16 pre      预分频
 *@param[out]  无
 *@retval:     
 */
s32 mcu_hspi_open(const char *name, SPI_MODE mode, u16 KHz)
{

	if (node->gd != -1) {
		SPI_DEBUG(LOG_DEBUG, "spi dev busy\r\n");
		return -1;
	}
	
	if(mode >= SPI_MODE_MAX)
		return -1;
	
	if (strcmp(name, "SPI2") != 0) {
		return -1;
    }

	McuHspi2.Init.CLKPolarity = SpiModeSet[mode].CPOL;
  	McuHspi2.Init.CLKPhase = SpiModeSet[mode].CPHA;

	/* 根据传入的KHz 计算分频*/
	///@bug need fix
	McuHspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	
	if (HAL_SPI_Init(&McuHspi2) != HAL_OK) {
		Error_Handler();
  	}

	node->gd = 0;
		
    return 0;
}
/**
 *@brief:      mcu_spi_close
 *@details:    关闭SPI 控制器
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_hspi_close(const char *name)
{
    SPI_TypeDef* SPIC;

	
    return 0;
}
/**
 *@brief:      mcu_spi_transfer
 *@details:    SPI 传输
 *@param[in]   u8 *snd  
               u8 *rsv  
               s32 len  
 *@param[out]  无
 *@retval:     
 */
s32 mcu_hspi_transfer(const char *name, u8 *snd, u8 *rsv, s32 len)
{
    s32 i = 0;

	HAL_StatusTypeDef res;
	
    if ( ((snd == NULL) && (rsv == NULL)) || (len < 0) ) {
        return -1;
    }
	
	if (strcmp(name, "SPI2") != 0) {
		return -1;
	}
	
	if (snd == NULL)	{
		//SPI_DEBUG(LOG_DEBUG, "r ");
		res = HAL_SPI_Receive(&McuHspi2, rsv, len, 20);//need fix 超时要根据速度和数据定
	} else if(rsv == NULL) {	
		//SPI_DEBUG(LOG_DEBUG, "t ");
		res = HAL_SPI_Transmit(&McuHspi2, snd, len, 20);
	} else {
		//SPI_DEBUG(LOG_DEBUG, "tr ");
		res = HAL_SPI_TransmitReceive(&McuHspi2, snd, rsv, len, 20);
	}
	//SPI_DEBUG(LOG_DEBUG, "spi tra: %d\r\n", res);
	if(res != 0 )
		return -1;
	
	i = len;
    return i;
}


