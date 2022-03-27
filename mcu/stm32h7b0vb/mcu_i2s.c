
#include "mcu.h"
#include "log.h"

#include "audio_pipeline.h"



extern SAI_HandleTypeDef hsai_BlockB2;
extern DMA_HandleTypeDef hdma_sai2_b;


/*
	  __HAL_SAI_RESET_HANDLE_STATE(&SaiHandle);

	    __HAL_SAI_DISABLE(&SaiHandle);

	  __HAL_SAI_ENABLE(&SaiHandle);

*/

//void MX_SAI2_Init(void)
s32 mcu_i2s_dataformat(u32 Freq, u8 Standard, short BitsPerSample)
{

  /* USER CODE BEGIN SAI2_Init 0 */
	uart_printf("mcu_i2s_dataformat...\r\n");
  /* USER CODE END SAI2_Init 0 */

  /* USER CODE BEGIN SAI2_Init 1 */
	u16 standard;
	u16 dataformat;

	//#define SAI_PCM_SHORT            4U
	if (Standard == AUDIO_I2S_LSB) {
		standard = 	SAI_I2S_LSBJUSTIFIED;
	} else if(Standard == AUDIO_I2S_MSB) {
		standard = 	SAI_I2S_MSBJUSTIFIED;
	} else if(Standard == AUDIO_I2S_Phillips) {
		standard = 	SAI_I2S_STANDARD;
	} else if(Standard == AUDIO_I2S_PCM) {
		standard = 	SAI_PCM_LONG;
	} else {
		standard = 	SAI_I2S_STANDARD;	
	}

	if (BitsPerSample == 16) {
		dataformat = 	SAI_PROTOCOL_DATASIZE_16BIT;
	} else if(BitsPerSample == 20) {
		dataformat = 	SAI_PROTOCOL_DATASIZE_16BITEXTENDED;
	} else if(BitsPerSample == 24) {
		dataformat = 	SAI_PROTOCOL_DATASIZE_24BIT;
	} else if(BitsPerSample == 32) {
		dataformat = 	SAI_PROTOCOL_DATASIZE_32BIT;
	}

  /* USER CODE END SAI2_Init 1 */
  hsai_BlockB2.Instance = SAI2_Block_B;
  hsai_BlockB2.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockB2.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockB2.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLE;
  hsai_BlockB2.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockB2.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockB2.Init.AudioFrequency = Freq;
  hsai_BlockB2.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockB2.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB2.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB2.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  
  if (HAL_SAI_InitProtocol(&hsai_BlockB2, standard, dataformat, 2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SAI2_Init 2 */
	__HAL_SAI_DISABLE(&hsai_BlockB2);
  /* USER CODE END SAI2_Init 2 */

}

void mcu_i2s_dma_init(u16 *buffer,u32 len)
{
	if(HAL_OK != HAL_SAI_Transmit_DMA(&hsai_BlockB2, (uint8_t *)buffer, len))
  	{
    	Error_Handler();
  	}
}

/**
 *@brief:      mcu_i2s_dma_start
 *@details:    开始DMA传输
 *@param[in]   void  
 *@param[out]  无
 *@retval:     
 */
void mcu_i2s_dma_start(void)
{   	  
	__HAL_SAI_ENABLE(&hsai_BlockB2);
}
/**
 *@brief:      mcu_i2s_dma_stop
 *@details:       停止DMA传输
 *@param[in]  void  
 *@param[out]  无
 *@retval:     
 */
void mcu_i2s_dma_stop(void)
{   	 
	__HAL_SAI_DISABLE(&hsai_BlockB2);
}


/**
  * @brief Tx Transfer completed callbacks.
  * @param  hsai : pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SAI_TxCpltCallback could be implemented in the user file
   */
	audio_pipe_callback(1);

}

/**
  * @brief Tx Transfer Half completed callbacks
  * @param  hsai : pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SAI_TxHalfCpltCallback could be implenetd in the user file
   */
  audio_pipe_callback(0);

}




